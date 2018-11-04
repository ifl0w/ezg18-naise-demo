#include <systems/animations/AnimationSystem.hpp>
#include <Engine.hpp>

using namespace NAISE::Engine;
using namespace std::chrono;

AnimationSystem::AnimationSystem() {
	Engine::getEntityManager().addSignature<TransformAnimationSignature>();
}

void AnimationSystem::process(microseconds deltaTime) {
	std::chrono::duration<float> sec = deltaTime;

	auto& animEntities = Engine::getEntityManager().getEntities<TransformAnimationSignature>();
	for (auto& entity: animEntities) {
		auto& animComp = entity->component<TransformAnimationComponent>();
		auto& transformComp = entity->component<TransformComponent>();

		for (auto& anim: animComp.animations) {
			if (anim.playing) {

				animateProperty(anim.position, transformComp.position, anim.state);
				animateProperty(anim.rotation, transformComp.rotation, anim.state);
				animateProperty(anim.scale, transformComp.scale, anim.state);

				anim.state += sec.count();

				if (anim.state >= anim.tMax) {
					anim.state = 0;
				}

			}
		}

	}
}
