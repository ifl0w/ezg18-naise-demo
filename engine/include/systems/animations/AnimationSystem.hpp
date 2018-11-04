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
	void animateProperty(KeyFrames<T> keyFrames, T& target, float& state);

	template<typename T>
	T linearInterpolation(T val1, T val2, float factor);

};

template<typename T>
T AnimationSystem::linearInterpolation(T val1, T val2, float factor) {
	return val1 * (1 - factor) + val2 * factor;
}

template<typename T>
void AnimationSystem::animateProperty(KeyFrames<T> keyFrames, T& target, float& state) {
	if (keyFrames.empty()) {
		return;
	}

	auto posIt = keyFrames.lower_bound(state);

	auto posTime2 = posIt->first;
	auto posVal2 = posIt->second;
	if (posIt != keyFrames.begin()) {
		posIt--;
		auto posTime1 = posIt->first;
		auto posVal1 = posIt->second;
		auto posFactor = (state - posTime1) / (posTime2 - posTime1);

		T newPos = linearInterpolation(posVal1, posVal1, posFactor);
		target = newPos;
	} else {
		target = posVal2;
	}
}

}
}
