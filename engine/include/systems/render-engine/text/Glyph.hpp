#pragma once

#include <string>
#include <glm/glm.hpp>
#include <glbinding/gl/gl.h>
#include <map>

namespace NAISE {
namespace RenderCore {

class Glyph {
public:
	Glyph(std::string fontFile, int characterSize);

	/// Holds all state information relevant to a character as loaded using FreeType
	struct Character {
	  gl::GLuint TextureID;   // ID handle of the glyph texture
	  glm::ivec2 Size;    // Size of glyph
	  glm::ivec2 Bearing;  // Offset from baseline to left/top of glyph
	  gl::GLuint Advance;    // Horizontal offset to advance to next glyph
	};

	std::map<gl::GLchar, Character> Characters;

private:

};

}
}


