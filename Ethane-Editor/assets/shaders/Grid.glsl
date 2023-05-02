#type vertex
#version 450 core

layout(location = 0) in vec3 a_Position;
layout(location = 1) in vec2 a_TexCoord;

layout(location = 0) out vec2 v_TexCoord;

layout(std140, set = 0, binding = 0) uniform UniformBufferObject{
	mat4 viewproj;
} ubo;


mat4 transform = mat4(
8.0, 0.0,  0.0, 0.0,
0.0, 0.0, -8.0, 0.0,
0.0, 8.0,  0.0, 0.0,
0.0, 0.0,  0.0, 1.0);

void main()
{
	gl_Position = ubo.viewproj * transform * vec4(a_Position, 1.0);

	v_TexCoord = a_TexCoord;
}

#type fragment
#version 450 core

layout(location = 0) in vec2 v_TexCoord;

layout(location = 0) out vec4 color;

float grid(vec2 st, float res)
{
	vec2 grid = fract(st);
	return step(res, grid.x) * step(res, grid.y);
}

void main()
{
	float scale = 16.025;
	float resolution = 0.025;

	float x = grid(v_TexCoord * scale, resolution);
	color = vec4(vec3(0.2), 0.5) * (1.0 - x);
}
