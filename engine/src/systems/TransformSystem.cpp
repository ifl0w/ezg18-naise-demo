#include <systems/TransformSystem.hpp>
#include <systems/RenderSystem.hpp>

#include <Engine.hpp>
#include <components/ParentComponent.hpp>
#include <components/TransformComponent.hpp>
#include <components/LightComponent.hpp>
#include <components/AABBComponent.hpp>
#include <components/CameraComponent.hpp>

using namespace NAISE::Engine;

TransformSystem::TransformSystem() {
	Engine::getEntityManager().addSignature<TransformSignature>();
	Engine::getEntityManager().addSignature<ParentSignature>();
	Engine::getEntityManager().addSignature<AABBSignature>();
	Engine::getEntityManager().addSignature<CameraSignature>();

	// remove all child entities if an entity is removed
	// TODO: asynchronous execution of events would allow deletion of entities without buffering in "del" vector
	Engine::getEventManager().event<RuntimeEvents::EntityRemoved>().subscribe([&](EntityID id) {
	  auto& p = Engine::getEntityManager().getEntities<ParentSignature>();
	  auto del = vector<EntityID>();

	  for (auto e: p) {
	  	if (e->component<ParentComponent>().parent == id) {
			del.push_back(e->id);
	  	}
	  }

	  Engine::getEntityManager().removeEntities(del);
	});
}

void TransformSystem::process(microseconds deltaTime) {

	// calculate all local model matrices
	auto& t = Engine::getEntityManager().getEntities<TransformSignature>();
	for (auto e: t) {
		e->component<TransformComponent>().calculateLocalModelMatrix();
	}

	auto& p = Engine::getEntityManager().getEntities<ParentSignature>();
	// reset evaluated flags
	for (auto e: p) {
		e->component<ParentComponent>().evaluated = false;
	}
	for (auto e: p) {
		evaluateNode(*e);
	}

	auto& b = Engine::getEntityManager().getEntities<AABBSignature>();
	for (auto e: b) {
		auto& tc = e->component<TransformComponent>();
		auto& aabb = e->component<AABBComponent>().aabb;

		aabb.transform(tc.getModelMatrix());
	}

	// TODO: move normalization and frustum calculation to own system
	for (auto entity: Engine::getEntityManager().getEntities<CameraSignature>()) {
		auto& transformComp = entity->component<TransformComponent>();
		auto& cameraComponent = entity->component<CameraComponent>();

		transformComp.rotation = glm::normalize(transformComp.rotation);

		// Recalculate camera frustum.
		// This has to be done always after camera movement.
		cameraComponent.frustum.recalculate(transformComp.getModelMatrix());
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
