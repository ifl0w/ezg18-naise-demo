#include <utility>

#pragma once

#include <systems/render-engine/frustum-culling/AABB.hpp>
#include <components/Component.hpp>

#include <map>

using namespace NAISE::RenderCore;

namespace NAISE {
namespace Engine {

enum AnimationLoop {
  ONCE,
  LOOP,
  PING_PONG
};

template<typename T>
using KeyFrames = std::map<float, T>;

struct TransformAnimation {
  std::string name;
  AnimationLoop loopBehaviour;

  float tMin = 0;
  float tMax = std::numeric_limits<float>::infinity();
  float state = 0;

  bool playing = false;

  KeyFrames<vec3> position;
  KeyFrames<glm::quat> rotation;
  KeyFrames<vec3> scale;
};

class TransformAnimationComponent: public Component {
public:
	TransformAnimationComponent() = default;

	std::vector<TransformAnimation> animations;
};

}
}
