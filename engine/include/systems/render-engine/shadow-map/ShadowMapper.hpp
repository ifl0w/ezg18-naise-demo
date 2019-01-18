#pragma once

#include <memory>

#include "scene/Entity.hpp"

#include "ShadowMap.hpp"
#include "../RenderCommands.hpp"

using namespace NAISE::Engine;

namespace NAISE {
namespace RenderCore {

class ShadowMapper {
public:
	ShadowMapper();

	virtual void activate() {};
	virtual void evaluate() {};
	virtual void deactivate() {};

	virtual void addShadowCaster(Mesh* mesh, mat4 transform, AABB aabb) {};

	virtual RenderCommandBuffer generateCommandBuffer() {
		return RenderCommandBuffer();
	}

	std::unique_ptr<ShadowMap> shaowMap;
};

}
}

