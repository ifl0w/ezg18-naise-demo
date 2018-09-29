#pragma once

#include <typeinfo>

namespace NAISE {
namespace Engine {

class Component {
public:
	template <typename T, typename S>
	static bool match() {
		return typeid(T) == typeid(S);
	}
};

}
}
