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
	void animateProperty(AnimationProperty<T>& property, T& target, float state, float progress);

};

template<typename T>
void AnimationSystem::animateProperty(AnimationProperty<T>& property, T& target, float state, float progress) {
	if (property.keyFrames.empty()) {
		return;
	}

	auto keyFrameIdx = property.currentKeyFrame;

	auto k2 = property.keyFrames[keyFrameIdx];
	auto t2 = k2.timePoint;

	if (keyFrameIdx == 0 || keyFrameIdx >= property.keyFrames.size()) {
		target = property.interpolate(k2, k2, 0);
	} else {
		auto k1 = property.keyFrames[keyFrameIdx-1];
		auto t1 = k1.timePoint;

		auto factor = 1 - (t2 - state) / (t2 - t1);

		target = property.interpolate(k1, k2, factor);
	}

	// update current key Frame
	while (state + progress > t2
		&& property.currentKeyFrame < property.keyFrames.size() - 1) {
		property.currentKeyFrame++;
		t2 = property.keyFrames[property.currentKeyFrame].timePoint;
	}
}

}
}
