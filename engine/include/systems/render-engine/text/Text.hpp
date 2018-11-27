#pragma once

#include "Glyph.hpp"
#include "shader/TextShader.hpp"

#include <glm/glm.hpp>
#include <glbinding/gl/gl.h>
#include <string>
#include <memory>

namespace NAISE {
namespace RenderCore {

class Text {
public:

	Text(std::string fontFile, int characterSize = 48, int viewportWidth = 400, int viewportHeight = 400);

	void renderText();

	std::string textString = "Default Text";
	glm::vec2 position = glm::vec2(25.0f, 25.0f);
	glm::vec2 scale = glm::vec2(1, 1);
	glm::vec3 color = glm::vec3(0.5, 0.8f, 0.2f);

	int viewportWidth = 400;
	int viewportHeight = 400;

	glm::vec2 globalBound = glm::vec2(0.0f, 0.0f);

	void setViewPort(int viewportWidth, int viewportHeight);

private:

	glm::mat4x4 modelMatrix;

	std::shared_ptr<Glyph> glyphs;
	std::shared_ptr<TextShader> textShader;

	gl::GLuint VAO, VBO;
	gl::GLint colorLocation = -1;

	void initText();

	void configureVAOVBOforTextureQuads();

	glm::mat4 getOrthogonalModelMatrix(float left, float right, float bottom, float top, float zNear, float zFar);

};

}
}
