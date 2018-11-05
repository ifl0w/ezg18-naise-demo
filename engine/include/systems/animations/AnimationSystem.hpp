#pragma once

#include "../System.hpp"

#include <vector>

#include <components/TransformComponent.hpp>
#include <components/AnimationComponent.hpp>

namespace NAISE {
namespace Engine {

struct TransformAnimationSignature: public Signature<TransformComponent, TransformAnimationComponent> {};

class AnimationSystem: public System {
public:
	AnimationSystem();

	void process(std::chrono::microseconds deltaTime) override;

private:

	template<typename T>
	void animateProperty(AnimationProperty<T>& property, T& target, float& state, float progression);

	template<typename T>
	T linearInterpolation(T val1, T val2, float factor);

};

template<typename T>
T AnimationSystem::linearInterpolation(T val1, T val2, float factor) {
	return val1 * (1 - factor) + val2 * factor;
}

template<typename T>
void AnimationSystem::animateProperty(AnimationProperty<T>& property, T& target, float& state, float progression) {
	if (property.keyFrames.empty()) {
		return;
	}

	auto keyFrameIdx = property.currentKeyFrame;
	auto t2 = property.keyFrames[keyFrameIdx].timePoint;
	auto v2 = property.keyFrames[keyFrameIdx].value;

	if (keyFrameIdx == 0) {
		target = v2;
	} else {
		auto t1 = property.keyFrames[keyFrameIdx-1].timePoint;
		auto v1 = property.keyFrames[keyFrameIdx-1].value;

		auto factor = (t2 - state) / (t2 - t1);

		target = linearInterpolation(v1, v2, factor);
	}

	// update current key Frame
	while (state + progression > t2
		&& property.currentKeyFrame < property.keyFrames.size()) {
		property.currentKeyFrame++;
		t2 = property.keyFrames[property.currentKeyFrame].timePoint;
	}

}

}
}
