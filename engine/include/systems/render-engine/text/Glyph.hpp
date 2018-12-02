#pragma once

#include <string>
#include <glm/glm.hpp>
#include <glbinding/gl/gl.h>
#include <map>
#include <systems/render-engine/materials/Material.hpp>

namespace NAISE {
namespace RenderCore {

/// Holds all state information relevant to a character as loaded using FreeType
class Glyph: NAISE::RenderCore::Material {
public:
  gl::GLuint TextureID;   // ID handle of the glyph texture
  glm::ivec2 Size;    // Size of glyph
  glm::ivec2 Bearing;  // Offset from baseline to left/top of glyph
  gl::GLuint Advance;    // Horizontal offset to advance to next glyph

  void useMaterial() const override;
};

class Font {
public:
	Font(std::string fontFile, int characterSize);

	std::map<gl::GLchar, Glyph> glyphs;
};

}
}


