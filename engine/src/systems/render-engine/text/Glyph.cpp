#include <systems/render-engine/text/Glyph.hpp>

#include <ft2build.h>
#include <freetype/freetype.h>

using namespace NAISE::RenderCore;
using namespace gl;

Glyph::Glyph(std::string fontFile, int characterSize) {
	// initialize the FreeType library
	FT_Library ft;
	if (FT_Init_FreeType(&ft)) {
		std::runtime_error("NAISE::GAMES::Engine :: Could not init FreeType Library");
	}

	// load a font as a face
	FT_Face face;
	if (FT_New_Face(ft, fontFile.c_str(), 0, &face)) {
		std::runtime_error("NAISE::GAMES::Engine :: Font could not be loaded");
	}

	// define the font size with width = 0 and height = characterSize
	// the width will be dynamically calculate based on the given height
	FT_Set_Pixel_Sizes(face, 0, static_cast<FT_UInt>(characterSize));

	// generate textures for 128 characters of the ASCII and store them
	glPixelStorei(GL_UNPACK_ALIGNMENT, 1); // Disable byte-alignment restriction, only use a single byte per pixel
	for (GLubyte c = 0; c < 128; c++) {
		// Load character glyph
		if (FT_Load_Char(face, c, FT_LOAD_RENDER)) {
			std::runtime_error("NAISE::GAMES::Engine :: Failed to load Glyph");
			continue;
		}

		// Generate texture
		GLuint texture;
		glGenTextures(1, &texture);
		glBindTexture(GL_TEXTURE_2D, texture);
		glTexImage2D(
				GL_TEXTURE_2D,
				0,
				GL_RED,
				face->glyph->bitmap.width,
				face->glyph->bitmap.rows,
				0,
				GL_RED,
				GL_UNSIGNED_BYTE,
				face->glyph->bitmap.buffer
		);

		// Set texture options
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

		// Store character
		Character character = {
				texture,
				glm::ivec2(face->glyph->bitmap.width, face->glyph->bitmap.rows),
				glm::ivec2(face->glyph->bitmap_left, face->glyph->bitmap_top),
				(GLuint) (face->glyph->advance.x)
		};
		Characters.insert(std::pair<GLchar, Character>(c, character));
	}
	// Unbind
	glBindTexture(GL_TEXTURE_2D, 0);
	// Destroy FreeType
	FT_Done_Face(face);
	FT_Done_FreeType(ft);

}
