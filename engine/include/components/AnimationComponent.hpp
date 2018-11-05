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
  LOOP
};

template<typename T>
struct KeyFrame {

  KeyFrame(float timePoint, T value): timePoint(timePoint), value(value) {};

  float timePoint;
  T value;
};

template<typename T>
struct AnimationProperty {
  AnimationProperty() = default;

  AnimationProperty(std::vector<float> timePoints, std::vector<T> values) {
	  assert(timePoints.size() == values.size());
	  for (size_t i = 0; i < timePoints.size(); ++i) {
		  keyFrames.emplace_back(timePoints[i], values[i]);
	  }
  }

  size_t currentKeyFrame = 0;

  vector<KeyFrame<T>> keyFrames;
};

struct TransformAnimation {
  std::string name;
  AnimationLoop loopBehaviour = ONCE;

  float state = 0;
  bool playing = false;

  float tMax = std::numeric_limits<float>::infinity();

  AnimationProperty<vec3> position;
  AnimationProperty<quat> rotation;
  AnimationProperty<vec3> scale;

  bool ended() {
	  return state >= tMax;
  }

  void reset() {
	  position.currentKeyFrame = 0;
	  rotation.currentKeyFrame = 0;
	  scale.currentKeyFrame = 0;
	  state = 0;
  }
};

class TransformAnimationComponent: public Component {
public:
	TransformAnimationComponent() = default;

	std::vector<TransformAnimation> animations;
};

}
}
