#pragma once

#include <components/MaterialComponent.hpp>

namespace NAISE {
namespace Engine {

class MaterialFactory {
public:
	template <typename T, typename  ... Args>
	static shared_ptr<MaterialComponent> createMaterial(Args... args) {
		auto comp = std::make_shared<MaterialComponent>();

		comp->material = make_shared<T>(args ...);

		return comp;
	};
};

}
}

