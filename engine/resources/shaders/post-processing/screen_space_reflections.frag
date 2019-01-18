#version 430

layout (location = 0) out vec4 reflections;
in vec2 TexCoords;

layout(std140, binding = 0) uniform screenData
{
	int viewportWidth;
	int viewportHeight;
	int multiSampling;
	float brightnessFactor;
};

layout(std140, binding = 1) uniform projectionData
{
    mat4 viewProjection;    // size = 64B
    mat4 projectionMatrix;    // size = 64B
    mat4 viewMatrix;    // size = 64B
    vec3 cameraPosition;    // size = 16B
};

// in worldspace
uniform sampler2D gPosition;
uniform sampler2D imageInput;
uniform sampler2D gNormal;
uniform sampler2D gAlbedoRoughness;
uniform sampler2D gEmissionMetallic;
uniform sampler2D gLinearDepth;
uniform sampler2D gHiZ;

// ADJUSTMENT VARIABLES

// jitter: between 0 and 1 for how far to bump the ray in stride units to conceal banding artifacts
float jitter = 0.0;

float near = 0.01;

float maxIterations;
float maxBinarySearchIterations;

float maxRayDistance = 200; // view or worldspace
float stepsize; // in screenspace


// VARIABLES
vec2 resolution;

vec3 world_position;
vec3 view_normal;
vec3 view_position;
vec3 screen_position;
vec3 view_reflection;
float fragment_depth;
float fragment_roughness;
float fragment_metallic;
float angle; //angle in degree between view_reflection and view_normal
float ViewAngleThreshold; //in degree, angle smallerthan this are ignored

vec4 screen_point;
vec4 screen_endPoint;
vec4 screen_reflection;
vec3 view_endPoint;

// HELPER FUNCTIONS
vec3 world2viewSpace(vec3 position);
vec3 world2viewSpaceDirectionVector(vec3 dir);
vec4 view2screenSpace(vec3 position);
vec4 view2clipSpace(vec3 position);
vec4 NDC2screenSpace(vec4 position);
void calculateScreenReflection(vec3 view_pos, vec3 view_refl);
float rand(vec2 co);
vec4 blending(vec4 fetchedColor, vec4 coords);
vec4 BinarySearch(vec4 rayDir, vec4 hitCoord);
vec4 screenSpace2view(vec4 position);


bool isHit = false;
//ss than the depth buffer value but greater than the depth buffer value minus pixel thickness)
// TODO: ray an laenge anpassen (near to far plane)?
void init(){
    maxIterations = 200;//30;

    stepsize = 4;//1
    maxRayDistance = 200;//20;

    ViewAngleThreshold = 5;
    resolution = vec2(viewportWidth, viewportHeight);

    fragment_roughness = texture(gAlbedoRoughness, TexCoords).a;
    fragment_metallic =  texture(gEmissionMetallic, TexCoords).a;
    //fragment_depth = texture(gLinearDepth, TexCoords).r;
    world_position = texture(gPosition, TexCoords).rgb;
    fragment_depth = texture(gPosition, TexCoords).a;

   /* step_bias = (zzzz.z-maxDistance)/(maxmaxDistance-maxDistance);
    if(step_bias < 0){
        step_bias = 0;
    }
     stepsize = step_bias *(1.0 - 0.1) + 0.1;
    }*/

    view_position = world2viewSpace(world_position);
    view_normal =  world2viewSpaceDirectionVector(texture(gNormal, TexCoords).rgb);
    view_reflection = normalize(reflect(normalize(view_position.xyz), normalize(view_normal)));
    //same as:
  //   view_reflection = normalize(reflect(normalize(world_position-cameraPosition), normalize(world_normal)));
   //  view_reflection = normalize(world2viewSpaceDirectionVector(view_reflection));

     vec2 c = world_position.xy * 0.25;
    jitter = rand(c);
    //angle = degrees(acos(dot(normalize(view_reflection), normalize(view_normal))));

    calculateScreenReflection(view_position, view_reflection);
}

// TODO: should the linear or non linear depth be used?
// TODO: stepsize must get greater while stepping through the ray?
// TODO: ray im lipspace steppen? Transform the View Space Reflection to clip-space
vec4 RayMarching(){
    vec4 screen_rayPosition = screen_point;
  // screen_reflection.xyz *= jitter * 1;

    float rayDepth = 0; // accumulated depth value on rayPosition
    float texDepth = 0; // corresponding depth at the view_positionOnRay in the depth texture

screen_rayPosition += screen_reflection * stepsize;
    for(int i = 0; i < maxIterations; i++){

        ivec2 denormalizedTexCoords = ivec2(screen_rayPosition.xy * resolution);
        //in rayDepth & texDepth exakt der selbe wert wie fragment_depth wenn view_positionOnRay = view2screenSpace(view_position);
        texDepth = texelFetch(gPosition, denormalizedTexCoords, 0).a;

        if(texDepth == 1.0 || texDepth <= 0.0 ){ //skybox//hit on skybox; TODO is early termination ok? probably not when the scene has flying/ hovering elements
        // can 1.0 occure in another case? if yes then set clearcolor to -1
           // out-of-boundsdepth-buffersample -> will terminate iteration
            //Stop ray marching when the ray goes outside screen space
            return vec4(0);
        }

        vec4 world = screenSpace2view(screen_rayPosition); //correct
        rayDepth = world.z;
       // rayDepth = (screen_rayPosition.z);
       float pixelthickness = 0.001;
        if(rayDepth >= texDepth + 0.0001 ){// + 0.0000001 // && rayDepth < texDepth + 0.2
            isHit = true;

           // return BinarySearch(screen_reflection, screen_rayPosition);
            return vec4(screen_rayPosition.xy, rayDepth, 1.0);
            //  return vec4(screen_rayPosition.xy, 0.0, 1.0);
        }

        //stepsize = (stepsize/screen_rayPosition.z);
        screen_rayPosition += screen_reflection * stepsize;// * stepsize;
        if(screen_rayPosition.z > screen_endPoint.z){
            return vec4(0); //ray missed
        }
    }
    return vec4(0); //ray missed
}


void main(){
    init();
    vec4 result = vec4(0,0,0,0);

    // TODO: angle wider then 90 degree -> set reflection fully transparent
    // TODO: ViewAngleThreshold-> only count reflection if they are over the treshhold (angle between the normal and the camera direction)
    //if(angle < 90.0 && angle > ViewAngleThreshold)

  /*  if(fragment_depth > 1.0 && fragment_depth > 0.0 && fragment_roughness < 1){
        result = RayMarching();
    }*/

    if(fragment_depth < 1.0 && fragment_depth > 0.0 ){//&& fragment_roughness < 1
         //   result = RayMarching();
        }
        result = RayMarching();

    if(isHit){
        ivec2 denormalizedTexCoords = ivec2(result.xy * resolution);
        reflections = texelFetch(imageInput, denormalizedTexCoords, 0);
        //reflections = blending(reflections,result);
    } else {
        reflections = texture(imageInput, TexCoords);
    }
   // reflections = texture(gHiZ, TexCoords);
}

vec4 BinarySearch(vec4 rayDir, vec4 hitCoord)
{
    float texDepth;
    float rayDepth;

    float numBinarySearchSteps = 3;
    vec4 screen_rayPosition = hitCoord;

    for(int i = 0; i < numBinarySearchSteps; i++)
    {
         ivec2 denormalizedTexCoords = ivec2(screen_rayPosition.xy * resolution);
        //in rayDepth & texDepth exakt der selbe wert wie fragment_depth wenn view_positionOnRay = view2screenSpace(view_position);

        texDepth = texelFetch(gPosition, denormalizedTexCoords, 0).a;

         vec4 world = screenSpace2view(screen_rayPosition); //correct
        rayDepth = world.z;

        float dDepth =rayDepth - texDepth;

        rayDir *= 0.1;
        if(dDepth > 0.0){
            screen_rayPosition -= rayDir;
        } else {
            screen_rayPosition += rayDir;
        }
    }
    return vec4(screen_rayPosition.xy, rayDepth, 1.0);
}

void calculateScreenReflection(vec3 view_pos, vec3 view_refl){
    view_endPoint = view_pos + normalize(view_refl);
    screen_point = view2screenSpace(view_pos);
    screen_endPoint = view2screenSpace(view_endPoint);
    // tip minus base (spitze minus schaft)
    screen_reflection = (screen_endPoint - screen_point);

// clip ray with the near plane or by the defined maxRayDistance
   if((view_pos.z + view_refl.z * maxRayDistance) > -near){
          // in viewspace zhe camera looks along the - z axis, therefore the nearPlane lies on a negativ position
        // ray passes near plane and maxDistance has to be redefined so the zPosition of the ray endpoint will lay on and not behind the nearplane

        // distance + the view_position.z has to result in -near
        // equation: -near = view_pos + X; X = -near - view_pos;
        float distance = -near - view_pos.z;
        // endPoint should sill lay on the view_refl on depth -near
        view_endPoint = view_pos + view_refl * (distance/view_refl.z);
    } else {
         view_endPoint = view_pos + normalize(view_refl) * maxRayDistance;
    }
     screen_endPoint = view2screenSpace(view_endPoint);

    float reflectScale = (1.0/resolution.x) / length(screen_reflection.xy);
  screen_reflection *= reflectScale;

    /*    float reflectScale = length((screen_reflection.xy)*(vec2(1.0)/resolution));
    screen_reflection *= reflectScale;*/


     //screen_reflection.xyz *= 1.0/resolution.x;

}


vec4 blending(vec4 fetchedColor, vec4 coords){
    return ( texture(imageInput, TexCoords) + fetchedColor) * 0.5;
}


vec3 world2viewSpace(vec3 position){
    vec3 result = (viewMatrix * vec4(position, 1)).xyz;
    return result;
}

vec3 world2viewSpaceDirectionVector(vec3 dir){
    vec3 result = (transpose(inverse(viewMatrix)) * vec4(dir, 1)).xyz;
    return result;
}

vec4 view2screenSpace(vec3 position){
    vec4 result = projectionMatrix * vec4(position, 1.0);
    result.xyz /= result.w;
    // NDC -> (0,1) so it can be used as TexCoords
    result.xyz = result.xyz * 0.5 + 0.5;
    //result.w = 1.0;
    return result;
}

vec4 view2clipSpace(vec3 position){
    vec4 result = projectionMatrix * vec4(position, 1.0);
    result.xyz /= result.w;
    return result;
}

vec4 NDC2screenSpace(vec4 position){
    vec4 result = position;
    result.xyz = result.xyz * 0.5 + 0.5;
    return result;
}

vec4 screenSpace2view(vec4 position){
    vec4 result = position;
    result.xyz = result.xyz * 2 - 1;
    result.xyz *= result.w;
    result = inverse(projectionMatrix) * result * -1;
     //result = inverse(viewMatrix) *result;
    return result;
}

float rand(vec2 co){
    return fract(sin(dot(co.xy ,vec2(12.9898,78.233))) * 43758.5453);
}
