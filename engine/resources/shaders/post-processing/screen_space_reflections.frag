#version 430

layout (location = 1) out vec4 reflections;

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
uniform sampler2D gHiZ; //TODO last mipmaplevel isnt correct
uniform int maxMipmapLevel; //starts to count by 0

/*
min-max tracing:
What this gives you is a better estimation of the depth of the object at the pixel you're currently processing. You can use this in the ray-tracing pass to walk behind an object - if the current ray depth (from O + D * t) is outside the range of [min, max] at that pixel, you know it's not intersecting and can continue marching along that ray without further processing at the current position.
 "wrong" ray hits.  This wasn't talked about in the article, or maybe I missed it, but the hiZ trace can return you a screen space position that is incorrect for the reflection.  Think of a ray going behind an object floating above the ground.  The Z position of the ray will be far in back of Z position of the object, but the hiZ trace will return you the screen space position of where the ray and object first intersect.  In this case, you need to understand it's a nonsensical intersection, and keep tracing.  This is also where the max part of the min/max buffer comes in handy, since you will now be "behind" the object, which the implementation in the book doesn't cover.
*/
vec3 P_ss;
vec3 V_ss;
bool overmaxIterations = false;

// ADJUSTMENT VARIABLES
#define HIZ_START_LEVEL 2
#define HIZ_STOP_LEVEL 0 //0.0 //shouldn't be too height because of artefacts
#define HIZ_MAX_LEVEL maxMipmapLevel
vec2 HIZ_CROSS_EPSILON; //TODO divide by the resolution?
#define HIZ_MAX_ITERATIONS 256
#define HIZ_ITERATION_STEP 2 //was fix at 2.0 in paper

vec2 hiZSize; // = resolution;

// jitter: between 0 and 1 for how far to bump the ray in stride units to conceal banding artifacts
float jitter = 0.0;
float near = 0.01;

float maxIterations;
float maxBinarySearchIterations;

float maxRayDistance = 200; // view or worldspace
float stepsize; // in screenspace


// VARIABLES
vec2 resolution;

float linearDepth;
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
vec3 world_normal;

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
vec3 hiZTrace(vec3 p, vec3 v);

/*
Transform the point from screen-space into view-space.
Inverse-projection matrix must be in left-handed coordinate system.
*/
vec3 UnprojectPoint(vec3 screenPoint)
{
	screenPoint.xy = (screenPoint.xy - vec2(0.5)) * vec2(2.0, 2.0);

  //  screenPoint /= gl_FragCoord.w;
/// gl_FragCoord.w;

	vec4 projPos = inverse(projectionMatrix) * vec4(screenPoint, 1.0);
    return projPos.xyz/projPos.w;
   // return projPos.xyz;
}

// HIZ
// getResolutionOfMipmap
vec2 getCellCountOfMipmapLevel(int level){

    return vec2(textureSize(gHiZ,level));
    // getTexture size from glsl TODO
    // for each mipmapLevel the resolution gets divide in two
    // TODO optimize -> no if / else
    if(level == 0.0){
        return resolution;
    } else {
        //return floor(resolution / exp2(level)); // round .xy to int
       return (resolution / exp2(level));
    }
}

vec2 getCell(vec2 ray, vec2 mipmap_resolution){
    // xy coordinates of intersected cell in the mipmap
    // for example ray = (0.5, 0.25) and mipmap_resolution = (1000, 500)
    // -> cell = (500, 125)
    //TODO floor ok? test both
    return floor(ray * mipmap_resolution); // round .xy to int
}

/**
Intersects the closest plane, returns = O + D * t only
Interpolation between O and O + D

O: Origin of the ray at the near plane -> Pss + D * -Pssz
D: Reflection Direction
t: interpolationfactor between 0 and 1
**/
//IntersectDepthPlane
vec3 intersectPlane(vec3 O, vec3 D, float t)
{
	return O + D * t;
}

/*
This function computes the new ray position where
the ray intersects with the specified cell's boundary.
*/
// cell index refers to the bottom-left corner of a cell
//Line Rasterization!!!!
vec3 intersectCellBoundary(vec3 O, vec3 D, vec2 oldCellIndex, vec2 mipmap_resolution, vec2 crossStep, vec2 crossOffset)
{
   /* "delta" is a vector pointing from the origin of the ray to "index" which is a point along the ray between "o" and "d" (where z = 0 at o and z = 1 at d).
    Dividing delta by d.xy gives you a percentage of how far along the ray each component is.  You take the minimum of the two to decide how far to move between
    "o" and "d".*/

     vec2 texelSize;
    texelSize.x = 1.0 / mipmap_resolution.x;
    texelSize.y  = 1.0 / mipmap_resolution.y;
    HIZ_CROSS_EPSILON = vec2( texelSize.x,  texelSize.y) * 0.5;
    //HIZ_CROSS_EPSILON = (vec2(1)/mipmap_resolution);

    crossOffset = crossStep * HIZ_CROSS_EPSILON;
    crossStep = clamp(crossStep, 0.0, 1.0);

	// transform cell coordinates from pixelspace (defined by resolution) to screenspace (0,1)
    //old cellIndex passt
	vec2 cell = (oldCellIndex + crossStep) / mipmap_resolution;

	cell += crossOffset;
	vec2 delta = cell - O.xy;
	delta /= D.xy;
	float t = min(delta.x, delta.y);
	return intersectPlane(O, D, t);
}

vec2 getMinAndMaxDepthPlanes(vec2 coords, int level)
{
        //TODO garantieren dass coordinaten in der mitte liegt und man darf nicht interpolieren
    return texelFetch(gHiZ, ivec2(coords * (resolution-vec2(1))), level).rg;
	//return textureLod(gHiZ, coords, level).rg;
}

bool crossedCellBoundary(vec2 x, vec2 y){
//TODO works? delete comments then
    //bvec2 isEqual = equal(firstCellIdx, secondCellIdx);
   // return (x.x != y.x) || (x.y != y.y);
    return !(all(equal(x,y)));
}

void init_HiZRayTracing(){
    resolution = vec2(viewportWidth, viewportHeight);
    hiZSize = resolution;

    vec2 texelSize;
    texelSize.x = 1.0 / resolution.x;
    texelSize.y  = 1.0 / resolution.y;
    HIZ_CROSS_EPSILON = vec2( texelSize.x,  texelSize.y) * 1;
   // HIZ_CROSS_EPSILON = (vec2(0.5)/resolution);

	P_ss = vec3(TexCoords, fragment_depth); // screen space

	vec3 P_vs = UnprojectPoint(P_ss);

	// V>VS - since calculations are in view-space, we can just normalize the position to point at it
	vec3 V_vs = normalize(P_vs);

    vec3 iii = vec3( normalize(world2viewSpace(normalize(world_position - cameraPosition))));
    V_vs = (world2viewSpaceDirectionVector(normalize(reflect(normalize(world_position - cameraPosition), normalize(world_normal))))).xyz;
    calculateScreenReflection(view_position, V_vs);

    vec4 P2_cs = projectionMatrix * vec4(P_vs + V_vs, 1.0);
	vec3 P2_ss = P2_cs.xyz / P2_cs.w;
    P2_ss.xy = P2_ss.xy * 0.5 + 0.5;

    vec3 P2_vs = UnprojectPoint(P2_ss);
	V_ss = P2_ss - P_ss;


     float rDotV = clamp(dot(V_vs, normalize(P_vs)), 0.0, 1.0);
     vec3 rayyyyyyy = vec3(0);
     if(fragment_depth < 1.0 && fragment_depth > 0.0 && fragment_roughness < 1 ){ //&& rDotV >= 0.1f //&& dot(view_normal.xyz, vec3(1.0f, 1.0f, 1.0f)) != 0.0f
        rayyyyyyy = hiZTrace(screen_point.xyz, (screen_reflection.xyz));
     }

    float reflectedDepth = getMinAndMaxDepthPlanes(rayyyyyyy.xy, 0).r;
    reflections = vec4(rayyyyyyy.xy,0,1);



    if(rayyyyyyy.x < 0 || rayyyyyyy.x > 1 || rayyyyyyy.y < 0 || rayyyyyyy.x > 1 || reflectedDepth >= 1 || reflectedDepth <= 0 || overmaxIterations){
        //reflections = vec4(1);
         reflections = texture(imageInput, TexCoords);
    }else if(rayyyyyyy.z  + 0.0001> reflectedDepth && reflectedDepth != 1){// - 0.000001
         //Hitted jey :) Todo between min and max?
       reflections = texelFetch(imageInput, ivec2(rayyyyyyy.xy * resolution), 0);
       reflections = (reflections + texture(imageInput, TexCoords)) * 0.5;

     } else {
       reflections = texture(imageInput, TexCoords);
     }

}

/*
Hi-Z ray tracing function.
rayOrigin: Ray start position (in screen space).
rayDir: Ray reflection vector (in screen space).
*/
vec3 hiZTrace(vec3 p, vec3 v){
    const int rootLevel = maxMipmapLevel;//heighest mipmap-level with 0 based indexing

    int currentLevel = HIZ_START_LEVEL;
    float currentIterations = 0.0;

    // get the cell cross direction and a small offset to enter the next cell when doing cell crossing
    vec2 crossStep;
    /*crossStep.x = (v.x >= 0) ? 1.0 : -1.0;
    crossStep.y = (v.y >= 0) ? 1.0 : -1.0;*/
    crossStep.x = (v.x >= 0) ? 1.0 : -1.0;
    crossStep.y = (v.y >= 0) ? 1.0 : -1.0; //TODO ?
   // return vec3(crossStep, 0.5);

    //float2 crossStep = float2(v.x >= 0.0f ? 1.0f : -1.0f, v.y >= 0.0f ? 1.0f : -1.0f);
    vec2 crossOffset = crossStep * HIZ_CROSS_EPSILON;
   // crossStep = clamp(crossStep, 0.0, 1.0);

    // set current ray_Position to the original screen coordinate and depth
    vec3 ray = p;

    //TODO rename to something with ray_Position
    // scale vector such that z is 1.0 (maximum depth => far plane)
    /* if(v.z == 0.0){ //skybox
         bool isHit = false;
        reflections = vec4( 0);
        return vec3(0);
     }*/
    vec3 d = v.xyz / v.z; //v.xyz /= v.z // ray_direction


    if( isnan(d.x) || isinf(d.x)){
        bool isHit = false;
        return vec3(0);
    }
    // todo skybox get nan

    // set starting point to the point where z equals 0.0 (minimum depth => near plane)
    // p is at the fragment position which has a positiv depth
    // to get the origin of the ray d, p has to pe interpolated in the negative raydirection till its depth = 0
    vec3 o = intersectPlane(p, d, -p.z);



    // cross to next cell so that we don't get a self-intersection immediately
    //TODO rename to something with mimap resolution or currentHiZSize
    vec2 firstCellCount = getCellCountOfMipmapLevel(currentLevel); // passt
    vec2 rayCell = getCell(ray.xy, firstCellCount); // passt // pixelspace

    //hier richtig spater nicht mehr
    ray = intersectCellBoundary(o, d, rayCell, firstCellCount, crossStep, crossOffset);//TODO: wtf inet? *16.0);
    //return vec3(0);
    //TODO something wrong with ray?
   // reflections = vec4(ray.xy, 0,1);
   // return vec3(0);

    // in book -> while loop
    //for (int i = 0; currentLevel >= HIZ_STOP_LEVEL && i < HIZ_MAX_ITERATIONS; ++i)
    while(currentLevel >= HIZ_STOP_LEVEL && currentIterations < HIZ_MAX_ITERATIONS)
    {
        // get the minimum & maximum depth plane in which the current ray resides
    	vec2 hiZ_minmax = getMinAndMaxDepthPlanes(ray.xy, currentLevel);

        // get the cell number of the current ray
        const vec2 cellCount = getCellCountOfMipmapLevel(currentLevel);
        const vec2 oldCellIdx = getCell(ray.xy, cellCount);

        // intersect only if ray depth is [below the minimum depth plane] between minimum and maximum depth planes
        // MIN of paper: vec3 tmpRay = intersectPlane(o.xy, d.xy, max(ray.z, hiZ_minmax.r));
        // MINMAX
        //vec3 tmpRay = intersectPlane(o, d, clamp(ray.z, hiZ_minmax.r, hiZ_minmax.g)); // TODO understand and maybe switch to only min
        //vec3 tmpRay = intersectPlane(o, d, min(max(ray.z, hiZ_minmax.r), hiZ_minmax.g));
        vec3 tmpRay = intersectPlane(o, d, max(ray.z, hiZ_minmax.r));

        if(ray.z >= hiZ_minmax.r && ray.z <= hiZ_minmax.g){
           // return ray;
        }


        // get the newcell number as well
        const vec2 newCellIdx = getCell(tmpRay.xy, cellCount);

        // if the new cell number is different from the old cell number, we know a cell was crossed
        if(crossedCellBoundary(oldCellIdx, newCellIdx))
        {
            // so intersect the boundary of that cell instead, and go up a level for taking a larger step next loop
        	tmpRay = intersectCellBoundary(o, d, oldCellIdx, cellCount, crossStep, crossOffset);
        	currentLevel = min(HIZ_MAX_LEVEL, currentLevel + HIZ_ITERATION_STEP);
        }

        ray = tmpRay;
        // go down a level in Hi-Z
        --currentLevel;
        ++currentIterations;

	}

    // TODO Reject ray intersections from hidden geometry
   // reflections = vec4(1,1, 0,1);
    overmaxIterations = currentIterations >= HIZ_MAX_ITERATIONS ? true : false;
    return ray;
}


//OLD
bool isHit = false;
//ss than the depth buffer value but greater than the depth buffer value minus pixel thickness)
// TODO: ray an laenge anpassen (near to far plane)?
void init(){
    maxIterations = 30;

    stepsize = 4;//1
    maxRayDistance = 20;

    ViewAngleThreshold = 5;
    resolution = vec2(viewportWidth, viewportHeight);

    fragment_roughness = texture(gAlbedoRoughness, TexCoords).a;
    fragment_metallic =  texture(gEmissionMetallic, TexCoords).a;
    fragment_depth = texture(gLinearDepth, TexCoords).r;
    world_position = texture(gPosition, TexCoords).rgb;
    linearDepth = texture(gPosition, TexCoords).a;
    world_normal = texture(gNormal, TexCoords).rgb;

   /* step_bias = (zzzz.z-maxDistance)/(maxmaxDistance-maxDistance);
    if(step_bias < 0){
        step_bias = 0;
    }
     stepsize = step_bias *(1.0 - 0.1) + 0.1;
    }*/

    view_position = world2viewSpace(world_position);
    view_normal =  world2viewSpaceDirectionVector(normalize(texture(gNormal, TexCoords).rgb));
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



vec3 ProjectPoint(vec3 viewPoint)
{
	vec4 projPoint = projectionMatrix * vec4(viewPoint, 1.0);
	projPoint.xyz /= projPoint.w;
	projPoint.xy = projPoint.xy * vec2(0.5, -0.5) + vec2(0.5);
	return projPoint.xyz;
}


void main(){
    init();
    init_HiZRayTracing();
    vec4 result = vec4(0,0,0,0);

    // TODO: angle wider then 90 degree -> set reflection fully transparent
    // TODO: ViewAngleThreshold-> only count reflection if they are over the treshhold (angle between the normal and the camera direction)

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
    view_endPoint = (view_refl * 1) + view_pos;//normalize
    screen_point = view2screenSpace(view_pos);
    screen_endPoint = view2screenSpace(view_endPoint);
    // tip minus base (spitze minus schaft)
    screen_reflection = (screen_endPoint - screen_point);
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
  //  result.xyz = result.xyz * 0.5 + 0.5;
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
