#pragma once

#include <glm/glm.hpp>
#include <memory>

#include "ShadowMap.hpp"

namespace NAISE {
namespace RenderCore {

struct Cascade {

  Cascade(glm::vec2 size, glm::vec2 range) {
      this->size = size;
      this->range = range;
      this->shadowMap = nullptr;
  };

  /**
   * width x height
   */
  glm::vec2 size;

  /**
   * near distance, far distance
   */
  glm::vec2 range;

  /**
   * The matrix used to transform worldspace into light space.
   */
  glm::mat4 viewMatrix = mat4(1);

  /**
   * The matrix used for the shadow projection.
   */
  glm::mat4 projectionMatrix;

  /**
   * The actual shadow map.
   */
  ShadowMap* shadowMap;
};

}
}
