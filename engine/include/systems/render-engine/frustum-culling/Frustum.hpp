#pragma once

#include "AABB.hpp"

#include <glm/glm.hpp>
#include <vector>

using namespace std;
using namespace glm;

namespace NAISE {
namespace Engine {

class Frustum {
public:
	Frustum() { planes = vector<pair<vec3, vec3>>(6); };
	Frustum(double fovX, double fovY, double nearDistance, double farDistance);

	/**
	 * This Constructor creates a orthographic "frustum" from an AABB and a
	 * camera/light matrix that specifies the orientation.
	 * Calling recalculate() on such an object is considered undefined behaviour.
	 *
	 * @param aabb
	 * @param invViewMatrix
	 * @param extendZ Stretches the z axis of the frustrum. (eg. Shadow distance)
	 */
	Frustum(AABB aabb, mat4 invViewMatrix, float extendZ);

	enum Planes {
	  NEAR_PLANE = 0,
	  FAR_PLANE,
	  LEFT_PLANE,
	  RIGHT_PLANE,
	  TOP_PLANE,
	  BOTTOM_PLANE
	};

	enum Points {
	  NTL = 0,
	  NTR,
	  NBL,
	  NBR,
	  FTL,
	  FTR,
	  FBL,
	  FBR
	};

	void setCameraSettings(double fovX, double fovY, double nearDistance, double farDistance);

	void recalculate(mat4 invViewMatrix);

	vector<vec4> getBoundingVolume(double maxDistance = INFINITY) const;

	/**
	 * Check whether an AABB intersects with the frustum.
	 * @param box
	 * @return
	 */
	bool intersect(AABB& box);

	/**
	 * Check weather an sphere intersects with the frustum.
	 * @param center
	 * @param radius
	 * @return
	 */
	bool intersect(vec3 center, float radius);

protected:
	double fovY;
	double fovX;
	double farDistance;
	double nearDistance;
	mat4 invViewMat;

	/**
	 * 	first: normal
	 * 	second: point
	 */
	vector<pair<vec3, vec3>> planes;

	double nh;
	double nw;
	double fw;
	double fh;
};

}
}

