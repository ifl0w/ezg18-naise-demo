#include <glm/gtc/matrix_transform.hpp>
#include <systems/render-engine/text/Text.hpp>

using namespace NAISE::RenderCore;
using namespace NAISE::Engine;
using namespace gl;

Text::Text(std::string fontFile, int characterSize, int viewportWidth, int viewportHeight)
		: font(std::make_shared<Font>(fontFile, characterSize)),
		  textShader(std::make_shared<TextShader>()),
		  viewportWidth(viewportWidth),
		  viewportHeight(viewportHeight) {

	initText();
}

void Text::initText() {
	//modelMatrix = glm::ortho(0, viewportWidth, 0, viewportHeight, -1, 1);
	modelMatrix = getOrthogonalModelMatrix(0.0f, (float) viewportWidth, 0.0f, (float) viewportHeight, -1.0f, 1.0f);
	// Configure VAO/VBO for texture quadsgetOrthogonalModelMatrix
	configureVAOVBOforTextureQuads();
	this->colorLocation = uniformLocation(textShader->shaderID, "textColor");
}

void Text::configureVAOVBOforTextureQuads() {
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

RenderCommandBuffer Text::createCommandBuffer() {
	RenderCommandBuffer buffer;

	GLfloat xPosition = position.x;
	GLfloat yPosition = position.y;
	GLfloat xScale = scale.x;
	GLfloat yScale = scale.y;
	GLfloat xbound = 0;
	GLfloat ybound = 0;

	if (textShader->shaderID != Shader::activeShader) {
		textShader->useShader();
	}

	textShader->setModelMatrix(modelMatrix);

	glUniform3f(colorLocation, color.x, color.y, color.z);
	glActiveTexture(GL_TEXTURE0);
	glBindVertexArray(VAO);

	// Iterate through all characters and calculate globalBound
	std::string::const_iterator c;
	for (c = textString.begin(); c != textString.end(); c++) {

		Glyph ch = font->glyphs[*c];

		GLfloat xpos = xPosition + ch.Bearing.x * xScale;
		GLfloat ypos = yPosition - (ch.Size.y - ch.Bearing.y) * yScale;

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

	globalBound.x = xbound;
	globalBound.y = ybound;
	glBindVertexArray(0);
	glBindTexture(GL_TEXTURE_2D, 0);
}

glm::mat4 Text::getOrthogonalModelMatrix(float left, float right, float bottom, float top, float zNear, float zFar) {
	// https://stackoverflow.com/questions/12230312/is-glmortho-actually-wrong

	glm::mat4 result = glm::mat4(1.0f);
	result[0][0] = 2 / (right - left);
	result[1][1] = 2 / (top - bottom);
	result[2][2] = -2 / (zFar - zNear);
	result[3][0] = -(right + left) / (right - left);
	result[3][1] = -(top + bottom) / (top - bottom);
	result[3][2] = -(zFar + zNear) / (zFar - zNear);
	return result;

}

void Text::setViewPort(int viewportWidth, int viewportHeight) {
	this->viewportWidth = viewportWidth;
	this->viewportHeight = viewportHeight;
	modelMatrix = getOrthogonalModelMatrix(0.0f, (float) viewportWidth, 0.0f, (float) viewportHeight, -1.0f, 1.0f);
}
