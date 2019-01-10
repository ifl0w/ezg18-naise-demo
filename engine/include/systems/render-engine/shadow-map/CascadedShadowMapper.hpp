#pragma once

#include <glm/vec2.hpp>

#include <systems/render-engine/shadow-map/ShadowMapper.hpp>
#include <systems/render-engine/shadow-map/ShadowMap.hpp>
#include <systems/render-engine/frustum-culling/Frustum.hpp>

#include "../lights/Light.hpp"
#include "ShadowShader.hpp"

namespace NAISE {
namespace RenderCore {

struct Cascade {
  /**
   * width x height
   */
  glm::vec2 size;

  /**
   * near distance, far distance
   */
  glm::vec2 range;
};

class CascadedShadowMapper: public ShadowMapper {
public:
	CascadedShadowMapper();
	explicit CascadedShadowMapper(std::vector<Cascade>& cascades);

	void activate() override;

	void evaluate() override;

	void deactivate() override;

	void update(Frustum& cameraFrustum, NAISE::RenderCore::Light* light);

	void addShadowCaster(Mesh* mesh, mat4 transform, AABB aabb) override;

	RenderCommandBuffer generateCommandBuffer() override;

	RenderCommandBuffer generateDebugCommandBuffer();

	std::vector<std::unique_ptr<ShadowMap>> shadowCascades;
	std::vector<Cascade> cascades;

private:
	ShadowShader _shadowShader;

	glm::mat4 _shadowViewMatrix = mat4(1);
	std::vector<AABB> _shadowCascadeAABBs;
	std::vector<Frustum> _shadowCascadeFrustums;
	std::vector<glm::mat4> _shadowProjectionMatrix;

	using ShadowCasterInstances = map<Mesh*, vector<glm::mat4>>;
	vector<ShadowCasterInstances> _cascadeInstances;

	uint64_t _commandBufferSize = 10;
};

}
}
