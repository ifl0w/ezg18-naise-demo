#pragma once

#include <glbinding/gl/gl.h>

using namespace gl;

namespace NAISE {
namespace Engine {

class RenderTarget {
public:
	RenderTarget();
	RenderTarget(int width, int height, int samples);
	virtual ~RenderTarget();

	virtual void use() = 0;

	void retrieveDepthBuffer(RenderTarget* target);
	void retrieveDepthBuffer(GLuint fbo);

//	void attachTexture(shared_ptr<Texture>& texture) {};

	GLuint fbo;

	const int width = 1024;
	const int height = 768;
	const int samples = 0;
};

}
}
