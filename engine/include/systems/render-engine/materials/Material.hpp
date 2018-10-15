#pragma once

#include "systems/render-engine/materials/shaders/ShaderUtils.hpp"

#include "systems/render-engine/materials/shaders/Shader.hpp"
//#include "../textures/Texture.h"
//#include "../animations/VertexDisplacementAnimation.h"

using namespace glm;
using namespace NAISE::Engine;
//using namespace NAISE::Engine::Texture;

namespace NAISE {
namespace RenderCore {

constexpr GLenum WIREFRAME = GL_LINE;
constexpr GLenum FACES = GL_FILL;
constexpr GLenum VERTICES = GL_POINT;

enum ShadowMode { SHADOW = true, NO_SHADOW = false };

class Material {
public:
	Material()
			: forwardShading(false) { };

	explicit Material(bool forwardShading)
			: forwardShading(forwardShading) { };

	virtual ~Material() = default;

	/**
	 * Initialize all necessary uniforms for this material.
	 * TODO: move useShader into this function if possible to ensure precondition
	 *
	 * @pre material->useShader() was called.
	 * @post OpenGL state is prepared with this material
	 */
	virtual void useMaterial() const = 0;

	const bool forwardShading = false;

	std::shared_ptr<Shader> shader;

//	std::shared_ptr<ANIMATION::VertexDisplacementAnimation> vertexDisplacementAnimation;
//	GLint useWatermeshAnimationLocation = -1;

	bool wireframe = false;

	GLenum polygonMode = FACES;
	ShadowMode shadowMode = SHADOW;

//	void setRenderProperty_PolygonMode(RenderProperties_PolygonMode POLYGONMODE); //bind and unbind
//	void setRenderProperty_ShadowMode(RenderProperties_ShadowMode SHADOWMODE);
//	GLenum renderPolygonMode(); //bind and unbind
//	bool renderShadowMode();
};

}
}
