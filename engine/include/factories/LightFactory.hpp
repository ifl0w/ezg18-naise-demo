#pragma once

#include <components/LightComponent.hpp>

namespace NAISE {
namespace Engine {

class LightFactory {
public:
	template <typename T, typename  ... Args>
	static shared_ptr<LightComponent> createLight(Args... args) {
		auto comp = std::make_shared<LightComponent>();

		comp->type = typeid(T);
		comp->light = make_shared<T>(args ...);

		return comp;
	};
};

}
}

