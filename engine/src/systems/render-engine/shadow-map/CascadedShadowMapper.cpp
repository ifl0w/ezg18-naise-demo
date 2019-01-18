#include <systems/render-engine/shadow-map/CascadedShadowMapper.hpp>

#include "Logger.hpp"

using namespace NAISE::RenderCore;

CascadedShadowMapper::CascadedShadowMapper() {
	cascades = {
			Cascade(vec2(1024 * 2,1024 * 2), vec2(0, 20)),
			Cascade(vec2(1024 * 2,1024 * 2), vec2(20, 40)),
			Cascade(vec2(1024 * 2,1024 * 2), vec2(40, 80)),
			Cascade(vec2(1024 * 2,1024 * 2), vec2(80, 200))
	};

	for (int i = 0; i < cascades.size(); ++i) {
		_shadowMaps.push_back(std::make_unique<ShadowMap>(cascades[i].size.x, cascades[i].size.y));
		cascades[i].shadowMap = _shadowMaps[i].get();
	}

	_cascadeInstances = vector<ShadowCasterInstances>(cascades.size());
}

CascadedShadowMapper::CascadedShadowMapper(std::vector<Cascade>& cascades)
		: cascades(cascades) {

	for (auto& cascade: cascades) {
		_shadowMaps.push_back(std::make_unique<ShadowMap>(cascade.size.x, cascade.size.y));
	}

	_cascadeInstances = vector<ShadowCasterInstances>(cascades.size());
}

void CascadedShadowMapper::activate() {
	ShadowMapper::activate();
}

void CascadedShadowMapper::evaluate() {
	ShadowMapper::evaluate();
}

void CascadedShadowMapper::deactivate() {
	ShadowMapper::deactivate();
}

void CascadedShadowMapper::addShadowCaster(Mesh* mesh, mat4 transform, AABB aabb) {
	for (int i = 0; i < _shadowCascadeFrustums.size(); ++i) {
		if (_shadowCascadeFrustums[i].intersect(aabb)) {
			_cascadeInstances[i][mesh].push_back(transform);
		}
	}
}

void CascadedShadowMapper::update(Frustum& cameraFrustum, NAISE::RenderCore::Light* light) {
	_shadowCascadeAABBs.clear();
	_shadowCascadeFrustums.clear();

	_shadowViewMatrix = light->getShadowMatrix();

	for (int i = 0; i < cascades.size(); ++i) {
		_cascadeInstances[i].clear();

		vector<vec4> points = cameraFrustum.getBoundingVolume(cascades[i].range.x, cascades[i].range.y);

		_shadowCascadeAABBs.emplace_back(points);
		_shadowCascadeFrustums.emplace_back(points, _shadowViewMatrix, 500);

		float minX = std::numeric_limits<float>::infinity();
		float maxX = -std::numeric_limits<float>::infinity();
		float minY = std::numeric_limits<float>::infinity();
		float maxY = -std::numeric_limits<float>::infinity();
		float minZ = std::numeric_limits<float>::infinity();
		float maxZ = -std::numeric_limits<float>::infinity();

		for (auto p : points) {
			vec3 tmp = _shadowViewMatrix * p;
			minX = glm::min(minX, tmp.x);
			maxX = glm::max(maxX, tmp.x);
			minY = glm::min(minY, tmp.y);
			maxY = glm::max(maxY, tmp.y);
			minZ = glm::min(minZ, tmp.z);
			maxZ = glm::max(maxZ, tmp.z);
		}

		cascades[i].viewMatrix = _shadowViewMatrix;
		cascades[i].projectionMatrix = glm::ortho(minX, maxX, minY, maxY, -maxZ - 500, -minZ);
	}
}

RenderCommandBuffer CascadedShadowMapper::generateCommandBuffer() {
	RenderCommandBuffer buffer;
	buffer.reserve(_commandBufferSize);

	buffer.push_back(SetRenderProperty { DEPTH_TEST, true });

	buffer.push_back(SetRenderProperty { BACKFACE_CULLING, false });

	for (int i = 0; i < cascades.size(); i++) {
		buffer.push_back(SetRenderTarget {
				cascades[i].shadowMap
		});

		buffer.push_back(ClearRenderTarget { true, true, false });

		buffer.push_back(SetShader { &_shadowShader });

		buffer.push_back(SetViewProjectionData {
				cascades[i].viewMatrix,
				cascades[i].projectionMatrix,
				vec3(0),
		});

		buffer.push_back(SetViewport { vec2(0), vec2(cascades[i].shadowMap->width, cascades[i].shadowMap->height) });

		// append draw commands
		for (auto& shadowCasterInstance: _cascadeInstances[i]) {
			if (shadowCasterInstance.second.size() > 1) {
				buffer.push_back(DrawInstancedDirect { shadowCasterInstance.first, shadowCasterInstance.second });
			} else {
				for (auto transform: shadowCasterInstance.second) {
					buffer.push_back(DrawMeshDirect { shadowCasterInstance.first, transform });
				}
			}
		}
	}

	buffer.push_back(SetRenderProperty { BACKFACE_CULLING, true });
	buffer.push_back(ResetViewport {});

	_commandBufferSize = glm::max(_commandBufferSize, buffer.size());
	return buffer;
}

RenderCommandBuffer CascadedShadowMapper::generateDebugCommandBuffer() {
	RenderCommandBuffer commandBuffer;

	for (int i = 0; i < cascades.size(); ++i) {
		commandBuffer.push_back(DrawWireframeDirect {
				_shadowCascadeFrustums[i].frustumMesh.get(),
				mat4(1),
				vec3(1,0,1)
		});
	}

	return commandBuffer;
}
