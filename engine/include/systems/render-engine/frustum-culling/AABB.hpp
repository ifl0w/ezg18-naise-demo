#pragma once

#include <vector>

#include <glm/glm.hpp>

using namespace glm;
using namespace std;

namespace NAISE {
namespace RenderCore {

class AABB {
public:
	AABB() {
		// insert inverse inifinity values to ensure that merge() is working
		values.first = vec3(std::numeric_limits<float>::infinity());
		values.second = vec3(-std::numeric_limits<float>::infinity());
	};
	explicit AABB(const std::vector<glm::vec3>& points);
	explicit AABB(const std::vector<glm::vec4>& points);

	void transform(mat4 transform);

	bool intersect(const AABB& other);
	static bool intersect(const AABB& aabb1, const AABB& aabb2);

	void merge(const std::vector<glm::vec3>& points) {
		float minX = std::numeric_limits<float>::infinity();
		float maxX = -std::numeric_limits<float>::infinity();
		float minY = std::numeric_limits<float>::infinity();
		float maxY = -std::numeric_limits<float>::infinity();
		float minZ = std::numeric_limits<float>::infinity();
		float maxZ = -std::numeric_limits<float>::infinity();

		for(auto point: obb) {
			minX = glm::min(minX, point.x);
			maxX = glm::max(maxX, point.x);
			minY = glm::min(minY, point.y);
			maxY = glm::max(maxY, point.y);
			minZ = glm::min(minZ, point.z);
			maxZ = glm::max(maxZ, point.z);
		}

		for (auto point : points) {
			minX = glm::min(minX, point.x);
			maxX = glm::max(maxX, point.x);
			minY = glm::min(minY, point.y);
			maxY = glm::max(maxY, point.y);
			minZ = glm::min(minZ, point.z);
			maxZ = glm::max(maxZ, point.z);
		}

		values = pair<vec3, vec3>(glm::vec3(minX, minY, minZ), glm::vec3(maxX, maxY, maxZ));
		obb = vector<vec3>({
								   vec3(minX, minY, minZ),
								   vec3(minX, maxY, minZ),
								   vec3(maxX, minY, minZ),
								   vec3(maxX, maxY, minZ),

								   vec3(minX, minY, maxZ),
								   vec3(minX, maxY, maxZ),
								   vec3(maxX, minY, maxZ),
								   vec3(maxX, maxY, maxZ),
						   });
	}

	/**
	 * first: min values
	 * second: max values
	 */
	std::pair<glm::vec3, glm::vec3> values;

	std::vector<vec3> obb;
protected:
};

}
}
