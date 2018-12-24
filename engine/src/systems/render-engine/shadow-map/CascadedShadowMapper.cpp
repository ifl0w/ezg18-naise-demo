#include <systems/render-engine/shadow-map/CascadedShadowMapper.hpp>

using namespace NAISE::RenderCore;

CascadedShadowMapper::CascadedShadowMapper(std::vector<Cascade>& cascades): _cascades(cascades) {
	for(auto& cascade: cascades) {
		_shadowCascades.push_back(std::make_unique<ShadowMap>(cascade.size.x, cascade.size.y));
	}
}

void CascadedShadowMapper::activate() {
	ShadowMapper::activate();
}

void CascadedShadowMapper::evaluate() {
	ShadowMapper::evaluate();
}

void CascadedShadowMapper::deactivate() {
	ShadowMapper::deactivate();
}
