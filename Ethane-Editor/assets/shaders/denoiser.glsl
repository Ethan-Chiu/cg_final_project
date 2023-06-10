#type compute
#version 450

layout(local_size_x = 8, local_size_y = 8, local_size_z = 1) in;

//layout(binding = 0) uniform UniformBufferObject {
//    float ratio;
//    uint currentSample;
//    uint lineNum;
//    float time;
//    vec3 camPos;
//} ubo;

layout(set = 1, binding = 1, rgba8) uniform image2D targetTexture;

#define kernelSize 2
void main()
{
    vec2 imageSize = vec2(imageSize(targetTexture));
    ivec2 screen_pos = ivec2(gl_GlobalInvocationID.x, gl_GlobalInvocationID.y);
    
    vec3 result = vec3(0.0);
    for (int i = -int(kernelSize); i <= int(kernelSize); i++) {
        for (int j = -int(kernelSize); j <= int(kernelSize); j++) {
            ivec2 offset = ivec2(i, j);
            vec3 color = imageLoad(targetTexture, screen_pos + offset).rgb;
            
            result += color;
        }
    }

    result = result / ( 4 * kernelSize * kernelSize + 4 * kernelSize + 1 );
    imageStore(targetTexture, ivec2(gl_GlobalInvocationID.xy), vec4(result, 1));
}
