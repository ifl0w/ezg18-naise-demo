#include <systems/TransformSystem.hpp>

#include <Engine.hpp>
#include <components/ParentComponent.hpp>
#include <components/TransformComponent.hpp>
#include <components/LightComponent.hpp>
#include <components/AABBComponent.hpp>

using namespace NAISE::Engine;

TransformSystem::TransformSystem() {
	Engine::getEntityManager().addSignature<TransformSignature>();
	Engine::getEntityManager().addSignature<ParentSignature>();
	Engine::getEntityManager().addSignature<AABBSignature>();
}

void TransformSystem::process(const EntityManager& em, microseconds deltaTime) {

	// calculate all local model matrices
	auto& t = Engine::getEntityManager().getSignature<TransformSignature>()->entities;
	for (auto e: t) {
		e->component<TransformComponent>().calculateLocalModelMatrix();
	}

	auto& p = Engine::getEntityManager().getSignature<ParentSignature>()->entities;
	// reset evaluated flags
	for (auto e: p) {
		e->component<ParentComponent>().evaluated = false;
	}
	for (auto& e: p) {
		evaluateNode(*e);
	}

	auto& b = Engine::getEntityManager().getSignature<AABBSignature>()->entities;
	for (auto e: b) {
		auto& tc = e->component<TransformComponent>();
		auto& aabb = e->component<AABBComponent>().aabb;

		aabb.transform(tc.getModelMatrix());
	}
}

mat4 TransformSystem::evaluateNode(Entity& entity) {
	mat4 ret = mat4(1);

	if (!entity.has<ParentComponent>()) {
		if (auto* transComp = entity.get<TransformComponent>()) {
			ret = transComp->getLocalTransform();
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

	if (auto tc = entity.get<TransformComponent>()) {
		tc->parentTransform = ret;
		ret *= tc->localTransform;
		tc->globalTransform = ret;

		tc->updateGlobalValues();
	}

	pc.evaluated = true;
	return ret;

}
