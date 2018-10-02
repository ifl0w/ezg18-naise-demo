#pragma once

#include <typeinfo>
#include <scene/Entity.hpp>

namespace NAISE {
namespace Engine {

class Filter {
public:
	bool match(const Entity& entity);

	template <typename T>
	void requirement();

private:
	vector<type_index> _required;
};

template<typename T>
void Filter::requirement() {
	auto x = type_index(typeid(T));
	spdlog::get("console")->debug(x.name());
	_required.push_back(type_index(typeid(T)));
}

}
}
