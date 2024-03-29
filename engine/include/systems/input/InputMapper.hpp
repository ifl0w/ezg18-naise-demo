#pragma once

#include <utility>
#include <string>
#include <any>
#include <vector>
#include <unordered_map>

#include <SDL2/SDL_events.h>
#include <spdlog/spdlog.h>
#include <typeindex>

#include <systems/SystemsManager.hpp>
#include <Logger.hpp>

namespace NAISE {
namespace Engine {

using InputValue = std::any;
using InputValueType = string;

using Action = type_index; //type_info;

template <typename T>
Action make_action() {
	return type_index(typeid(T));
}

template <typename T>
bool is_action(Action test) {
	return type_index(typeid(T)) == test;
}

class Input {
public:
	Input() = default;

	template <typename T>
	T get(const string& name);

	void set(const string& name, InputValue value) {
		values[name] = std::move(value);
	}

	std::unordered_map<InputValueType, InputValue> values;
};

template<typename T>
T Input::get(const string& name) {
	InputValue iv;
	T ret = 0;

	try {
		iv = values.at(name);

		try {
			ret = std::any_cast<T>(iv);
		} catch (std::bad_any_cast& e) {
			NAISE_WARN_CONSOL("Bad any cast");
			// swallow and return default
		}

	} catch (std::out_of_range& e) {
		// swallow and ignore
	}

	return ret;
}

class InputMapper {
public:
	virtual std::vector<Action> resolve(const SDL_Event& event) { return vector<Action>(); };
	virtual Input input(Action action, Input& input, const SDL_Event& event) { return input; };
	virtual void handleEvent(const SDL_Event& event) { };
};

}
}
