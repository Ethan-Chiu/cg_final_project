#type vertex
#version 450 core

layout(location = 0) in vec3 a_Position;

layout(std140, binding = 0) uniform Camera
{
	mat4 u_InverseVP;
};

layout(location = 0)  out vec3 v_Position;

void main()
{
	gl_Position = vec4(a_Position.xy, 1.0, 1.0);

	v_Position = (u_InverseVP * position).xyz;
}




#type fragment
#version 450 core

layout(location = 0) out vec4 color;

layout(binding = 0) uniform uniform samplerCube u_Texture;

layout(location = 0)  in vec3 v_Position;

layout(location = 1) uniform UniformBuffer
{
	float u_TextureLod;
};

void main()
{
	color = textureLod(u_Texture, v_Position, u_TextureLod);
}
