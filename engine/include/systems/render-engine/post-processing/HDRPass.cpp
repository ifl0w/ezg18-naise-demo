#include "HDRPass.hpp"

using namespace NAISE::RenderCore;
using namespace std;
using namespace glm;

HDRPass::HDRPass(int viewportWidth, int viewportHeight)
	: PostProcessingPass(viewportWidth, viewportHeight) {
	target = make_unique<PostProcessingTarget>(viewportWidth, viewportHeight, 0);
	luminanceTexture = make_unique<Texture>(ivec2(viewportWidth, viewportHeight));
	luminanceTexture2 = make_unique<Texture>(ivec2(viewportWidth, viewportHeight));

	glGenBuffers(1, &ssbo_histogramBins);
	glBindBuffer(GL_SHADER_STORAGE_BUFFER, ssbo_histogramBins);
	glBufferData(GL_SHADER_STORAGE_BUFFER, sizeof(glm::vec4) * bins, nullptr, GL_STATIC_DRAW);
	glBindBuffer(GL_SHADER_STORAGE_BUFFER, 0);
}

HDRPass::~HDRPass() {

}

RenderCommandBuffer HDRPass::generateCommandBuffer(PostProcessingTarget* lightTarget, float deltaTime) {
	RenderCommandBuffer buffer;

	glDepthMask(GL_FALSE);
	glDisable(GL_DEPTH_TEST);

	auto mipmapCount = (int) (log(glm::max(lightTarget->width, lightTarget->height)) / log(2));

	// generate luminance image
	luminanceCompute.useShader();

	glBindImageTexture(0, lightTarget->output, 0, GL_FALSE, 0, GL_READ_ONLY, GL_RGBA16F);
	glBindImageTexture(1, luminanceTexture->textureID, 0, GL_FALSE, 0, GL_READ_WRITE, GL_RGBA16F);

	auto groups = ivec3((viewportWidth / 16) + 1, (viewportHeight / 16) + 1, 1);
	luminanceCompute.compute(groups);

	// reduce luminance image
//	luminanceReductionCompute.useShader();
//
//	glBindImageTexture(0, luminanceTexture->textureID, 0, GL_FALSE, 0, GL_READ_WRITE, GL_RGBA16F);
//	for (int i = 0; i <= mipmapCount; ++i) {
//		int iteration = i;
//
//		glUniform1i(glGetUniformLocation(luminanceReductionCompute.shaderID, "iteration"), iteration);
//
//		groups = ivec3((viewportWidth / (16 * (iteration + 1))) + 1,  (viewportHeight / (16 * (iteration + 1))) + 1, 1);
//		luminanceCompute.compute(groups);
//	}

	glBindTexture(GL_TEXTURE_2D, luminanceTexture->textureID);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAX_LEVEL, mipmapCount);
	glGenerateMipmap(GL_TEXTURE_2D);

	target->use();
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	hdrShader.useShader();

	glActiveTexture(GL_TEXTURE0 + 0);
	glBindTexture(GL_TEXTURE_2D, lightTarget->output);

	glBindImageTexture(0, luminanceTexture->textureID, 0, GL_FALSE, 0, GL_READ_WRITE, GL_RGBA16F);
	glBindImageTexture(1, luminanceTexture->textureID, mipmapCount - 1, GL_FALSE, 0, GL_READ_ONLY, GL_RGBA16F);

	glUniform1f(glGetUniformLocation(hdrShader.shaderID, "deltaTime"), deltaTime);
	glUniform1i(glGetUniformLocation(hdrShader.shaderID, "imageInput"), 0);
	glUniform1i(glGetUniformLocation(hdrShader.shaderID, "mipmapCount"), mipmapCount);

	hdrShader.setModelMatrix(mat4(1.0));

	buffer.push_back(DrawMeshDirect {
		quad.get(),
		glm::mat4()
	});

	glEnable(GL_DEPTH_TEST);
	glDepthMask(GL_TRUE);

	return buffer;
}
