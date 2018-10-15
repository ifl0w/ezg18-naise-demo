#pragma once

#include <components/MeshComponent.hpp>

#include <vector>
#include <memory>

using namespace glm;
using namespace std;

namespace NAISE {
namespace Engine {

class MeshFactory {
public:
	static shared_ptr<MeshComponent> createBox(float width = 1.0, float height = 1.0, float depth = 1.0);
	static shared_ptr<MeshComponent> createSphere(float radius = 1.0, uint32_t segments = 32, uint32_t rings = 16);

	template <typename T, typename ... Args>
	static shared_ptr<MeshComponent> create(Args... args);

private:
	static void fillBuffers(MeshComponent& c);
};

template<typename T, typename... Args>
shared_ptr<MeshComponent> MeshFactory::create(Args... args) {
	auto comp = std::make_shared<MeshComponent>();

	comp->mesh = make_shared<T>(args ...);

	return comp;
}


}
}
