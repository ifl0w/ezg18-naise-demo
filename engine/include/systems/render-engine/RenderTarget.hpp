#pragma once

#include <glbinding/gl/gl.h>

using namespace gl;

namespace NAISE {
namespace Engine {

class RenderTarget {
public:
	RenderTarget();
	virtual ~RenderTarget();

	virtual void use() = 0;

//	void attachTexture(shared_ptr<Texture>& texture) {};

	GLuint fbo;
};

}
}
