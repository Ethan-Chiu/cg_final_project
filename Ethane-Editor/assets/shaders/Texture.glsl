#type vertex
#version 450 core

layout(location = 0) in vec3 a_Position;
layout(location = 1) in vec4 a_Color;
layout(location = 2) in vec2 a_TexCoord;
layout(location = 3) in float a_TexIndex;
layout(location = 4) in float a_TilingFactor;
// layout(location = 5) in int a_EntityID;

layout(binding = 0) uniform UniformBufferObject{
	mat4 viewproj;
} ubo;

struct VertexOutput
{
	vec4 Color;
	vec2 TexCoord;
	float TexIndex;
	float TilingFactor;
};

layout(location = 0) out VertexOutput Output;
// layout(location = 4) out flat int v_EntityID;

void main()
{
	Output.Color = a_Color;
	Output.TexCoord = a_TexCoord;
	Output.TexIndex = a_TexIndex;
	Output.TilingFactor = a_TilingFactor;
	// v_EntityID = a_EntityID;

	gl_Position = ubo.viewproj * vec4(a_Position, 1.0);
}




#type fragment
#version 450 core

layout(location = 0) out vec4 color;

struct VertexOutput
{
	vec4 Color;
	vec2 TexCoord;
	float TexIndex;
	float TilingFactor;
};

layout(location = 0) in VertexOutput Input;
// layout(location = 4) in flat int v_EntityID;

layout(binding = 1) uniform sampler2D u_Textures[32];

void main()
{
	color = texture(u_Textures[int(Input.TexIndex)], Input.TexCoord * Input.TilingFactor) * Input.Color;

	// color2 = v_EntityID;
}
