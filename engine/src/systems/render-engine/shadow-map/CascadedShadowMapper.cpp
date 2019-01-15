#include <systems/render-engine/shadow-map/CascadedShadowMapper.hpp>

#include "Logger.hpp"

using namespace NAISE::RenderCore;

CascadedShadowMapper::CascadedShadowMapper() {
	cascades = {
			Cascade(vec2(1024 * 4,1024 * 4), vec2(0, 50)),
			Cascade(vec2(1024 * 2,1024 * 2), vec2(50, 150)),
			Cascade(vec2(1024,1024), vec2(150, 300))
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

		_shadowCascadeAABBs.emplace_back(cameraFrustum.getBoundingVolume(cascades[i].range.x, cascades[i].range.y));
		_shadowCascadeFrustums.emplace_back(_shadowCascadeAABBs[i], _shadowViewMatrix, 500);

		cascades[i].viewMatrix = _shadowViewMatrix;
		cascades[i].projectionMatrix = light->getProjectionMatrix(_shadowCascadeAABBs[i]);
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
