#include "MovementSystem.hpp"
#include "GameInputMapper.hpp"

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
			transform.position.z -= moveSpeed * sec.count();
	  	}
	  }

	  if (inputComponent.hasAction<Actions::MoveBackward>()) {
		  auto& inp = inputComponent.action<Actions::MoveBackward>();
		  bool active = inp.get<bool>("active");

		  if(active) {
			  transform.position.z += moveSpeed * sec.count();
		  }
	  }

	  if (inputComponent.hasAction<Actions::MoveLeft>()) {
		  auto& inp = inputComponent.action<Actions::MoveLeft>();
		  bool active = inp.get<bool>("active");

		  if(active) {
			  transform.position.x -= moveSpeed * sec.count();
		  }
	  }

	  if (inputComponent.hasAction<Actions::MoveRight>()) {
		  auto& inp = inputComponent.action<Actions::MoveRight>();
		  bool active = inp.get<bool>("active");

		  if(active) {
			  transform.position.x += moveSpeed * sec.count();
		  }
	  }

	  if (inputComponent.hasAction<Actions::MouseMotion>()) {
		  auto& inp = inputComponent.action<Actions::MouseMotion>();

		  if (inp.get<bool>("active")) {
			  float deltaX = inp.get<int>("delta_x") * mouseSpeed;
			  float deltaY = inp.get<int>("delta_y") * mouseSpeed;
			  inp.set("active", false);

			  auto tmp = glm::rotate(transform.rotation, radians<float>(-deltaX), vec3(0,1,0));
			  tmp = glm::rotate(tmp, radians<float>(-deltaY), vec3(1,0,0));

			  transform.rotation = tmp;
		  }
	  }

	});
}
