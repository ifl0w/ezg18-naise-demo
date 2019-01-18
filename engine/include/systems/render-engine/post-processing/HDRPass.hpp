#pragma once

#include <memory>
#include <glm/glm.hpp>

#include <systems/render-engine/textures/Texture.hpp>
#include <systems/render-engine/PostProcessingTarget.hpp>
#include <systems/particle-system/ComputeShader.hpp>
#include <systems/render-engine/meshes/Plane.hpp>
#include "../RenderCommands.hpp"

using namespace NAISE::Engine;

namespace NAISE {
namespace RenderCore {

struct PostProcessingPass {
  PostProcessingPass(int viewportWidth, int viewportHeight)
		  : viewportWidth(viewportWidth),
			viewportHeight(viewportHeight) { };

  std::unique_ptr<Plane> quad = std::make_unique<Plane>(2.0f, 2.0f);

  int viewportWidth = 1024;
  int viewportHeight = 768;
};

class HDRPass : public PostProcessingPass {
public:
	HDRPass(int viewportWidth, int viewportHeight);
	~HDRPass();

	RenderCommandBuffer generateCommandBuffer(PostProcessingTarget* lightTarget, float deltaTime);

	std::unique_ptr<PostProcessingTarget> target;
private:

	GLuint ssbo_histogramBins;
	uint32_t bins = 100;

	ComputeShader luminanceReductionCompute = ComputeShader(
			"engine/resources/shaders/post-processing/luminance_reduction.glsl");
	ComputeShader luminanceCompute = ComputeShader("engine/resources/shaders/post-processing/luminance.glsl");
//	ComputeShader histogramCompute = ComputeShader("engine/resources/shaders/post-processing/histogram.glsl");
	Shader hdrShader = Shader("engine/resources/shaders/post-processing/quad.vert",
							  "engine/resources/shaders/post-processing/hdr.frag");

	std::unique_ptr<Texture> luminanceTexture;
	std::unique_ptr<Texture> luminanceTexture2;
};

}
}
