#type compute
#version 450

layout(local_size_x = 8, local_size_y = 8, local_size_z = 1) in;

// Include definitions for ubo, triangle, material, etc.
#include "/Users/ethan/ethans_folder/Program_dev/cg_final_project/Ethane-Editor/assets/shaders/include/definitions.glsl"

layout(binding = 0) uniform UniformBufferObject {
    float ratio;
    uint currentSample;
    uint lineNum;
    float time;
    vec3 camPos;
} ubo;

layout(set = 1, binding = 1, rgba8) uniform image2D targetTexture;

//layout(set = 1, binding = 2, rgba8) uniform image2D accumulationTex;

layout(std430, set = 1, binding = 3) readonly buffer TriangleBufferObject {
    triangle[] triangles;
 };

 layout(std430, set = 1, binding = 4) readonly buffer MaterialBufferObject {
    material[] materials;
 };

layout(std430, set = 1, binding = 5) readonly buffer AabbBufferObject {
    bvhNode[] bvh;
 };

layout(std430, set = 1, binding = 6) readonly buffer LightsBufferObject {
    light[] lights;
 };


// Random functions
 #include "/Users/ethan/ethans_folder/Program_dev/cg_final_project/Ethane-Editor/assets/shaders/include/random.glsl"

struct ray {
    vec3 origin;
    vec3 dir;
};

vec3 ray_at(ray r, float t) {
    return r.origin + t * r.dir;
}

struct hit_record {
    vec3 p;
    vec3 normal;
    uint materialIndex;
    float t;
    // integer to decrease branches.
    int backFaceInt;
    // PDF of scattering in given direction due to material properties.
    float scatterPdf;
    // PDF of sampling in given direction.
    float samplePdf;

};

vec3 randomOnATriangle(uint triangleIndex) {
    float s = random();
    float t = random();

    vec3 v01 = - triangles[triangleIndex].v0 + triangles[triangleIndex].v1;
    vec3 v02 = - triangles[triangleIndex].v0 + triangles[triangleIndex].v2;
    return triangles[triangleIndex].v0 + s * v01 + t * v02;
}

vec3 sampleLight(vec3 p, inout float pdf, inout float lightCosine) {
    int lightIndex = int(floor(lights.length() * random()));
    vec3 onLight = randomOnATriangle(lights[lightIndex].triangleIndex);
    vec3 toLight = onLight - p;
    float distanceSquared = dot(toLight, toLight);
    toLight = normalize(toLight);
    lightCosine = abs(toLight.y);

    // This assumes that all lights have the same area. 
    // TODO: Calculate probability for arbitrary light area.
    pdf = distanceSquared / (lights.length() * lightCosine * lights[lightIndex].area);

    return toLight;
}

vec3 sampleLambertian(vec3 normal, inout float pdf) {
    onb uvw = Onb(normal);
    vec3 randomCos = random_cosine_direction();
    vec3 randomCosScattered = normalize(onbLocal(randomCos, uvw));
    float cosine = dot(normalize(normal), randomCosScattered);
    pdf = max(0.01, cosine)/pi;
    
    return randomCosScattered;
}

vec3 sampleMetal(vec3 I, vec3 normal, inout float pdf) {
    pdf = 1;
    return reflect(I, normal);
}

vec3 sampleGlass(vec3 I, hit_record rec, inout float pdf) {
    pdf = 1;
    float ir = 1.5;
    float refraction_ratio = (1 - rec.backFaceInt) * 1.0/ir + rec.backFaceInt * ir; 
    vec3 i = normalize(I);
    float cos_theta = min(dot(-i, rec.normal), 1.0);
    float sin_theta = sqrt(1.0 - cos_theta*cos_theta);
    float t = floor(clamp(refraction_ratio * sin_theta, 0, 1));

    return t*reflect(i, rec.normal) + (1-t)*refract(i, rec.normal, refraction_ratio);
}

bool scatter(ray r_in, inout hit_record rec, inout vec3 albedo, inout ray scattered) {
    albedo = materials[rec.materialIndex].albedo;
    //float lightSamplePdf;

    float materialSamplePdf = 0;
    vec3 materialSample = vec3(0);

    if(materials[rec.materialIndex].materialType == LAMBERTIAN_MATERIAL) {
        materialSample = sampleLambertian(rec.normal, materialSamplePdf);
    }
    else if(materials[rec.materialIndex].materialType == METAL_MATERIAL) {
        materialSample = sampleMetal(r_in.dir, rec.normal, materialSamplePdf);
    }
    else if(materials[rec.materialIndex].materialType == GLASS_MATERIAL) {
        materialSample = sampleGlass(r_in.dir, rec, materialSamplePdf);
        albedo = vec3(1.0);
    }
    float lightSamplePdf = materialSamplePdf;
    vec3 finalSample = materialSample;

    if (random() < 0.5 && materials[rec.materialIndex].materialType == LAMBERTIAN_MATERIAL) {
        // sample light
        float lightCosine = 0;
        finalSample = sampleLight(rec.p, lightSamplePdf, lightCosine);

        if(abs(lightCosine) < 0.001) {
            finalSample = materialSample;
            lightSamplePdf = materialSamplePdf;
        }
    }

    scattered = ray(rec.p, finalSample);

    rec.samplePdf = 0.5 * materialSamplePdf + 0.5 *lightSamplePdf;
    rec.scatterPdf = materialSamplePdf;

    return materials[rec.materialIndex].materialType == LIGHT_MATERIAL;
}

vec3 triIntersect( in vec3 ro, in vec3 rd, triangle tri, inout vec3 n )
{
    vec3 a = tri.v0 - tri.v1;
    vec3 b = tri.v2 - tri.v0;
    vec3 p = tri.v0 - ro;
    n = cross( b, a );

    vec3 q = cross( p, rd );

    float idet = 1.0/dot( rd, n );

    float u = dot( q, b )*idet;
    float v = dot( q, a )*idet;
    float t = dot( n, p )*idet;

    return vec3( t, u, v );
}

bool hit_triangle(int triangle_index, ray r, float tMin, float tMax, inout hit_record rec) {
    triangle t = triangles[triangle_index];
    vec3 n = vec3(0,0,0);
    vec3 hit = triIntersect(r.origin, r.dir, t, n);
    if (!( hit.y<0.0 || hit.y>1.0 || hit.z<0.0 || (hit.y+hit.z)>1.0 )) {
        rec.p = r.origin + hit.x * r.dir;
        rec.normal =  normalize(n);
        rec.backFaceInt = dot(r.dir,rec.normal) > 0 ? 1 : 0;
        rec.normal *=  1 - 2 * rec.backFaceInt;
        rec.p +=  rec.normal*0.0001;
        rec.t = hit.x;
        rec.materialIndex = t.materialIndex;
        return hit.x > tMin && hit.x < tMax;
    }
    return false;
}

// no intersection means vec.x > vec.y (really tNear > tFar)
vec2 intersectAABB(ray r, vec3 boxMin, vec3 boxMax) {
    vec3 tMin = (boxMin - r.origin) / r.dir;
    vec3 tMax = (boxMax - r.origin) / r.dir;
    vec3 t1 = min(tMin, tMax);
    vec3 t2 = max(tMin, tMax);
    float tNear = max(max(t1.x, t1.y), t1.z);
    float tFar = min(min(t2.x, t2.y), t2.z);
    return vec2(tNear, tFar);
}


#define MAX_STACK_DEPTH 16
bool hit_bvh(ray r, inout hit_record rec) {
    float t_min = 0.001;
    float t_max = 10000;

    bool hit_anything = false;
    float closest_so_far = t_max;
    
    //Since shader doesn't have a stack structure, implementing it with an array and a counter.
    int nodeStack[MAX_STACK_DEPTH];
    int stackIndex = 0;

    // Traversing a flattened bvh using a stack.
    // nodeStack[stackIndex] contains an index of AABB in bhv[]
    // bvh[nodeStack[stackIndex]] is an index of a triangle in triangles.
    nodeStack[stackIndex] = 0;
    stackIndex++;

    while (stackIndex>0 && stackIndex < MAX_STACK_DEPTH) {
        stackIndex--;
        int currentNode = nodeStack[stackIndex];
        if(currentNode == -1) continue;

        vec2 tIntersect = intersectAABB(r, bvh[currentNode].min, bvh[currentNode].max);
        if (tIntersect.x > tIntersect.y) continue;
        
        // Idndex of triangle in current node.
        int ti = bvh[currentNode].objectIndex;
        if(ti != -1){
            hit_record temp_rec;
            if (hit_triangle(ti, r, t_min, closest_so_far, temp_rec)) {
                hit_anything = true;
                closest_so_far = temp_rec.t;
                rec = temp_rec;
            }
        }

        // Pushing both left unto the stack even if they are -1 to reduce branches. 
        // if (bvh[currentNode].leftNodeIndex != -1) {
        nodeStack[stackIndex] = bvh[currentNode].leftNodeIndex;
        stackIndex++;
        //}
        // if (bvh[currentNode].rightNodeIndex != -1) {
        nodeStack[stackIndex] = bvh[currentNode].rightNodeIndex;
        stackIndex++;
        //}
    }
    
    return hit_anything;
}

#define NUM_BOUNCES 3
vec3 ray_color(ray r) {
    vec3 unit_direction = normalize(r.dir);
    hit_record rec;

    vec3 final_color = vec3(1.0);
    ray current_ray = {r.origin, normalize(r.dir)};
    
    for (int i = 0; i< NUM_BOUNCES; i++) {
        if (hit_bvh(current_ray, rec)) {
            vec3 albedo;
            bool emits = scatter(current_ray, rec, albedo, current_ray);
            final_color *= albedo;
            if (emits) {
                break;
            }
        } else {
            float t = 0.5*(unit_direction.y + 1.0);
            final_color *= (1.0-t)*vec3(1.0, 1.0, 1.0) + t*vec3(1.0, 0.1, 1.0);
            break;
        }
    }
    return final_color;
}

void main()
{   
    // Image
    vec2 imageSize = vec2(imageSize(targetTexture));

    // Camera
    float vfov = 30;
    float theta = vfov * pi / 180.0;
    float h = tan(theta/2);
    float viewport_height = 2.0 * h;
    float viewport_width = imageSize.x / imageSize.y * viewport_height;
    float focal_length = 1.0;

    vec3 horizontal = vec3( viewport_width, 0,  0);
    vec3 vertical = vec3(0, -viewport_height, 0);

    vec3 origin = ubo.camPos.xyz * vec3(1, 1, 1);
    vec3 lower_left_corner = origin - horizontal/2 - vertical/2 - vec3(0, 0, focal_length);

    vec2 uv = (gl_GlobalInvocationID.xy);
    
    vec3 pixel_color = vec3(0);
    for (int i = 0; i < 2; i++) {
        ray r = {origin, lower_left_corner + (uv.x + random())*horizontal/imageSize.x + (uv.y + random())*vertical/imageSize.y - origin};
        pixel_color += ray_color(r);
    }

//    vec4 currentColor = imageLoad(accumulationTex, ivec2(gl_GlobalInvocationID.xy)).rgba * min(ubo.currentSample, 1.0);
//
//    vec4 to_write = (vec4(pixel_color, 1.0) + currentColor*(ubo.currentSample)) / (ubo.currentSample+1.0);

    imageStore(targetTexture, ivec2(gl_GlobalInvocationID.xy), vec4(pixel_color/2, 1));
}
