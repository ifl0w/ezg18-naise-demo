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

// ADJUSTMENT VARIABLES
float near = 0.01;
float far  = 1000.0;

int maxIterations;
int maxBinarySearchIterations;

float maxRayDistance = 200; // view or worldspace
float stepsize; // in screenspace


// VARIABLES
vec2 resolution;

vec3 world_position;
vec3 view_normal;
vec3 view_position;
vec3 screen_position;
vec3 view_reflection;
vec3 view_rayMarchingStartPoint;
vec3 screen_rayMarchingStartPoint;
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
float linearizeDepth(float depthToVisualize);
vec3 world2viewSpace(vec3 position);
vec3 world2viewSpaceDirectionVector(vec3 dir);
vec4 view2screenSpace(vec3 position);
vec4 view2clipSpace(vec3 position);
vec4 NDC2screenSpace(vec4 position);
void calculateScreenReflection(vec3 view_pos, vec3 view_refl);
void testing();

bool skybox = false;
bool isHit = false;

// TODO: ray an laenge anpassen (near to far plane)?
void init(){
    maxIterations = 100;
    stepsize = 0.1;
    ViewAngleThreshold = 5;
    resolution = vec2(viewportWidth, viewportHeight);

    fragment_roughness = texture(gAlbedoRoughness, TexCoords).a;
    fragment_metallic =  texture(gEmissionMetallic, TexCoords).a;
    fragment_depth = texture(gPosition, TexCoords).a;
    world_position = texture(gPosition, TexCoords).rgb;

    view_position = world2viewSpace(world_position);
    view_normal =  world2viewSpaceDirectionVector(texture(gNormal, TexCoords).rgb);
    view_reflection = normalize(reflect(normalize(view_position.xyz), normalize(view_normal)));
    angle = degrees(acos(dot(normalize(view_reflection), normalize(view_normal))));

    calculateScreenReflection(view_position, view_reflection);
}

// TODO: should the linear or non linear depth be used?
// TODO: stepsize must get greater while stepping through the ray?
// TODO: ray im lipspace steppen? Transform the View Space Reflection to clip-space
vec4 RayMarching(){
    vec4 last_rayPosition = screen_point;
    vec4 screen_rayPosition = screen_point + screen_reflection;
    float rayDepth = 0; // accumulated depth value on rayPosition
    float texDepth = 0; // corresponding depth at the view_positionOnRay in the depth texture

    isHit = false;

    for(int i = 0; i < maxIterations; i++){
        ivec2 denormalizedTexCoords = ivec2(screen_rayPosition.xy * resolution);
        //in rayDepth & texDepth exakt der selbe wert wie fragment_depth wenn view_positionOnRay = view2screenSpace(view_position);
        rayDepth = screen_rayPosition.z;
        texDepth = texelFetch(gPosition, denormalizedTexCoords, 0).a;

        if(texDepth<=0){
         return vec4(0,1,0,1);
            //hit on skybox; TODO is early termination ok? probably not when the scene has flying/ hovering elements
            //Stop ray marching when the ray goes outside screen space
            return vec4(0);
        }

        if(rayDepth >= texDepth){
                // Correct the offset based on the sampled depth???
          //  screen_rayPosition.xy = last_rayPosition.xy + (texDepth - last_rayPosition.z ) * screen_reflection.xy;
            isHit = true;

            return vec4(screen_rayPosition.xy, 0.0, 1.0);
        }
        last_rayPosition = screen_rayPosition;
        screen_rayPosition += screen_reflection* stepsize;// * stepsize;
        if(screen_rayPosition.z > screen_endPoint.z){
            return vec4(0); //ray missed
        }

    }
    return vec4(0); //ray missed
}

vec4 blending(vec4 fetchedColor, vec4 coords){
    vec4 result = vec4(0);

    float AlphaCompositing_Alpha = 0; //accumulation of intensity-value in each direction
	vec4 AlphaCompositing_Color = vec4(0,0,0,0);
    float reflectorR = fragment_roughness;
    vec4 oldColor = vec4(0);
    oldColor.a = reflectorR;
    oldColor.rgb = texture(imageInput, TexCoords).rgb;
    ivec2 denormalizedTexCoords = ivec2(coords.xy * resolution);
    reflections = texelFetch(imageInput, denormalizedTexCoords, 0);
    float reflectedR = texelFetch(gAlbedoRoughness, denormalizedTexCoords, 0).a;
     vec4 newColor = fetchedColor;
     newColor.a = reflectedR;

    AlphaCompositing_Alpha = newColor.a + (1-newColor.a) * oldColor.a;

    AlphaCompositing_Color.rgb = (1/AlphaCompositing_Alpha) * ( oldColor.a * oldColor.rgb + ((1- oldColor.a)*newColor.a*newColor.rgb));

	AlphaCompositing_Color.a = AlphaCompositing_Alpha;
    return (oldColor + newColor) / 2;
     return vec4(1,0,0,1);
    return AlphaCompositing_Color;
}

void main(){
    init();
    vec4 result = vec4(0,0,0,0);

    // TODO: angle wider then 90 degree -> set reflection fully transparent
    // TODO: ViewAngleThreshold-> only count reflection if they are over the treshhold (angle between the normal and the camera direction)
    //if(angle < 90.0 && angle > ViewAngleThreshold)

    if(world_position.z != 0.0 && fragment_depth > 0.0 && fragment_roughness < 1){
        result = RayMarching();
       // reflections = result;
    }

    if(isHit){
    //TODO texelFetch like in debug texture
    vec2 test = smoothstep(0.2, 0.6, abs(vec2(0.5, 0.5) - result.xy));

float screenEdgefactor = clamp(1.0 - (test.x + test.y), 0.0, 1.0);
float reflectionSpecularFalloffExponent = 3.0;
    float ReflectionMultiplier = fragment_metallic;
    //pow(fragment_roughness, reflectionSpecularFalloffExponent) *
     //           screenEdgefactor; //* -screen_reflection.z;


    ivec2 denormalizedTexCoords = ivec2(result.xy * resolution);
    reflections = texelFetch(imageInput, denormalizedTexCoords, 0);//* clamp(ReflectionMultiplier, 0.7, 1);//TODO .a should be defined by roughness

    reflections = blending(reflections, result);
    } else {
     reflections = texture(imageInput, TexCoords);
     //reflections = vec4(fragment_metallic);
    }
}

void calculateScreenReflection(vec3 view_pos, vec3 view_refl){

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
         view_endPoint = view_pos + view_refl * maxRayDistance;
    }

    screen_point = view2screenSpace(view_pos);
    screen_endPoint = view2screenSpace(view_endPoint);
    // tip minus base (spitze minus schaft)
    screen_reflection = normalize(screen_endPoint - screen_point);


}

float linearizeDepth(float depthToVisualize){
    float d = depthToVisualize * 2.0 - 1.0;
    d = (2.0 * near * far) / (far + near - d * (far - near));
    return d/far;
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

// TODO delete later on
void testing(){
    /** TEST of worldNormals: **/
    // reflections = texture(gNormal, TexCoords); //passt
    /** TEST of viewNormals: **/
    // reflections = vec4(view_normal,1); //passt
    /** TEST of worldPosition: **/
    // reflections = vec4(world_position,1); //passt
    /** TEST of view_position: **/
    // reflections = vec4(view_position,1); //passt
    /** TEST of screen_position: **/ //TODO: skybox fucked up!!
    // reflections = vec4(screen_point.rgb,1); //clip und screen space passt
    /** TEST of view_reflection: **/
    // reflections = vec4(view_reflection,1); //passt
    /** TEST of screen_reflection: **/

  //TODO
 /*  if(screen_startPoint.w < 0.0){
       reflections = vec4(1);
   }*/
  // reflections = screen_startPoint;
   //reflections = screen_point;
   // reflections = vec4(0,0,normalize(view_reflection).b,1);
   //reflections = vec4(1,0,0,1);
   // reflections = normalize(screen_reflection);///0.5;
// reflections = screen_point + vec4(0.5,0.5,0,0);
    /** TEST of reflection Ray: **/
   // reflections = vec4(screen_reflection.rgb, length(screen_reflection.xy));
}
