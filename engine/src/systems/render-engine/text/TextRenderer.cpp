#include <glm/gtc/matrix_transform.hpp>
#include <systems/render-engine/text/TextRenderer.hpp>

using namespace NAISE::RenderCore;
using namespace NAISE::Engine;
using namespace gl;

TextRenderer::TextRenderer()
		: textShader(std::make_unique<TextShader>()) {
	initText();
}

void TextRenderer::initText() {
	// Configure VAO/VBO for texture quadsgetOrthogonalModelMatrix
	configureVAOVBOforTextureQuads();
	this->colorLocation = uniformLocation(textShader->shaderID, "textColor");
}

void TextRenderer::configureVAOVBOforTextureQuads() {
	glGenVertexArrays(1, &VAO);
	glGenBuffers(1, &VBO);
	glBindVertexArray(VAO);
	glBindBuffer(GL_ARRAY_BUFFER, VBO);
	glBufferData(GL_ARRAY_BUFFER, sizeof(GLfloat) * 6 * 4, nullptr, GL_DYNAMIC_DRAW);
	glEnableVertexAttribArray(0);
	glVertexAttribPointer(0, 4, GL_FLOAT, GL_FALSE, 4 * sizeof(GLfloat), 0);
	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glBindVertexArray(0);
}

void TextRenderer::render(std::string text, Font* font, glm::vec3 color, mat4 transform) {

	GLfloat xPosition = 0;
	GLfloat yPosition = 0;
	GLfloat xScale = 1;
	GLfloat yScale = 1;
	GLfloat xbound = 0;
	GLfloat ybound = 0;

	bool centerText = true;

	vec2 bound = calculateBound(text, font);

	if (textShader->shaderID != Shader::activeShader) {
		textShader->useShader();
	}

	textShader->setModelMatrix(transform);

	glUniform3f(colorLocation, color.x, color.y, color.z);
	glActiveTexture(GL_TEXTURE0);
	glBindVertexArray(VAO);

	// Iterate through all characters and calculate globalBound
	std::string::const_iterator c;
	for (c = text.begin(); c != text.end(); c++) {

		Glyph ch = font->glyphs[*c];

		GLfloat xpos = xPosition + ch.Bearing.x * xScale;
		GLfloat ypos = yPosition - (ch.Size.y - ch.Bearing.y) * yScale;

		if (centerText) {
			xpos -= bound.x/2;
			ypos -= bound.y/2;
		}

		GLfloat w = ch.Size.x * xScale;
		GLfloat h = ch.Size.y * yScale;
		ybound = std::max(ybound, h);

		// Update VBO for each character
		GLfloat vertices[6][4] = {
				{xpos, ypos + h, 0.0, 0.0},
				{xpos, ypos, 0.0, 1.0},
				{xpos + w, ypos, 1.0, 1.0},

				{xpos, ypos + h, 0.0, 0.0},
				{xpos + w, ypos, 1.0, 1.0},
				{xpos + w, ypos + h, 1.0, 0.0}
		};

		// Render glyph texture over quad
		glBindTexture(GL_TEXTURE_2D, ch.TextureID);

		// Update content of VBO memory
		glBindBuffer(GL_ARRAY_BUFFER, VBO);
		glBufferSubData(GL_ARRAY_BUFFER, 0, sizeof(vertices), vertices);
		glBindBuffer(GL_ARRAY_BUFFER, 0);

		// Render quad
		glDrawArrays(GL_TRIANGLES, 0, 6);

		// Now advance cursors for next glyph (1/64 pixels)
		xPosition += (ch.Advance >> 6) * xScale; // Bitshift by 6 to get value in pixels (2^6 = 64)
		xbound += (ch.Advance / 64) * xScale;
	}

	glBindVertexArray(0);
	glBindTexture(GL_TEXTURE_2D, 0);
}

vec2 TextRenderer::calculateBound(std::string text, Font* font) {
	vec2 bound = vec2(0,0);
	vec2 scale = vec2(1,1);

	// Iterate through all characters and calculate globalBound
	std::string::const_iterator c;
	for (c = text.begin(); c != text.end(); c++) {
		Glyph ch = font->glyphs[*c];

		GLfloat h = ch.Size.y * scale.y;

		bound.y = std::max(bound.y, h);
		bound.x += (ch.Advance / 64) * scale.x;
	}

	return bound;
}
