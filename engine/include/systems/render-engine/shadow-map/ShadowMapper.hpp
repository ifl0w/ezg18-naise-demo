#pragma once

#include "memory"
#include "ShadowMap.hpp"

namespace NAISE {
namespace RenderCore {

class ShadowMapper {
public:
	ShadowMapper();

	virtual void activate() {};
	virtual void evaluate() {};
	virtual void deactivate() {};

	std::unique_ptr<ShadowMap> shaowMap;
};

}
}

