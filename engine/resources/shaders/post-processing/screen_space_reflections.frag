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
#define HIZ_START_LEVEL 0
#define HIZ_STOP_LEVEL 0 //0.0 //shouldn't be too height because of artefacts
#define HIZ_MAX_LEVEL maxMipmapLevel
vec2 HIZ_CROSS_EPSILON; //TODO divide by the resolution?
#define HIZ_MAX_ITERATIONS 200
#define HIZ_ITERATION_STEP 2 //was fix at 2.0 in paper

// HI-Z Fading
#define _ScreenEdgeFadeStart 0.1

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
float screenspacereflection = 0.0;

/** FUNCTIONS **/

// Hi-Z
vec3 hiZTrace(vec3 p, vec3 v);
vec2 lookUpHiZ(vec2 coords, int level, vec2 mipmapresolution);
vec2 getHiZMipmapResolution(int level); // currently just a wrapper
vec2 getCell(vec2 ray, vec2 mipmap_resolution);
vec3 intersectPlane(vec3 O, vec3 D, float t);
bool crossedCellBoundary(vec2 x, vec2 y);
vec3 intersectCellBoundary(vec3 O, vec3 D, vec2 oldCellIndex, vec2 mipmap_resolution, vec2 crossStep, vec2 crossOffset);
vec4 runHiZTracing();

// Ray-Casting
vec4 BinarySearch(vec4 rayDir, vec4 hitCoord);
vec4 RayMarching();

// Space transformations
vec3 world2viewSpace(vec3 position);
vec3 world2viewSpaceDirectionVector(vec3 dir);
vec4 view2screenSpace(vec3 position);
vec4 view2clipSpace(vec3 position);
vec4 NDC2screenSpace(vec4 position);
vec3 screenBack2View(vec3 screenPoint);
vec4 screenSpace2view(vec4 position);

// Helper functions
void calculateScreenReflection(vec3 view_pos, vec3 view_refl);
vec4 blending(vec4 fetchedColor, vec4 coords);


/*
This function computes the new ray position where
the ray intersects with the specified cell's boundary.
*/

/*  Line Rasterization
    Calculates the neighbor cell within a Hi-Z mipmap and computes the associated ray position in screen space
    by interpolationg between O and O + D
*/

vec3 intersectCellBoundary(vec3 O, vec3 D, vec2 oldCellIndex, vec2 mipmap_resolution, vec2 crossStep, vec2 crossOffset)
{

    //TODO
   /* "delta" is a vector pointing from the origin of the ray to "index" which is a point along the ray between "o" and "d" (where z = 0 at o and z = 1 at d).
    Dividing delta by d.xy gives you a percentage of how far along the ray each component is.  You take the minimum of the two to decide how far to move between
    "o" and "d".*/

     vec2 texelSize;
   // texelSize.x = 1.0 / mipmap_resolution.x;
    //texelSize.y  = 1.0 / mipmap_resolution.y;
    //  HIZ_CROSS_EPSILON = vec2( texelSize.x,  texelSize.y) * 1;
    texelSize.x = 1.0 / resolution.x;
    texelSize.y  = 1.0 / resolution.y;

    HIZ_CROSS_EPSILON = vec2( texelSize.x,  texelSize.y) * exp2(HIZ_START_LEVEL + 1);
   // HIZ_CROSS_EPSILON = (vec2(1)/mipmap_resolution);

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

void init_HiZRayTracing(){
    resolution = vec2(viewportWidth, viewportHeight);

    vec2 texelSize;
    texelSize.x = 1.0 / resolution.x;
    texelSize.y  = 1.0 / resolution.y;
    HIZ_CROSS_EPSILON = vec2( texelSize.x,  texelSize.y) * 1;
   // HIZ_CROSS_EPSILON = (vec2(0.5)/resolution);

	P_ss = vec3(TexCoords, fragment_depth); // screen space

	vec3 P_vs = screenBack2View(P_ss);

	// V>VS - since calculations are in view-space, we can just normalize the position to point at it
	vec3 V_vs = normalize(P_vs);

    vec3 iii = vec3( normalize(world2viewSpace(normalize(world_position - cameraPosition))));
    view_reflection = (world2viewSpaceDirectionVector(normalize(reflect(normalize(world_position - cameraPosition), normalize(world_normal))))).xyz;
    calculateScreenReflection(view_position, view_reflection);

    vec4 P2_cs = projectionMatrix * vec4(P_vs + V_vs, 1.0);
	vec3 P2_ss = P2_cs.xyz / P2_cs.w;
    P2_ss.xy = P2_ss.xy * 0.5 + 0.5;

    vec3 P2_vs = screenBack2View(P2_ss);
	V_ss = P2_ss - P_ss;
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
    screenspacereflection = texture(gLinearDepth, TexCoords).g;

    view_position = world2viewSpace(world_position);
    view_normal =  world2viewSpaceDirectionVector(normalize(texture(gNormal, TexCoords).rgb));
    view_reflection = normalize(reflect(normalize(view_position.xyz), normalize(view_normal)));
    //same as:
  //   view_reflection = normalize(reflect(normalize(world_position-cameraPosition), normalize(world_normal)));
   //  view_reflection = normalize(world2viewSpaceDirectionVector(view_reflection));

    //angle = degrees(acos(dot(normalize(view_reflection), normalize(view_normal))));

    calculateScreenReflection(view_position, view_reflection);
}

void main(){
    init();
    init_HiZRayTracing();
    reflections = runHiZTracing();

    //reflections = RayMarching();

    // TODO: angle wider then 90 degree -> set reflection fully transparent
    // TODO: ViewAngleThreshold-> only count reflection if they are over the treshhold (angle between the normal and the camera direction)

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

/** HI-Z Functions **/

vec2 lookUpHiZ(vec2 coords, int level, vec2 mipmapresolution)
{
    // return textureLod(gHiZ, coords, level).rg;
    // return texelFetch(gHiZ, ivec2(coords * (resolution-vec2(1))), level).rg;
    // return texelFetch(gHiZ, ivec2(coords * (resolution-vec2(1))), 0).rg;
    return texelFetch(gHiZ, ivec2(coords * (mipmapresolution-vec2(1))), level).rg;
}

vec2 getHiZMipmapResolution(int level){
    return vec2(textureSize(gHiZ,level));
}

/* pixel coordinates of intersected cell in the mipmap */
vec2 getCell(vec2 ray, vec2 mipmap_resolution){
    // example: ray = (0.5, 0.25); mipmap_resolution = (1000, 500) -> cell = (500, 125)
    return floor(ray * mipmap_resolution); // round .xy to int
}

/**
Intersects the closest plane, returns = O + D * t only
Interpolation between O and O + D where O is the origin of the ray at the near plane (Pss + D * -Pss.z),
D the reflectiondirecton and t the interpolation factor between 0 and 1
**/
vec3 intersectPlane(vec3 O, vec3 D, float t)
{
	return O + D * t;
}

bool crossedCellBoundary(vec2 x, vec2 y){
    // returns true if x and y are different and false if they are exactly the same
    return !(all(equal(x,y)));
}

vec4 runHiZTracing(){
    vec4 result = vec4(0); // result.a -> alpha (if reflection alpha > = else alpha == 0)
    vec3 ray_pos = vec3(0);

    // TODO add if reflection or not
    if(fragment_depth < 1.0 && fragment_depth > 0.0 && fragment_roughness < 1 && screenspacereflection > 0){ //&& rDotV >= 0.1f //&& dot(view_normal.xyz, vec3(1.0f, 1.0f, 1.0f)) != 0.0f
        ray_pos = hiZTrace(screen_point.xyz, (screen_reflection.xyz));

        float reflectedDepth = lookUpHiZ(ray_pos.xy, 0, resolution).r;

        if(ray_pos.x < 0 || ray_pos.x > 1 || ray_pos.y < 0 || ray_pos.x > 1 || ray_pos.z <= 0 || ray_pos.z >= 1 || reflectedDepth >= 1 || reflectedDepth <= 0|| overmaxIterations){// || overmaxIterations
            // swallow
        }else if(ray_pos.z  + 0.0001> reflectedDepth && reflectedDepth != 1){// - 0.000001
            //Hitted
            result = texelFetch(imageInput, ivec2(ray_pos.xy * resolution), 0);//.xyz;

            // Fading inspired by: http://www.kode80.com/blog/2015/03/11/screen-space-reflections-in-unity-5/index.html
            float alpha = 1.0;
            vec2 ray_posNDC = (ray_pos.xy * 2.0 - 1.0);
            float maxDimension =max( abs(ray_posNDC.x), abs(ray_posNDC.y));
            alpha *= min(0.7, (1.0 - (max( 0, maxDimension - _ScreenEdgeFadeStart) / (1.0 - _ScreenEdgeFadeStart))));

            // Fade ray hits based on distance from ray origin
            alpha *= 1.0 - clamp( distance( screen_point.xy, ray_pos.xy) , 0.0, 1.0); //0.6 /// 0.8

            // Fade if normals of surface show to the camera
            float rDotV = clamp(dot(view_reflection, normalize(view_position)), 0.0, 1.0);
            if(rDotV < 0.1f){
                alpha *= clamp(rDotV,0.0,0.1);
            }

            result.a = alpha;
        }
    }
    return result;
}

/*
p: Ray start position (in screen space)
v: Ray reflection vector (in screen space)
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
    vec2 firstCellCount = getHiZMipmapResolution(currentLevel); // passt
    vec2 rayCell = getCell(ray.xy, firstCellCount); // passt // pixelspace

    //hier richtig spater nicht mehr
    ray = intersectCellBoundary(o, d, rayCell, firstCellCount, crossStep, crossOffset);//TODO: wtf inet? *16.0);
    //return vec3(0);
    //TODO something wrong with ray?
   // reflections = vec4(ray.xy, 0,1);
   // return vec3(0);
    vec2 hiZ_minmax;
    vec2 cellCount;
    // in book -> while loop
    //for (int i = 0; currentLevel >= HIZ_STOP_LEVEL && i < HIZ_MAX_ITERATIONS; ++i)
    while(currentLevel >= HIZ_STOP_LEVEL && currentIterations < HIZ_MAX_ITERATIONS)
    {


        // get the cell number of the current ray
        cellCount = getHiZMipmapResolution(currentLevel);
        const vec2 oldCellIdx = getCell(ray.xy, cellCount);

        // get the minimum & maximum depth plane in which the current ray resides
    	hiZ_minmax = lookUpHiZ(ray.xy, currentLevel, cellCount);

        // intersect only if ray depth is [below the minimum depth plane] between minimum and maximum depth planes
        // MIN of paper: vec3 tmpRay = intersectPlane(o.xy, d.xy, max(ray.z, hiZ_minmax.r));
        // MINMAX
        //vec3 tmpRay = intersectPlane(o, d, clamp(ray.z, hiZ_minmax.r, hiZ_minmax.g)); // TODO understand and maybe switch to only min
        vec3 tmpRay = intersectPlane(o, d, min(max(ray.z, hiZ_minmax.r), hiZ_minmax.g+ (0.0001 - 0.00001 * currentLevel)));
        // tmpRay = intersectPlane(o, d, max(ray.z, hiZ_minmax.r));


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
   // reflections = vec4(1,1, 0,1);
    overmaxIterations = currentIterations >= HIZ_MAX_ITERATIONS ? true : false;
    hiZ_minmax = lookUpHiZ(ray.xy, 0,  getHiZMipmapResolution(0));
   vec2 hiZ_minmax2 = lookUpHiZ(ray.xy, 5,  getHiZMipmapResolution(5));
    if(ray.z < hiZ_minmax.r || ray.z > hiZ_minmax2.g){
      //  return vec3(0);
    }
    return ray.xyz;
}



/** RayCasting Functions **/

// TODO: should the linear or non linear depth be used? -> linear depth is better
// TODO: stepsize must get greater while stepping through the ray?
vec4 RayMarching(){
    vec4 screen_rayPosition = screen_point;
    float rayDepth = 0; // accumulated depth value on rayPosition
    float texDepth = 0; // corresponding depth at the view_positionOnRay in the depth texture
    screen_rayPosition += screen_reflection * stepsize; // TODO jitter

    for(int i = 0; i < maxIterations; i++){

        ivec2 denormalizedTexCoords = ivec2(screen_rayPosition.xy * resolution);
        //depth in rayDepth & texDepth & fragment_depth must be exactly the same if view_positionOnRay = view2screenSpace(view_position);
        texDepth = texelFetch(gPosition, denormalizedTexCoords, 0).a;

        if(texDepth == 1.0 || texDepth <= 0.0 ){
            // if 1.0: hit on skybox; TODO is early termination ok? probably not when the scene has flying/ hovering elements
            // Stop ray marching when the ray goes outside screen space
            return vec4(0);
        }

        vec4 world = screenSpace2view(screen_rayPosition);
        rayDepth = world.z; // rayDepth = (screen_rayPosition.z);

        float pixelthickness = 0.001;
        if(rayDepth >= texDepth + 0.0001 ){// + 0.0000001 // && rayDepth < texDepth + 0.2
            isHit = true;
            return BinarySearch(screen_reflection, screen_rayPosition); // return vec4(screen_rayPosition.xy, rayDepth, 1.0);
        }
        //stepsize = (stepsize/screen_rayPosition.z);
        screen_rayPosition += screen_reflection * stepsize;
        if(screen_rayPosition.z > screen_endPoint.z){
            return vec4(0); //ray missed
        }
    }
    return vec4(0); //ray missed
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

// Space Transformations

vec3 world2viewSpace(vec3 position){
    return (viewMatrix * vec4(position, 1)).xyz;
}

vec3 world2viewSpaceDirectionVector(vec3 dir){
    return (transpose(inverse(viewMatrix)) * vec4(dir, 1)).xyz;
}

vec4 view2screenSpace(vec3 position){
    vec4 result = projectionMatrix * vec4(position, 1.0);
    result.xyz /= result.w;
    result.xyz = result.xyz * 0.5 + 0.5; // NDC -> (0,1) so it can be used as TexCoords
    result.w = 1.0;
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
    return result;
}
// TODO which is correct? screenSpace2view or screenBack2View
vec3 screenBack2View(vec3 screenPoint)
{
	screenPoint.xy = (screenPoint.xy - vec2(0.5)) * vec2(2.0, 2.0);
	vec4 projPos = inverse(projectionMatrix) * vec4(screenPoint, 1.0);
    return projPos.xyz/projPos.w;
}

// Other herlper functions
