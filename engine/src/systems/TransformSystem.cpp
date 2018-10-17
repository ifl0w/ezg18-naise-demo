#include <systems/TransformSystem.hpp>

#include <components/ParentComponent.hpp>
#include <components/TransformComponent.hpp>
#include <Engine.hpp>

using namespace NAISE::Engine;

TransformSystem::TransformSystem() {
	parentFilter.requirement<ParentComponent>();
	transformFilter.requirement<TransformComponent>();
}

void TransformSystem::process(const EntityManager& em, microseconds deltaTime) {
	em.filter(transformFilter, [&](Entity& entity){
	  entity.component<TransformComponent>().calculateModelMatrix();
	});

	em.filter(parentFilter, [&](Entity& entity){
	  entity.component<ParentComponent>().evaluated = false;
	});

	em.filter(parentFilter, [&](Entity& entity){
	  evaluateNode(entity);
	});
}

mat4 TransformSystem::evaluateNode(Entity& entity) {
	mat4 ret = mat4(1);

	if (!entity.has<ParentComponent>()) {
		if (entity.has<TransformComponent>()) {
			ret = entity.component<TransformComponent>().getModelMatrix();
		}

		return ret;
	}

	auto& pc = entity.component<ParentComponent>();
	EntityID parentId = pc.parent;
	Entity& parent = *Engine::getEntityManager().getEntity(parentId);

	if (!pc.evaluated){
		ret = evaluateNode(parent);
	} else {
		ret = pc.cachedTransform;
	}

	if (entity.has<TransformComponent>()) {
		auto& tc = entity.component<TransformComponent>();
		ret *= tc.transform;
		tc.transform = ret;
	}

	pc.evaluated = true;
	return ret;

}
