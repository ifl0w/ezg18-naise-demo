#include <utility>

#pragma once

#include <systems/render-engine/frustum-culling/AABB.hpp>
#include <components/Component.hpp>

using namespace NAISE::RenderCore;

namespace NAISE {
namespace Engine {

class AABBComponent: public Component {
public:
	AABBComponent() = default;
	explicit AABBComponent(AABB aabb): aabb(std::move(aabb)) {};

	AABB aabb;
};

}
}
