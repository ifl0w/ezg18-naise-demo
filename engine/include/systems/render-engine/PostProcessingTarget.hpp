#pragma once

#include "RenderTarget.hpp"
#include "systems/render-engine/shaders/GlowShader.hpp"
#include "PingPongTarget.hpp"

namespace NAISE {
namespace Engine {

class PostProcessingTarget: public RenderTarget {
public:
	PostProcessingTarget();
	PostProcessingTarget(int width, int height, int samples);
	~PostProcessingTarget() override;

	void use() override;

	GLuint input;
	GLuint output;
	GLuint depth_rbo;

	const int width;
	const int height;
private:
	int samples;
};

}
}

