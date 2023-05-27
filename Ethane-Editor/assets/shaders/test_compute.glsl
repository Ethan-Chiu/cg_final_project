#type compute
#version 450 core

layout (local_size_x = 8, local_size_y = 8, local_size_z = 1) in;

// Input texture
layout(binding = 0, rgba8) uniform readonly image2D inputTexture;

layout (binding = 1, rgba8) uniform image2D colorBuffer;

struct Sphere {
    vec3 center;
    float radius;
    vec3 color;
};

struct Camera {
    vec3 position;
    vec3 forwards;
    vec3 right;
    vec3 up;
};

struct Ray {
    vec3 origin;
    vec3 direction;
};

bool hit(Ray ray, Sphere sphere);

void main() {
    ivec2 screen_pos = ivec2(gl_GlobalInvocationID.x, gl_GlobalInvocationID.y);
    ivec2 screen_size = imageSize(colorBuffer);
    // float horizontalCoefficient = (float(screen_pos.x) * 2 - screen_size.x) / screen_size.x;
    // float verticalCoefficient = (float(screen_pos.y) * 2 - screen_size.y) / screen_size.x;

    // vec3 pixel_color = vec3(0.5, 0.0, 0.25);

    // Camera camera;
    // camera.position = vec3(0.0);
    // camera.forwards = vec3(1.0, 0.0, 0.0);
    // camera.right = vec3(0.0, -1.0, 0.0);
    // camera.up = vec3(0.0, 0.0, 1.0);

    // Sphere sphere;
    // sphere.center = vec3(3.0, 0.0, 0.0);
    // sphere.radius = 1.0;
    // sphere.color = vec3(1.0, 0.3, 0.7);

    // Ray ray;
    // ray.origin = camera.position;
    // ray.direction = camera.forwards + horizontalCoefficient * camera.right + verticalCoefficient * camera.up;

    // if (hit(ray, sphere)) {
    //     pixel_color = sphere.color;
    // }
	vec4 pixel_color = imageLoad(inputTexture, screen_pos);

	imageStore(colorBuffer, screen_pos, pixel_color);
    // imageStore(colorBuffer, screen_pos, vec4(pixel_color, 1.0));
}

bool hit(Ray ray, Sphere sphere) {

    float a = dot(ray.direction, ray.direction);
    float b = 2.0 * dot(ray.direction, ray.origin - sphere.center);
    float c = dot(ray.origin - sphere.center, ray.origin - sphere.center) - sphere.radius * sphere.radius;
    float discriminant = b*b - 4.0*a*c;

    return discriminant > 0;
}