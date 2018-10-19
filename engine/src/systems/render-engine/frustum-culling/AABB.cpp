#include <systems/render-engine/frustum-culling/AABB.hpp>

using namespace NAISE::RenderCore;

AABB::AABB(const std::vector<glm::vec3>& points) {
	float minX = std::numeric_limits<float>::infinity();
	float maxX = -std::numeric_limits<float>::infinity();
	float minY = std::numeric_limits<float>::infinity();
	float maxY = -std::numeric_limits<float>::infinity();
	float minZ = std::numeric_limits<float>::infinity();
	float maxZ = -std::numeric_limits<float>::infinity();

	for (auto point : points) {
		minX = glm::min(minX, point.x);
		maxX = glm::max(maxX, point.x);
		minY = glm::min(minY, point.y);
		maxY = glm::max(maxY, point.y);
		minZ = glm::min(minZ, point.z);
		maxZ = std::max(maxZ, point.z);
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

bool AABB::intersect(const AABB& aabb1, const AABB& aabb2) {
	return (aabb1.values.first.x <= aabb2.values.second.x && aabb1.values.second.x >= aabb2.values.first.x) &&
			(aabb1.values.first.y <= aabb2.values.second.y && aabb1.values.second.y >= aabb2.values.first.y) &&
			(aabb1.values.first.z <= aabb2.values.second.z && aabb1.values.second.z >= aabb2.values.first.z);
}

// TODO: not copy code
AABB::AABB(const std::vector<glm::vec4>& points) {
	float minX = std::numeric_limits<float>::infinity();
	float maxX = -std::numeric_limits<float>::infinity();
	float minY = std::numeric_limits<float>::infinity();
	float maxY = -std::numeric_limits<float>::infinity();
	float minZ = std::numeric_limits<float>::infinity();
	float maxZ = -std::numeric_limits<float>::infinity();

	for (auto point : points) {
		minX = glm::min(minX, point.x);
		maxX = glm::max(maxX, point.x);
		minY = glm::min(minY, point.y);
		maxY = glm::max(maxY, point.y);
		minZ = glm::min(minZ, point.z);
		maxZ = std::max(maxZ, point.z);
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

void AABB::transform(mat4 transform) {
	float minX = std::numeric_limits<float>::infinity();
	float maxX = -std::numeric_limits<float>::infinity();
	float minY = std::numeric_limits<float>::infinity();
	float maxY = -std::numeric_limits<float>::infinity();
	float minZ = std::numeric_limits<float>::infinity();
	float maxZ = -std::numeric_limits<float>::infinity();

	for (auto i : obb) {
		vec3 tmp = vec3(transform * vec4(i, 1));
		minX = glm::min(minX, tmp.x);
		maxX = glm::max(maxX, tmp.x);
		minY = glm::min(minY, tmp.y);
		maxY = glm::max(maxY, tmp.y);
		minZ = glm::min(minZ, tmp.z);
		maxZ = std::max(maxZ, tmp.z);
	}

	values = pair<vec3, vec3>(glm::vec3(minX, minY, minZ), glm::vec3(maxX, maxY, maxZ));
}

bool AABB::intersect(const AABB& other) {
	return AABB::intersect(*this, other);
}
