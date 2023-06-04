#type vertex
#version 450 core

layout(location = 0) in vec3 a_Position;
layout(location = 1) in vec3 a_Normal;
layout(location = 2) in vec2 a_Tangent;
layout(location = 3) in vec3 a_Binormal;
layout(location = 4) in vec2 a_TexCoord;


layout(location = 0) out vec3 FS_IN_FragPos;
layout(location = 1) out vec2 FS_IN_Texcoord;
layout(location = 2) out vec3 FS_IN_Normal;
layout(location = 3) out vec3 FS_IN_Tangent;
layout(location = 4) out vec3 FS_IN_Binormal;
layout(location = 5) out vec4 FS_IN_CSPos;
layout(location = 6) out vec4 FS_IN_PrevCSPos;


struct Light
{
    vec4 data0;
    vec4 data1;
    vec4 data2;
    vec4 data3;
};

// ------------------------------------------------------------------------
// DESCRIPTOR SETS --------------------------------------------------------
// ------------------------------------------------------------------------

layout(set = 0, binding = 0) uniform UniformBufferObject
{
    mat4  view_inverse;
    mat4  proj_inverse;
    mat4  view_proj_inverse;
    mat4  prev_view_proj;
    mat4  view_proj;
    vec4  cam_pos;
    vec4  current_prev_jitter;
    Light light;
}
u_GlobalUBO;

// ------------------------------------------------------------------------
// PUSH CONSTANTS ---------------------------------------------------------
// ------------------------------------------------------------------------

layout(push_constant) uniform PushConstants
{
    mat4  model;
    uint  material_idx;
    uint  mesh_id;
    float roughness_multiplier;
}
u_PushConstants;


// ubo.viewproj
void main() {
	// Transform position into world space
    vec4 world_pos      = u_PushConstants.model * vec4(a_Position, 1.0);

    // Since this demo has static scenes we can use the current Model matrix as the previous one
    vec4 prev_world_pos = u_PushConstants.model * vec4(a_Position, 1.0);

    // Transform world position into clip space
    gl_Position = u_GlobalUBO.view_proj * world_pos;

    // Pass world position into Fragment shader
    FS_IN_FragPos = world_pos.xyz;

    // Pass clip space positions for motion vectors
    FS_IN_CSPos     = gl_Position;
    FS_IN_PrevCSPos = u_GlobalUBO.prev_view_proj * prev_world_pos;

    // Pass texture coordinate
    FS_IN_Texcoord = a_Texcoord;

    // Transform vertex normal into world space
    mat3 normal_mat = mat3(u_PushConstants.model);

    FS_IN_Normal    = normal_mat * a_Normal;
    FS_IN_Tangent   = normal_mat * a_Tangent;
    FS_IN_Binormal = normal_mat * a_Binormal;
}


#type fragment
#version 450 core

// ------------------------------------------------------------------------
// INPUTS -----------------------------------------------------------------
// ------------------------------------------------------------------------

layout(location = 0) in vec3 FS_IN_FragPos;
layout(location = 1) in vec2 FS_IN_TexCoord;
layout(location = 2) in vec3 FS_IN_Normal;
layout(location = 3) in vec3 FS_IN_Tangent;
layout(location = 4) in vec3 FS_IN_Bitangent;
layout(location = 5) in vec4 FS_IN_CSPos;
layout(location = 6) in vec4 FS_IN_PrevCSPos;

// ------------------------------------------------------------------------
// OUTPUTS ----------------------------------------------------------------
// ------------------------------------------------------------------------

layout(location = 0) out vec4 FS_OUT_GBuffer1; // RGB: Albedo, A: Metallic
layout(location = 1) out vec4 FS_OUT_GBuffer2; // RG: Normal, BA: Motion Vector
layout(location = 2) out vec4 FS_OUT_GBuffer3; // R: Roughness, G: Curvature, B: Mesh ID, A: Linear Z

// ------------------------------------------------------------------------
// PUSH CONSTANTS ---------------------------------------------------------
// ------------------------------------------------------------------------

layout(push_constant) uniform PushConstants
{
    mat4  model;
    uint  material_idx;
    uint  mesh_id;
    float roughness_multiplier;
}
u_PushConstants;


// ------------------------------------------------------------------------
// FUNCTIONS --------------------------------------------------------------
// ------------------------------------------------------------------------

// A simple utility to convert a float to a 2-component octohedral representation
vec2 direction_to_octohedral(vec3 normal)
{
    vec2 p = normal.xy * (1.0f / dot(abs(normal), vec3(1.0f)));
    return normal.z > 0.0f ? p : (1.0f - abs(p.yx)) * (step(0.0f, p) * 2.0f - vec2(1.0f));
}

// ------------------------------------------------------------------------

vec2 compute_motion_vector(vec4 prev_pos, vec4 current_pos)
{
    // Perspective division, covert clip space positions to NDC.
    vec2 current = (current_pos.xy / current_pos.w);
    vec2 prev    = (prev_pos.xy / prev_pos.w);

    // Remap to [0, 1] range
    current = current * 0.5 + 0.5;
    prev    = prev * 0.5 + 0.5;

    // Calculate velocity (current -> prev)
    return (prev - current);
}

// ------------------------------------------------------------------------

float compute_curvature(float depth)
{
    vec3 dx = dFdx(FS_IN_Normal);
    vec3 dy = dFdy(FS_IN_Normal);

    float x = dot(dx, dx);
    float y = dot(dy, dy);

    return pow(max(x, y), 0.5f);
}


void main() {
	const Material material = Materials.data[u_PushConstants.material_idx];

    vec4 albedo = fetch_albedo(material, FS_IN_TexCoord);

    if (albedo.a < 0.1)
        discard;

    // G-Buffer 1
    FS_OUT_GBuffer1.rgb = albedo.rgb;
    FS_OUT_GBuffer1.a   = fetch_metallic(material, FS_IN_TexCoord);

    // G-Buffer 2
    vec2 packed_normal = direction_to_octohedral(fetch_normal(material, normalize(FS_IN_Tangent), normalize(FS_IN_Bitangent), normalize(FS_IN_Normal), FS_IN_TexCoord));
    vec2 motion_vector = compute_motion_vector(FS_IN_PrevCSPos, FS_IN_CSPos);

    FS_OUT_GBuffer2 = vec4(packed_normal, motion_vector);

    // G-Buffer 3
    float roughness = fetch_roughness(material, FS_IN_TexCoord) * u_PushConstants.roughness_multiplier;
    float linear_z  = gl_FragCoord.z / gl_FragCoord.w;
    float curvature = compute_curvature(linear_z);
    float mesh_id   = float(u_PushConstants.mesh_id);

    FS_OUT_GBuffer3 = vec4(roughness, curvature, mesh_id, linear_z);
}