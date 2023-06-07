#type compute
#version 450 core

layout (local_size_x = 8, local_size_y = 8, local_size_z = 1) in;

layout (binding = 0) uniform ParameterUBO {
    float ratio;
	uint currentSample;
    uint lineNum;
    float time;
    vec3 camPos;
} ubo;


layout (set = 1, binding = 0, rgba8) uniform image2D colorBuffer;

// Input texture
layout(set = 1, binding = 1, rgba8) uniform readonly image2D inputTextureOne;
layout(set = 1, binding = 2, rgba8) uniform readonly image2D inputTextureTwo;

struct Line {
	vec2 start;
	vec2 end;
    float len;
};

layout(std140, set = 1, binding = 3) readonly buffer ImageOneRefLines {
   Line imageOneRefLines[ ];
};

layout(std140, set = 1, binding = 4) readonly buffer ImageTwoRefLines {
   Line imageTwoRefLines[ ];
};

layout(std140, set = 1, binding = 5) readonly buffer WarpLines {
   Line warpLines[ ];
};


void drawLine(ivec2 startPoint, ivec2 endPoint, vec4 color) {
   
}

//#include "/Users/ethan/ethans_folder/Program_dev/cg_final_project/Ethane-Editor/assets/shaders/include/random.glsl"

//float randomRange(float min, float max) {
//    // Returns a random real in [min,max).
//    return min + (max-min)*random();
//}


void main() {
    ivec2 screen_pos = ivec2(gl_GlobalInvocationID.x, gl_GlobalInvocationID.y);
    ivec2 image_size = imageSize(inputTextureOne);
    int width = image_size.x;
    int height = image_size.y;
    
    vec2 sum_1 = vec2(0);
    vec2 sum_2 = vec2(0);
    vec4 scalar = vec4(0);
    
    if (ubo.ratio == 0) {
        imageStore(colorBuffer, screen_pos, imageLoad(inputTextureOne, screen_pos));
        for (int i = 0; i<ubo.lineNum; i++)
        {
            if ((screen_pos.x >= imageOneRefLines[i].start.x && screen_pos.x <= imageOneRefLines[i].end.x) || (screen_pos.x <= imageOneRefLines[i].start.x && screen_pos.x >= imageOneRefLines[i].end.x)) {
                float dy = imageOneRefLines[i].end.y - imageOneRefLines[i].start.y;
                float expecty = imageOneRefLines[i].start.y + dy * (screen_pos.x - imageOneRefLines[i].start.x) / (imageOneRefLines[i].end.x - imageOneRefLines[i].start.x);
                if (abs(expecty - screen_pos.y) < 1) {
                    imageStore(colorBuffer, screen_pos, vec4(0, 1, 0, 1));
                }
            }
        }
    } else if (ubo.ratio == 1) {
        imageStore(colorBuffer, screen_pos, imageLoad(inputTextureTwo, screen_pos));
        for (int i = 0; i<ubo.lineNum; i++)
        {
            if ((screen_pos.x >= imageTwoRefLines[i].start.x && screen_pos.x <= imageTwoRefLines[i].end.x) || (screen_pos.x <= imageTwoRefLines[i].start.x && screen_pos.x >= imageTwoRefLines[i].end.x)) {
                float dy = imageTwoRefLines[i].end.y - imageTwoRefLines[i].start.y;
                float expecty = imageTwoRefLines[i].start.y + dy * (screen_pos.x - imageTwoRefLines[i].start.x) / (imageTwoRefLines[i].end.x - imageTwoRefLines[i].start.x);
                if (abs(expecty - screen_pos.y) < 1) {
                    imageStore(colorBuffer, screen_pos, vec4(1, 0, 0, 1));
                }
            }
        }
    } else {
        for (int i = 0; i<ubo.lineNum; i++)
        {
            if (imageOneRefLines[i].start.x != 0 && imageOneRefLines[i].start.y != 0 && imageOneRefLines[i].end.x != 0 && imageOneRefLines[i].end.y != 0) {
                vec2 X_P = screen_pos - warpLines[i].start;
                vec2 Q_P = warpLines[i].end - warpLines[i].start;
                vec2 Perp_Q_P = vec2(Q_P.y, -Q_P.x);
                float u = dot(X_P, Q_P) / warpLines[i].len / warpLines[i].len;
                float v = (X_P.x * Q_P.y - X_P.y * Q_P.x) / warpLines[i].len;
                sum_1 += imageOneRefLines[i].start + u * Q_P + (v / imageOneRefLines[i].len) * Perp_Q_P;
                sum_2 += imageTwoRefLines[i].start + u * Q_P + (v / imageTwoRefLines[i].len) * Perp_Q_P;
            }
        }
        if (screen_pos.x < width && screen_pos.y < height) {
            sum_1.x = clamp(sum_1.x / ubo.lineNum, 0, width-1);
            sum_1.y = clamp(sum_1.y / ubo.lineNum, 0, height-1);
            sum_2.x = clamp(sum_2.x / ubo.lineNum, 0, width-1);
            sum_2.y = clamp(sum_2.y / ubo.lineNum, 0, height-1);
            int x_floor = int(floor(sum_1.x));
            int y_floor = int(floor(sum_1.y));
            int x_ceil = min(x_floor + 1, width - 1);
            int y_ceil = min(y_floor + 1, height - 1);
            float dx = sum_1.x - x_floor;
            float dy = sum_1.y - y_floor;
            scalar += (1-ubo.ratio) * ((1 - dx) * (1 - dy) * imageLoad(inputTextureOne, ivec2(x_floor, y_floor)) +
                                       dx * (1 - dy) * imageLoad(inputTextureOne, ivec2(x_ceil, y_floor)) +
                                       dx * dy * imageLoad(inputTextureOne, ivec2(x_ceil, y_ceil)) +
                                       (1 - dx) * dy * imageLoad(inputTextureOne, ivec2(x_floor, y_ceil)));
            x_floor = int(floor(sum_2.x));
            y_floor = int(floor(sum_2.y));
            x_ceil = min(x_floor + 1, width - 1);
            y_ceil = min(y_floor + 1, height - 1);
            dx = sum_2.x - x_floor;
            dy = sum_2.y - y_floor;
            scalar += ubo.ratio * ((1 - dx) * (1 - dy) * imageLoad(inputTextureTwo, ivec2(x_floor, y_floor)) +
                                   dx * (1 - dy) * imageLoad(inputTextureTwo, ivec2(x_ceil, y_floor)) +
                                   dx * dy * imageLoad(inputTextureTwo, ivec2(x_ceil, y_ceil)) +
                                   (1 - dx) * dy * imageLoad(inputTextureTwo, ivec2(x_floor, y_ceil)));
            imageStore(colorBuffer, screen_pos, scalar);
        }
    }
}
