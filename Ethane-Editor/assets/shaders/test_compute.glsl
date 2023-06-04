#type compute
#version 450 core

layout (local_size_x = 8, local_size_y = 8, local_size_z = 1) in;

layout (binding = 0) uniform ParameterUBO {
    float time;
	uint currentSample;
} ubo;


layout (set = 1, binding = 0, rgba8) uniform image2D colorBuffer;

// Input texture
layout(set = 1, binding = 1, rgba8) uniform readonly image2D inputTextureOne;
layout(set = 1, binding = 2, rgba8) uniform readonly image2D inputTextureTwo;

struct Line {
	vec2 start;
	vec2 end;
};

layout(std140, set = 1, binding = 3) readonly buffer ImageOneRefLines {
   Line imageOneRefLines[ ];
};

layout(std140, set = 1, binding = 4) readonly buffer ImageTwoRefLines {
   Line imageTwoRefLines[ ];
};


void drawLine(ivec2 startPoint, ivec2 endPoint, vec4 color) {
   
}

#include "/Users/ethan/ethans_folder/Program_dev/cg_final_project/Ethane-Editor/assets/shaders/include/random.glsl"

float randomRange(float min, float max) {
    // Returns a random real in [min,max).
    return min + (max-min)*random();
}


void main() {
    ivec2 screen_pos = ivec2(gl_GlobalInvocationID.x, gl_GlobalInvocationID.y);
    ivec2 screen_size = imageSize(colorBuffer);

	vec4 pixel_color;
	vec4 pixel_color_1 = imageLoad(inputTextureOne, screen_pos);
	vec4 pixel_color_2 = imageLoad(inputTextureTwo, screen_pos);

	// pixel_color = pixel_color_1 * ubo.time + pixel_color_2 * (1-ubo.time);
	float rnd = randomRange(0.0, 0.3);
	pixel_color = pixel_color_1 * (ubo.time + rnd) + pixel_color_2 * ((1-ubo.time) - rnd);

	for (int i = 0; i<imageOneRefLines.length(); i++)
	{ 
		if (imageOneRefLines[i].start.x < screen_pos.x && screen_pos.x < imageOneRefLines[i].end.x && imageOneRefLines[i].start.y < screen_pos.y && screen_pos.y < imageOneRefLines[i].end.y)
		{
			pixel_color = vec4(imageOneRefLines.length()/30, 0, 0, 1);
		}
		if (imageTwoRefLines[i].start.x < screen_pos.x && screen_pos.x < imageTwoRefLines[i].end.x && imageTwoRefLines[i].start.y < screen_pos.y && screen_pos.y < imageTwoRefLines[i].end.y)
		{
			pixel_color = vec4(0, 1, 0, 1);
		}
	}
	imageStore(colorBuffer, screen_pos, pixel_color);
}
