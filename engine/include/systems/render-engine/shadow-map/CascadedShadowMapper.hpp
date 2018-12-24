#pragma once

#include <systems/render-engine/shadow-map/ShadowMapper.hpp>
#include <systems/render-engine/shadow-map/ShadowMap.hpp>

#include <glm/vec2.hpp>

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
	explicit CascadedShadowMapper(std::vector<Cascade>& cascades);

	void activate() override;

	void evaluate() override;

	void deactivate() override;

private:
	std::vector<std::unique_ptr<ShadowMap>> _shadowCascades;
	std::vector<Cascade> _cascades;
};

}
}
