#pragma once

#include <vector>
#include <variant>
#include <typeindex>

#include <glbinding/gl/gl.h>
#include <glm/glm.hpp>

#include "meshes/Mesh.hpp"
#include "textures/Texture.hpp"

#include "text/TextRenderer.hpp"
#include "text/Glyph.hpp"

#define BIT(x) (1<<(x))

using namespace NAISE::RenderCore;

namespace NAISE {
namespace RenderCore {

enum DebugState {
  DEBUG_NONE = 0,
  DEBUG_POSITION = BIT(0),
  DEBUG_NORMAL = BIT(1),
  DEBUG_ALBEDO = BIT(2),
  DEBUG_GLOW = BIT(3)
};

struct SetShader {
  Shader* shader;
};

struct BindTexture {
  Texture* texture;
  int32_t slot; // OpenGL texture unit; values < 0 considered invalid
  Shader* shader;
  int32_t location; // OpenGL uniform location; values < 0 considered invalid
};

enum BlendMode {
  ADD,
  SUBTRACT,
  OVERLAY
};

struct SetBlendMode {
  BlendMode mode;
};

struct DrawMeshDirect {
  Mesh* mesh;
  mat4 transform;
};

struct DrawMesh {
  Mesh* mesh;
  Material* material;
  mat4 transform;
};

struct DrawText {
  Font* font;
  std::string text;
  mat4 transform;
};

struct DrawInstanced {
  Mesh* mesh;
  Material* material;
  std::vector<mat4> transforms;
};

struct DrawInstancedDirect {
  Mesh* mesh;
  std::vector<mat4> transforms;
};

struct DrawWireframeDirect {
  Mesh* mesh;
  glm::mat4 transform = glm::mat4(1);
  glm::vec3 color = glm::vec3(1);
};

struct DrawInstancedSSBO {
  Mesh* mesh;
  Material* material;
  glm::mat4 originTransformation = glm::mat4(1);
  GLuint count;
  GLuint transformSSBO;
};

enum RenderProperty {
  DEPTH_TEST,
  BACKFACE_CULLING,
  BLEND
};

struct SetRenderProperty {
  RenderProperty property;
  bool state;
};

struct SetRenderTarget {
  RenderTarget* target;
};

struct ClearRenderTarget {
  bool color = true;
  bool depth = false;
  bool stencil = false;
  bool accum = false;
};

struct SetViewport {
  glm::ivec2 offset = { 0, 0 };
  glm::ivec2 size = { 1024, 768 };
};

struct ResetViewport {};

struct SetViewProjectionData {
  mat4 viewMatrix = glm::mat4(1);
  mat4 projectionMatrix = glm::mat4(1);
  vec3 cameraPosition;
};

using RenderCommand = std::variant<
		SetShader,
		DrawMeshDirect,
		DrawMesh,
		DrawInstanced,
		DrawInstancedDirect,
		DrawInstancedSSBO,
		DrawText,
		DrawWireframeDirect,
		SetRenderTarget,
		ClearRenderTarget,
		SetViewport,
		ResetViewport,
		SetViewProjectionData,
		SetRenderProperty,
		SetBlendMode,
		BindTexture
>;

class RenderCommandBuffer : public vector<RenderCommand> {
public:
	void append(const RenderCommandBuffer& commandBuffer) {
		this->insert(this->end(), commandBuffer.begin(), commandBuffer.end());
	}
};

}
}
