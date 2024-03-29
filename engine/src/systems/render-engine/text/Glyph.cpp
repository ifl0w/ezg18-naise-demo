#include <systems/render-engine/text/Glyph.hpp>

#include <ft2build.h>
#include <freetype/freetype.h>

using namespace NAISE::RenderCore;
using namespace gl;

Font::Font(std::string fontFile, int characterSize) {
	// initialize the FreeType library
	FT_Library ft;
	if (FT_Init_FreeType(&ft)) {
		throw std::runtime_error("Could not init FreeType Library");
	}

	// load a font as a face
	FT_Face face;
	if (FT_New_Face(ft, fontFile.c_str(), 0, &face)) {
		throw std::runtime_error("Font could not be loaded");
	}

	// define the font size with width = 0 and height = characterSize
	// the width will be dynamically calculate based on the given height
	FT_Set_Pixel_Sizes(face, 0, static_cast<FT_UInt>(characterSize));

	// generate textures for 128 characters of the ASCII and store them
	glPixelStorei(GL_UNPACK_ALIGNMENT, 1); // Disable byte-alignment restriction, only use a single byte per pixel
	for (GLubyte c = 0; c < 128; c++) {
		// Load character glyph
		if (FT_Load_Char(face, c, FT_LOAD_RENDER)) {
			throw std::runtime_error("Failed to load Glyph");
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
		Glyph glyph;
		glyph.TextureID = texture;
		glyph.Size = glm::ivec2(face->glyph->bitmap.width, face->glyph->bitmap.rows);
		glyph.Bearing = glm::ivec2(face->glyph->bitmap_left, face->glyph->bitmap_top);
		glyph.Advance = (GLuint) (face->glyph->advance.x);

		glyphs.insert(std::pair<GLchar, Glyph>(c, glyph));
	}
	// Unbind
	glBindTexture(GL_TEXTURE_2D, 0);
	// Destroy FreeType
	FT_Done_Face(face);
	FT_Done_FreeType(ft);

}

void Glyph::useMaterial() const {
	// Render glyph texture over quad
	glBindTexture(GL_TEXTURE_2D, TextureID);
}
