#type vertex
#version 450 core

layout(binding = 0) uniform UniformBufferObject{
	mat4 viewproj;
} ubo;

layout(location = 0) in vec3 a_Position;
layout(location = 1) in vec3 a_Normal;
layout(location = 2) in vec2 a_Tangent;

layout(push_constant) uniform TransformUniform
{
	mat4 transform;
} u_TransformUniform;

// layout(location = 0) out vec3 v_Color;
// layout(location = 1) out vec2 v_TexCoord;

// vec2 positions[3] = vec2[](
// 	vec2(0.0, -0.5),
// 	vec2(0.5, 0.5),
// 	vec2(-0.5, 0.5)
// 	);

// ubo.viewproj
void main() {
	// gl_Position = vec4(positions[gl_VertexIndex], 0.0, 1.0);
	gl_Position = ubo.viewproj * vec4(a_Position, 1.0);
	// v_Color = a_Color;
	// v_TexCoord = a_TexCoord;
}


#type fragment
#version 450 core

// layout(binding = 1) uniform sampler2D u_Texture;

// layout(location = 0) in vec3 v_Color;
// layout(location = 1) in vec2 v_TexCoord;

layout(location = 0) out vec4 outColor;

void main() {
	outColor = vec4(1.0, 0.0, 0.0, 1.0);
	// outColor = vec4(v_TexCoord, 0.0, 1.0);
	// outColor = texture(u_Texture, v_TexCoord);
}