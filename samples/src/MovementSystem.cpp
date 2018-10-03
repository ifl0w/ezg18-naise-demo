#include "MovementSystem.hpp"
#include "GameInputMapper.hpp"

#define GLM_ENABLE_EXPERIMENTAL
#include <glm/gtx/rotate_vector.hpp>

void MovementSystem::process(const NAISE::Engine::EntityManager& em, microseconds deltaTime) {
	em.filter(movementFilter, [=](Entity& entity){
	  std::chrono::duration<float> sec = deltaTime;

	  float moveSpeed = 10;
	  float mouseSpeed = 0.1;

	  auto& inputComponent = entity.component<InputComponent>();
	  auto& transform = entity.component<TransformComponent>();

	  if (inputComponent.hasAction<Actions::MoveForward>()) {
	  	auto& inp = inputComponent.action<Actions::MoveForward>();
	  	bool active = inp.get<bool>("active");

	  	if(active) {
	  		vec3 orientationAxis = axis(transform.rotation);
	  		vec3 movement = vec3(0,0, -moveSpeed * sec.count());
			transform.position += rotate(movement, angle(transform.rotation), orientationAxis);
	  	}
	  }

	  if (inputComponent.hasAction<Actions::MoveBackward>()) {
		  auto& inp = inputComponent.action<Actions::MoveBackward>();
		  bool active = inp.get<bool>("active");

		  if(active) {
			  vec3 orientationAxis = axis(transform.rotation);
			  vec3 movement = vec3(0,0, moveSpeed * sec.count());
			  transform.position += rotate(movement, angle(transform.rotation), orientationAxis);
		  }
	  }

	  if (inputComponent.hasAction<Actions::MoveLeft>()) {
		  auto& inp = inputComponent.action<Actions::MoveLeft>();
		  bool active = inp.get<bool>("active");

		  if(active) {
			  vec3 orientationAxis = axis(transform.rotation);
			  vec3 movement = vec3(-moveSpeed * sec.count(), 0, 0);
			  transform.position += rotate(movement, angle(transform.rotation), orientationAxis);
		  }
	  }

	  if (inputComponent.hasAction<Actions::MoveRight>()) {
		  auto& inp = inputComponent.action<Actions::MoveRight>();
		  bool active = inp.get<bool>("active");

		  if(active) {
			  vec3 orientationAxis = axis(transform.rotation);
			  vec3 movement = vec3(moveSpeed * sec.count(), 0, 0);
			  transform.position += rotate(movement, angle(transform.rotation), orientationAxis);
		  }
	  }

	  if (inputComponent.hasAction<Actions::MouseMotion>()) {
		  auto& inp = inputComponent.action<Actions::MouseMotion>();

		  if (inp.get<bool>("active")) {
			  float deltaX = inp.get<int>("delta_x");
			  float deltaY = inp.get<int>("delta_y");
			  inp.set("delta_x", 0);
			  inp.set("delta_y", 0);
			  inp.set("active", false);

			  deltaX *= mouseSpeed;
			  deltaY *= mouseSpeed;

			  auto tmp = glm::rotate(transform.rotation, radians<float>(-deltaX), vec3(0,1,0));
			  tmp = glm::rotate(tmp, radians<float>(-deltaY), vec3(1,0,0));

			  transform.rotation = tmp;
		  }
	  }

	});
}
