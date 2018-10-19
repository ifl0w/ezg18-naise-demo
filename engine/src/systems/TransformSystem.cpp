#include <systems/TransformSystem.hpp>

#include <Engine.hpp>
#include <components/ParentComponent.hpp>
#include <components/TransformComponent.hpp>
#include <components/LightComponent.hpp>
#include <components/AABBComponent.hpp>

using namespace NAISE::Engine;

TransformSystem::TransformSystem() {
	parentFilter.requirement<ParentComponent>();
	transformFilter.requirement<TransformComponent>();

	boundingBoxFilter.requirement<TransformComponent>();
	boundingBoxFilter.requirement<AABBComponent>();
}

void TransformSystem::process(const EntityManager& em, microseconds deltaTime) {
	// calculate all local model matrices
	em.filter(transformFilter, [&](Entity& entity) {
	  entity.component<TransformComponent>().calculateLocalModelMatrix();
	});

	// reset evaluated flags
	em.filter(parentFilter, [&](Entity& entity) {
	  entity.component<ParentComponent>().evaluated = false;
	});

	em.filter(parentFilter, [&](Entity& entity) {
	  evaluateNode(entity);
	});

	em.filter(boundingBoxFilter, [&](Entity& entity) {
	  auto& tc = entity.component<TransformComponent>();
	  auto& aabb = entity.component<AABBComponent>().aabb;

	  aabb.transform(tc.getModelMatrix());
	});
}

mat4 TransformSystem::evaluateNode(Entity& entity) {
	mat4 ret = mat4(1);

	if (!entity.has<ParentComponent>()) {
		if (entity.has<TransformComponent>()) {
			ret = entity.component<TransformComponent>().getLocalTransform();
		}

		return ret;
	}

	auto& pc = entity.component<ParentComponent>();
	EntityID parentId = pc.parent;
	Entity& parent = *Engine::getEntityManager().getEntity(parentId);

	if (!pc.evaluated) {
		ret = evaluateNode(parent);
		pc.cachedParentTransform = ret;
	} else {
		ret = pc.cachedParentTransform;
	}

	if (entity.has<TransformComponent>()) {
		auto& tc = entity.component<TransformComponent>();
		tc.parentTransform = ret;
		ret *= tc.localTransform;
		tc.globalTransform = ret;

		tc.updateGlobalValues();
	}

	pc.evaluated = true;
	return ret;

}
