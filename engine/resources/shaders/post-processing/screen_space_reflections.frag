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
int steps;
float stepsize;
float angle; //angle in degree between view_reflection and view_normal
float ViewAngleThreshold; //in degree, angle smallerthan this are ignored

vec4 screen_point;
vec4 screen_startPoint;
vec4 screen_reflection;
vec3 view_startPoint;

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
    steps = 100;
    stepsize = 0.1;
    ViewAngleThreshold = 5;
    resolution = vec2(viewportWidth, viewportHeight);

    fragment_roughness = texture(gAlbedoRoughness, TexCoords).a;
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
    vec4 screen_rayPosition = screen_startPoint; // TODO * stepsize?
    float rayDepth = 0; // accumulated depth value on rayPosition
    float texDepth = 0; // corresponding depth at the view_positionOnRay in the depth texture

    isHit = false;

    for(int i = 0; i < steps; i++){
        //Stop ray marching when the ray goes outside screen space
        //TODO randbehandlung?
        if(screen_rayPosition.x < 0 || screen_rayPosition.x > 1 ||
           screen_rayPosition.y < 0 || screen_rayPosition.y > 1 ||
           screen_rayPosition.z < 0 || screen_rayPosition.z > 1){
            return vec4(0);
         }

        ivec2 denormalizedTexCoords = ivec2(screen_rayPosition.xy * resolution);
        //in rayDepth & texDepth exakt der selbe wert wie fragment_depth wenn view_positionOnRay = view2screenSpace(view_position);
        rayDepth = screen_rayPosition.z;
        texDepth = texelFetch(gPosition, denormalizedTexCoords, 0).a;

        if(texDepth<=0){
            //hit on skybox; TODO is early termination ok? probably not when the scene has flying/ hovering elements
            return vec4(0);
        } else if(rayDepth >= texDepth){
            isHit = true;
            return vec4(screen_rayPosition.xy, 0.0, 1.0);
        }
        screen_rayPosition += screen_reflection * stepsize;
    }
    return vec4(0); //ray missed
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
    reflections = texture(imageInput, result.xy);//TODO .a should be defined by roughness
    } else {
     reflections = texture(imageInput, TexCoords);
    }
 //reflections = RayMarching();
 if(reflections.a == 0){
     // reflections = texture(imageInput, TexCoords);
 }
    //testing();
}

void calculateScreenReflection(vec3 view_pos, vec3 view_refl){
    view_startPoint = view_pos + normalize(view_refl);
    screen_point = view2screenSpace(view_pos);
    screen_startPoint = view2screenSpace(view_startPoint);
    // tip minus base (spitze minus schaft)
    screen_reflection = normalize(screen_startPoint - screen_point);
}

float linearizeDepth(float depthToVisualize){
    float near = 0.01;
    float far  = 1000.0;

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
