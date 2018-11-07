#pragma once

#include <systems/render-engine/frustum-culling/AABB.hpp>
#include <components/Component.hpp>

#include <map>
#include <utility>
#include <cassert>
#include <type_traits>
#include <variant>

#include <Logger.hpp>

using namespace NAISE::RenderCore;

namespace NAISE {
namespace Engine {

enum AnimationLoop {
  ONCE,
  LOOP
};

enum InterpolationMode {
  NONE,
  LINEAR,
  QUBIC_SPLINE
};

template<typename T>
struct KeyFrame {
  KeyFrame(float timePoint, T value)
		  : timePoint(timePoint),
			value(value) { };

  KeyFrame(float timePoint, std::tuple<T, T, T> value)
		  : timePoint(timePoint),
			value(value) { };

  float timePoint;
  std::variant<T, std::tuple<T, T, T>> value;
};

template<typename T>
using InterpolationFunction = std::function<T(KeyFrame<T>, KeyFrame<T>, float)>;

template<typename T>
class AnimationProperty {
public:
	AnimationProperty() = default;

	AnimationProperty(std::vector<float> timePoints, std::vector<T> values) {
		assert(timePoints.size() == values.size() || values.size() == timePoints.size() * 3);

		if (values.size() == timePoints.size()) {
			for (size_t i = 0; i < timePoints.size(); ++i) {
				keyFrames.emplace_back(timePoints[i], values[i]);
			}
		} else if(values.size() == timePoints.size() * 3) {
			for (int i = 0; i < values.size(); i += 3) {
				keyFrames.emplace_back(timePoints[i], std::tuple(values[i], values[i+1], values[i+2]));
			}
		} else {
			// error
			NAISE_ERROR_LOG("broken animation input!")
		}
	}

	InterpolationFunction<T> interpolate = [](KeyFrame<T> k1, KeyFrame<T>, float) {
	  return std::get<T>(k1.value);
	};

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

class TransformAnimationComponent : public Component {
public:
	TransformAnimationComponent() = default;

	std::vector<TransformAnimation> animations;
};

}
}
