#pragma once

#include "../System.hpp"

#include <vector>

namespace NAISE {
namespace Engine {

struct Animation {};

class AnimationSystem: public System {
public:
	void process(std::chrono::microseconds deltaTime) override;

private:
	std::vector<Animation> animations;
};

}
}
