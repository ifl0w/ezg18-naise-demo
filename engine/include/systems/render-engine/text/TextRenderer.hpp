#pragma once

#include "Glyph.hpp"
#include "shader/TextShader.hpp"

#include <glm/glm.hpp>
#include <glbinding/gl/gl.h>
#include <string>
#include <memory>

namespace NAISE {
namespace RenderCore {

class TextRenderer {
public:

	TextRenderer();

	void render(std::string text, Font* font, glm::vec3 color, glm::mat4 transform = glm::mat4(1));

private:
	std::unique_ptr<TextShader> textShader;

	gl::GLuint VAO, VBO;
	gl::GLint colorLocation = -1;
	glm::vec2 globalBound = glm::vec2(0.0f, 0.0f);

	void initText();
	void configureVAOVBOforTextureQuads();

	vec2 calculateBound(std::string text, Font* font);
};

}
}
