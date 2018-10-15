#pragma once

#include <vector>

#include <glm/glm.hpp>

using namespace glm;
using namespace std;

namespace NAISE {
namespace RenderCore {

class AABB {
public:
	AABB() = default;
	explicit AABB(const std::vector<glm::vec3>& points);
	explicit AABB(const std::vector<glm::vec4>& points);

	void transform(mat4 transform);

	static bool intersect(const AABB& aabb1, const AABB& aabb2);

	/**
	 * first: min values
	 * second: max values
	 */
	std::pair<glm::vec3, glm::vec3> values;

protected:
	std::vector<vec3> obb;
};

}
}
