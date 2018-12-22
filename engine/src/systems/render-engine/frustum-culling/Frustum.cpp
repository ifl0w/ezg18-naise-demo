#include <systems/render-engine/frustum-culling/Frustum.hpp>

using namespace NAISE::RenderCore;

Frustum::Frustum(double fovX, double fovY, double nearDistance, double farDistance)
		: fovX(fovX),
		  fovY(fovY),
		  nearDistance(nearDistance),
		  farDistance(farDistance) {

	invViewMat = mat4(1);
	planes = vector<pair<vec3, vec3>>(6);
	frustumMesh = std::make_unique<Mesh>();

	setCameraSettings(fovX, fovY, nearDistance, farDistance);
}

void Frustum::setCameraSettings(double fovX, double fovY, double nearDistance, double farDistance) {
	this->fovX = fovX;
	this->fovY = fovY;
	this->nearDistance = nearDistance;
	this->farDistance = farDistance;

	// compute width and height of the near and far plane sections
	nw = nearDistance * tan(fovX / 2);
	nh = nearDistance * tan(fovY / 2);
	fw = farDistance * tan(fovX / 2);
	fh = farDistance * tan(fovY / 2);
}

vector<vec4> Frustum::getBoundingVolume(double maxDistance) {
	double localFar = farDistance;
	if (maxDistance < farDistance) {
		localFar = maxDistance;
	}

	vector<vec4> result = vector<vec4>(0);

	double tanHalfHorizonalFOV = glm::tan(fovX / 2.0f);
	double tanHalfVerticalFOV = glm::tan(fovY / 2.0f);

	double xn = nearDistance * tanHalfHorizonalFOV;
	double xf = localFar * tanHalfHorizonalFOV;
	double yn = nearDistance * tanHalfVerticalFOV;
	double yf = localFar * tanHalfVerticalFOV;

	// insert in world space
	result.insert(result.begin(), {
			// near face (ntl, ntr, nbl, nbr)
			invViewMat * vec4(-xn, yn, -nearDistance, 1.0),
			invViewMat * vec4(xn, yn, -nearDistance, 1.0),
			invViewMat * vec4(-xn, -yn, -nearDistance, 1.0),
			invViewMat * vec4(xn, -yn, -nearDistance, 1.0),

			// far face (ftl, ftr, fbl, fbr)
			invViewMat * vec4(-xf, yf, -localFar, 1.0),
			invViewMat * vec4(xf, yf, -localFar, 1.0),
			invViewMat * vec4(-xf, -yf, -localFar, 1.0),
			invViewMat * vec4(xf, -yf, -localFar, 1.0)
	});

	std::vector<vec3> points(result.begin(), result.end());
	frustumMesh = std::make_unique<Mesh>(points);

	return result;
}

void Frustum::recalculate(mat4 invViewMatrix) {
	// ingore scaling of matrix. The scale should not influence the frustum size.
	invViewMat = mat4(normalize(invViewMatrix[0]), normalize(invViewMatrix[1]), normalize(invViewMatrix[2]), invViewMatrix[3]);

	dvec3 X = normalize(vec3(invViewMat * vec4(1, 0, 0, 0)));
	dvec3 Y = normalize(vec3(invViewMat * vec4(0, 1, 0, 0)));
	dvec3 Z = normalize(vec3(invViewMat * vec4(0, 0, 1, 0)));

	dvec3 camPos = vec3(invViewMat * vec4(0, 0, 0, 1));
	dvec3 nc = dvec3(camPos) - Z * nearDistance;
	dvec3 fc = dvec3(camPos) - Z * farDistance;

	planes[Planes::NEAR_PLANE] = pair(-Z, nc);
	planes[Planes::FAR_PLANE] = pair(Z, fc);

	dvec3 aux, normal;

	aux = (nc + Y * nh) - camPos;
	aux = normalize(aux);
	normal = cross(aux, X);
	planes[Planes::TOP_PLANE] = pair(normal, nc + Y * nh);

	aux = (nc - Y * nh) - camPos;
	aux = normalize(aux);
	normal = cross(X, aux);
	planes[Planes::BOTTOM_PLANE] = pair(normal, nc - Y * nh);

	aux = (nc - X * nw) - camPos;
	aux = normalize(aux);
	normal = cross(aux, Y);
	planes[Planes::LEFT_PLANE] = pair(normal, nc - X * nw);

	aux = (nc + X * nw) - camPos;
	aux = normalize(aux);
	normal = cross(Y, aux);
	planes[Planes::RIGHT_PLANE] = pair(normal, nc + X * nw);
}

bool Frustum::intersect(AABB& box) {
	//for each plane do ...
	for (int i = 0; i < 6; i++) {
		vec3 normal = planes[i].first;

		vec3 p = box.values.first;
		if (normal.x > 0)
			p.x = box.values.second.x;
		if (normal.y > 0)
			p.y = box.values.second.y;
		if (normal.z > 0)
			p.z = box.values.second.z;

		vec3 n = box.values.second;
		if (normal.x < 0)
			p.x = box.values.first.x;
		if (normal.y < 0)
			p.y = box.values.first.y;
		if (normal.z < 0)
			p.z = box.values.first.z;

		vec3 planePoint = planes[i].second;
		vec3 planeToMin = p - planePoint;
		vec3 planeToMax = n - planePoint;

		auto d1 = dot(planeToMin, normal);
		auto d2 = dot(planeToMax, normal);

		if (d1 < 0.0f && d2 < 0.0f) {
			return false;
		}
	}

	return true;
}

bool Frustum::intersect(vec3 center, float radius) {

	for(int i=0; i < 6; i++) {
		vec3 planePoint = planes[i].second;
		vec3 normal = planes[i].first;

		vec3 planeToCenter = center - planePoint;
		vec3 planeToExtrema = (center + normal * radius) - planePoint;

		auto d1 = dot(planeToCenter, normal);
		auto d2 = dot(planeToExtrema, normal);

		if (d1 < 0.0f && d2 < 0.0f)
			return false;
	}

	return true;
}

Frustum::Frustum(AABB aabb, mat4 invViewMatrix, float extend) {
	vec3 X = normalize(vec3(invViewMatrix * vec4(1, 0, 0, 0)));
	vec3 Y = normalize(vec3(invViewMatrix * vec4(0, 1, 0, 0)));
	vec3 Z = normalize(vec3(invViewMatrix * vec4(0, 0, 1, 0)));

	aabb.transform(invViewMatrix);
	float minX = aabb.values.first.x;
	float minY = aabb.values.first.y;
	float minZ = aabb.values.first.z;
	float maxX = aabb.values.second.x;
	float maxY = aabb.values.second.y;
	float maxZ = aabb.values.second.z;

	planes = vector<pair<vec3, vec3>>(6);

	planes[Planes::NEAR_PLANE] = pair(-Z, Z * (maxZ + extend));
	planes[Planes::FAR_PLANE] = pair(Z, Z * minZ);

	planes[Planes::TOP_PLANE] = pair(-Y, Y * maxY);
	planes[Planes::BOTTOM_PLANE] = pair(Y, Y * minY);

	planes[Planes::LEFT_PLANE] = pair(X, X * maxX);
	planes[Planes::RIGHT_PLANE] = pair(-X, X * minX);

//	std::vector<vec3> points({Z * (maxZ + extend), Z * minZ,  Y * maxY, Y * minY ,  X * maxX, X * minX});
	std::vector<vec3> points({
		vec3(minX, minY, minZ), vec3(maxX, minY, minZ), vec3(maxX, maxY, minZ), vec3(minX, maxY, minZ),
		vec3(minX, minY, maxZ), vec3(maxX, minY, maxZ), vec3(maxX, maxY, maxZ), vec3(minX, maxY, maxZ)
	});
	frustumMesh = std::make_unique<Mesh>(points);
}
