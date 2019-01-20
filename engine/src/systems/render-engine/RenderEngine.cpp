#include <systems/render-engine/RenderEngine.hpp>
#include <systems/render-engine/materials/PBRMaterial.hpp>

#include <components/MaterialComponent.hpp>
#include <components/LightComponent.hpp>
#include <components/AABBComponent.hpp>

#include <Logger.hpp>

using namespace NAISE::Engine;
using namespace std;
using namespace glm;

RenderEngine::RenderEngine()
		: RenderEngine(1024, 768) { };

RenderEngine::RenderEngine(int viewportWidth, int viewportHeight)
		: viewportWidth(viewportWidth),
		  viewportHeight(viewportHeight),
		  _defaultMaterial(make_unique<PBRMaterial>(vec3(0.9, 0, 0.9), 0, 0.5)) {
	deferredTarget = make_unique<DeferredRenderTarget>(viewportWidth, viewportHeight, multiSampling);

	hiZTarget = make_unique<HiZRenderTarget>(viewportWidth, viewportHeight, multiSampling);
    blendingTarget = make_unique<PostProcessingTarget>(viewportWidth, viewportHeight, multiSampling);

	hdrpass = make_unique<HDRPass>(viewportWidth, viewportHeight);

	glowTextureSize = ivec2(viewportWidth, viewportHeight) / 2;
	postProcessingTarget = make_unique<PostProcessingTarget>(glowTextureSize.x, glowTextureSize.y, multiSampling);

    blurTextureSize = ivec2(viewportWidth, viewportHeight) / 4;
    blurTarget = make_unique<PostProcessingTarget>(blurTextureSize.x, blurTextureSize.y, multiSampling);

	bloomTextureSize = ivec2(viewportWidth, viewportHeight) / 4;
	bloomTarget = make_unique<PostProcessingTarget>(bloomTextureSize.x, bloomTextureSize.y, multiSampling);

    blendingTarget = make_unique<PostProcessingTarget>(viewportWidth, viewportHeight, multiSampling);

	lightTarget = make_unique<PostProcessingTarget>(viewportWidth, viewportHeight, multiSampling);
	hdrTarget = make_unique<PostProcessingTarget>(viewportWidth, viewportHeight, multiSampling);
    screenSpaceReflectionTarget = make_unique<PostProcessingTarget>(viewportWidth, viewportHeight, multiSampling);
	luminanceTexture = make_unique<Texture>(ivec2(viewportWidth, viewportHeight));
	luminanceTexture2 = make_unique<Texture>(ivec2(viewportWidth, viewportHeight));
	motionBlurTarget = make_unique<PostProcessingTarget>(viewportWidth, viewportHeight, multiSampling);

	// enable back face culling
	glEnable(GL_CULL_FACE);
	// enable depth test
	glEnable(GL_DEPTH_TEST);
	// enable seamless cubemaps for mipmapping
	glEnable(GL_TEXTURE_CUBE_MAP_SEAMLESS);

	glGenBuffers(1, &uboScreenData);
	glBindBuffer(GL_UNIFORM_BUFFER, uboScreenData);
	glBufferData(GL_UNIFORM_BUFFER, 4 * 4, nullptr, GL_STATIC_DRAW); // allocate 16 bytes of memory
	glBindBuffer(GL_UNIFORM_BUFFER, 0);
	setScreenData();

	glGenBuffers(1, &uboProjectionData);
	glBindBuffer(GL_UNIFORM_BUFFER, uboProjectionData);
	glBufferData(GL_UNIFORM_BUFFER, 64 * 3 + 16, nullptr, GL_STATIC_DRAW);
	glBindBuffer(GL_UNIFORM_BUFFER, 0);

	glGenBuffers(1, &ssboLightData);
	glBindBuffer(GL_SHADER_STORAGE_BUFFER, ssboLightData);
	glBufferData(GL_SHADER_STORAGE_BUFFER, sizeof(LightData) * 1000, nullptr, GL_STATIC_DRAW);
	glBindBuffer(GL_SHADER_STORAGE_BUFFER, 0);

	glGenBuffers(1, &ssboInstanceTransforms);
	glBindBuffer(GL_SHADER_STORAGE_BUFFER, ssboInstanceTransforms);
	glBufferData(GL_SHADER_STORAGE_BUFFER, sizeof(mat4) * maxInstanceCount, nullptr, GL_STATIC_DRAW);
	glBindBuffer(GL_SHADER_STORAGE_BUFFER, 0);
}

void RenderEngine::setScreenData() {
	glBindBufferBase(GL_UNIFORM_BUFFER, 0, uboScreenData);

	glBindBuffer(GL_UNIFORM_BUFFER, uboScreenData);

	glBufferSubData(GL_UNIFORM_BUFFER, 0, 4, &viewportWidth);
	glBufferSubData(GL_UNIFORM_BUFFER, 4, 4, &viewportHeight);
	glBufferSubData(GL_UNIFORM_BUFFER, 8, 4, &multiSampling);
	glBufferSubData(GL_UNIFORM_BUFFER, 12, 4, &graphicsBrightness);

	glBindBuffer(GL_UNIFORM_BUFFER, 0);
}

void RenderEngine::setProjectionData(const mat4 projectionMatrix, const mat4 viewMatrix, const vec3 cameraPosition) {
	// Ignore scale for the view matrix. It should not influence the
	//viewMatrix = mat4(normalize(viewMatrix[0]), normalize(viewMatrix[1]), normalize(viewMatrix[2]), viewMatrix[3]);

	glBindBufferBase(GL_UNIFORM_BUFFER, 1, uboProjectionData);

	glBindBuffer(GL_UNIFORM_BUFFER, uboProjectionData);

	glm::mat4 viewProjectionMatrix = projectionMatrix * viewMatrix;

	glBufferSubData(GL_UNIFORM_BUFFER, 0, 64, value_ptr(viewProjectionMatrix));
	glBufferSubData(GL_UNIFORM_BUFFER, 64, 64, value_ptr(projectionMatrix));
	glBufferSubData(GL_UNIFORM_BUFFER, 128, 64, value_ptr(viewMatrix));
	glBufferSubData(GL_UNIFORM_BUFFER, 192, 16, value_ptr(cameraPosition));

	glBindBuffer(GL_UNIFORM_BUFFER, 0);
}

//void RenderEngine::setLightData() {
//	std::vector<std::shared_ptr<Light>> lights = Shader::activeLights;
//
//	auto lightCount = static_cast<uint32_t>(lights.size());
//	std::vector<LightData> lightData = std::vector<LightData>(lightCount);
//
//	for (int i = 0; i < lightCount; ++i) {
//		lightData[i] = lights[i]->data;
//	}
//
//	glBindBufferRange(GL_SHADER_STORAGE_BUFFER, 0, ssboLightData, 0, sizeof(LightData) * lightCount);
//
//	glBindBuffer(GL_SHADER_STORAGE_BUFFER, ssboLightData);
//	glBufferSubData(GL_SHADER_STORAGE_BUFFER, 0, sizeof(LightData) * lightCount, lightData.data());
//	glBindBuffer(GL_SHADER_STORAGE_BUFFER, 0);
//}

void RenderEngine::setViewportSize(int width, int height) {
	glViewport(0, 0, width, height);
	viewportWidth = width;
	viewportHeight = height;

	glowTextureSize = ivec2(viewportWidth, viewportHeight) / 2;
	bloomTextureSize = ivec2(viewportWidth, viewportHeight) / 4;

	setScreenData();
	deferredTarget = make_unique<DeferredRenderTarget>(viewportWidth, viewportHeight, multiSampling);

	postProcessingTarget = make_unique<PostProcessingTarget>(glowTextureSize.x, glowTextureSize.y, multiSampling);
	bloomTarget = make_unique<PostProcessingTarget>(bloomTextureSize.x, bloomTextureSize.y, multiSampling);
	blurTarget = make_unique<PostProcessingTarget>(blurTextureSize.x, blurTextureSize.y, multiSampling);
    blendingTarget = make_unique<PostProcessingTarget>(viewportWidth, viewportHeight, multiSampling);

    hiZTarget = make_unique<HiZRenderTarget>(viewportWidth, viewportHeight, multiSampling);
	screenSpaceReflectionTarget = make_unique<PostProcessingTarget>(viewportWidth, viewportHeight, multiSampling);

	lightTarget = make_unique<PostProcessingTarget>(viewportWidth, viewportHeight, multiSampling);
	hdrTarget = make_unique<PostProcessingTarget>(viewportWidth, viewportHeight, multiSampling);
	hdrpass = make_unique<HDRPass>(viewportWidth, viewportHeight);
	luminanceTexture = make_unique<Texture>(ivec2(viewportWidth, viewportHeight));
	luminanceTexture2 = make_unique<Texture>(ivec2(viewportWidth, viewportHeight));
	motionBlurTarget = make_unique<PostProcessingTarget>(viewportWidth, viewportHeight, multiSampling);
}

void RenderEngine::setMultiSampling(int sampling) {
	multiSampling = sampling;

	setScreenData();
	deferredTarget = make_unique<DeferredRenderTarget>(viewportWidth, viewportHeight, multiSampling);

	postProcessingTarget = make_unique<PostProcessingTarget>(glowTextureSize.x, glowTextureSize.y, multiSampling);
	bloomTarget = make_unique<PostProcessingTarget>(bloomTextureSize.x, bloomTextureSize.y, multiSampling);
    blurTarget = make_unique<PostProcessingTarget>(blurTextureSize.x, blurTextureSize.y, multiSampling);
    blendingTarget = make_unique<PostProcessingTarget>(viewportWidth, viewportHeight, multiSampling);

	hiZTarget = make_unique<HiZRenderTarget>(viewportWidth, viewportHeight, multiSampling);
	screenSpaceReflectionTarget = make_unique<PostProcessingTarget>(viewportWidth, viewportHeight, multiSampling);

	lightTarget = make_unique<PostProcessingTarget>(viewportWidth, viewportHeight, multiSampling);
	hdrTarget = make_unique<PostProcessingTarget>(viewportWidth, viewportHeight, multiSampling);
	hdrpass = make_unique<HDRPass>(viewportWidth, viewportHeight);
	luminanceTexture = make_unique<Texture>(ivec2(viewportWidth, viewportHeight));
	luminanceTexture2 = make_unique<Texture>(ivec2(viewportWidth, viewportHeight));
	motionBlurTarget = make_unique<PostProcessingTarget>(viewportWidth, viewportHeight, multiSampling);
}

void RenderEngine::setResolution(int width, int height, int sampling) {
	setViewportSize(width, height);
	setMultiSampling(sampling);
}

void RenderEngine::setBrightness(float brightness) {
	graphicsBrightness = brightness;
	setScreenData();
}

void RenderEngine::glowPass() {

	//TODO don't write into depth buffer
	glDisable(GL_DEPTH_TEST);
	glViewport(0, 0, postProcessingTarget->width, postProcessingTarget->height);
	//TODO set uniformbuffer opject for glowTexture-size in glowShader instead of setting it by overwriting the viewport
	int tmpViewportWidth = viewportWidth;
	int tmpViewportHeight = viewportHeight;
	viewportWidth = postProcessingTarget->width;
	viewportHeight = postProcessingTarget->height;
	setScreenData();

	/** BLUR STEPS **/
	postProcessingTarget->use();
	GLenum attachmentpoints[] = { GL_COLOR_ATTACHMENT0, GL_COLOR_ATTACHMENT1 };

	//first blur step (horizontal)
	//reading from gEmissionMetallic and writing to input (GL_COLOR_ATTACHMENT0)
	glDrawBuffer(attachmentpoints[0]);
	glowShader.useShader();
	glowShader.setHorizontalUnit(true);
	glowShader.setTextureUnit(deferredTarget->gEmissionMetallic);
	glowShader.setModelMatrix(mat4(1.0));
	drawMeshDirect(quad);

	//second blur step (vertical)
	//reading from input and writing to output (GL_COLOR_ATTACHMENT1)
	glDrawBuffer(attachmentpoints[1]);
	glowShader.useShader();
	glowShader.setHorizontalUnit(false);
	glowShader.setTextureUnit(postProcessingTarget->input);
	glowShader.setModelMatrix(mat4(1.0));
	drawMeshDirect(quad);

	//second blur step (vertical)
	//reading from input and writing to output (GL_COLOR_ATTACHMENT1)
	glDrawBuffer(attachmentpoints[0]);
	glowShader.useShader();
	glowShader.setHorizontalUnit(true);
	glowShader.setTextureUnit(postProcessingTarget->output);
	glowShader.setModelMatrix(mat4(1.0));
	drawMeshDirect(quad);

	//second blur step (vertical)
	//reading from input and writing to output (GL_COLOR_ATTACHMENT1)
	glDrawBuffer(attachmentpoints[1]);
	glowShader.useShader();
	glowShader.setHorizontalUnit(false);
	glowShader.setTextureUnit(postProcessingTarget->input);
	glowShader.setModelMatrix(mat4(1.0));
	drawMeshDirect(quad);

	//bind input
	glDrawBuffer(attachmentpoints[0]);

	/** DRAW STEP **/
	hdrTarget->use();
	glBlendFunc(GL_ONE, GL_ONE);
	glEnable(GL_BLEND);
	glBlendEquation(GL_FUNC_ADD);

	//TODO: use a blend shader instead of misusing the old debug shader
	viewportWidth = tmpViewportWidth;
	viewportHeight = tmpViewportHeight;
	setScreenData();
	glViewport(0, 0, viewportWidth, viewportHeight);
	textureDebugShader.useShader();
	textureDebugShader.setTextureUnit(postProcessingTarget->output);
	textureDebugShader.setModelMatrix(mat4(1));
	drawMeshDirect(quad);

	glDisable(GL_BLEND);
	glEnable(GL_DEPTH_TEST);
}

void RenderEngine::screenSpaceReflectionPass(){
	int levels = 1 + (int)floorf(log2f(fmaxf(viewportWidth, viewportHeight)));
	int currentWidth = viewportWidth;
	int currentHeight = viewportHeight;

	hiZTarget->use();
	deferredTarget->retrieveDepthBuffer((GLuint) hiZTarget->fbo);

	hiZTarget->use();
	glDepthMask(GL_FALSE);
	glDisable(GL_DEPTH_TEST);

	// FIRST: linearDepth values from the deferredTarget has to be written to mipmap-level 0
	hiZShader.useShader();
	hiZShader.setModelMatrix(mat4(1.0));
	glUniform1i(glGetUniformLocation(hiZShader.shaderID, "first"), true);
	glUniform1i(glGetUniformLocation(hiZShader.shaderID, "lastImage"), 12);
	glActiveTexture(GL_TEXTURE0 + 12);
	glBindTexture(GL_TEXTURE_2D, deferredTarget->gLinearDepth);

    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_BASE_LEVEL, 0);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAX_LEVEL, levels-1);
    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, hiZTarget->gLinearDepth, 0);
	drawMeshDirect(quad);

	// SECOND: generate Hi-Z mipmap-levels
	hiZShader.useShader();
	glDepthMask(GL_FALSE);
	glDisable(GL_DEPTH_TEST);
	hiZShader.setModelMatrix(mat4(1.0));
	glUniform1i(glGetUniformLocation(hiZShader.shaderID, "first"), false);
	glUniform1i(glGetUniformLocation(hiZShader.shaderID, "lastImage"), 12);
	glActiveTexture(GL_TEXTURE0 + 12);
	glBindTexture(GL_TEXTURE_2D,  hiZTarget->gLinearDepth);


    for (int i = 1; i < levels; i++) {
        //viewport must always be greater than 0 NPOT
		vec2 offset;

        currentWidth = (int)(currentWidth*0.5) > 0 ? (int)(currentWidth*0.5) : 1;
        currentHeight = (int)(currentHeight*0.5) > 0 ? (int)(currentHeight*0.5) : 1;
        glUniform2i(glGetUniformLocation(hiZShader.shaderID, "lastSize"), currentWidth, currentHeight);
        glViewport(0, 0, currentWidth, currentHeight);
		offset.x = (currentWidth  % 2 == 0 ? 1 : 2);
		offset.y = (currentHeight % 2 == 0 ? 1 : 2);


        glUniform2i(glGetUniformLocation(hiZShader.shaderID, "offset"), offset.x, offset.y);
        // lookup in shader on level i-1
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_BASE_LEVEL, i-1);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAX_LEVEL, i-1);
        // bind next level for rendering
        glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D,  hiZTarget->gLinearDepth, i);
		drawMeshDirect(quad);
    }
    // reset
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_BASE_LEVEL, 0);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAX_LEVEL, levels-1);
    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, hiZTarget->gLinearDepth, 0);
    glViewport(0, 0, viewportWidth, viewportHeight);


/*	hiZTarget->use();
	glDepthMask(GL_FALSE);
	glDisable(GL_DEPTH_TEST);*/
	screenSpaceReflectionsShader.useShader();
	screenSpaceReflectionsShader.setModelMatrix(mat4(1.0));

    glUniform1i(glGetUniformLocation(screenSpaceReflectionsShader.shaderID, "maxMipmapLevel"), levels-1);

	glUniform1i(glGetUniformLocation(screenSpaceReflectionsShader.shaderID, "gNormal"), 5);
	glActiveTexture(GL_TEXTURE0 + 5);
	glBindTexture(GL_TEXTURE_2D, deferredTarget->gNormal);

	glUniform1i(glGetUniformLocation(screenSpaceReflectionsShader.shaderID, "gAlbedoRoughness"), 2);
	glActiveTexture(GL_TEXTURE0 + 2);
	glBindTexture(GL_TEXTURE_2D, deferredTarget->gAlbedoRoughness);

	glUniform1i(glGetUniformLocation(screenSpaceReflectionsShader.shaderID, "gPosition"), 4);
	glActiveTexture(GL_TEXTURE0 + 4);
	glBindTexture(GL_TEXTURE_2D, deferredTarget->gPosition);

    glUniform1i(glGetUniformLocation(screenSpaceReflectionsShader.shaderID, "imageInput"), 3);
    glActiveTexture(GL_TEXTURE0 + 3);
    glBindTexture(GL_TEXTURE_2D, hdrTarget->output);

	glUniform1i(glGetUniformLocation(screenSpaceReflectionsShader.shaderID, "gEmissionMetallic"), 8);
	glActiveTexture(GL_TEXTURE0 + 8);
	glBindTexture(GL_TEXTURE_2D, deferredTarget->gEmissionMetallic);

	glUniform1i(glGetUniformLocation(screenSpaceReflectionsShader.shaderID, "gLinearDepth"), 9);
	glActiveTexture(GL_TEXTURE0 + 9);
	glBindTexture(GL_TEXTURE_2D, deferredTarget->gLinearDepth);

	glUniform1i(glGetUniformLocation(screenSpaceReflectionsShader.shaderID, "gHiZ"), 10);
	glActiveTexture(GL_TEXTURE0 + 10);
	glBindTexture(GL_TEXTURE_2D, hiZTarget->gLinearDepth);

	drawMeshDirect(quad);








	//TODO don't write into depth buffer
	glDisable(GL_DEPTH_TEST);
	glViewport(0, 0, blurTarget->width, blurTarget->height);
	//TODO set uniformbuffer opject for glowTexture-size in glowShader instead of setting it by overwriting the viewport
	int tmpViewportWidth = viewportWidth;
	int tmpViewportHeight = viewportHeight;
	viewportWidth = blurTarget->width;
	viewportHeight = blurTarget->height;
	setScreenData();

	/** BLUR STEPS **/
	blurTarget->use();
	GLenum attachmentpoints[] = { GL_COLOR_ATTACHMENT0, GL_COLOR_ATTACHMENT1 };

	//first blur step (horizontal)
	//reading from gEmissionMetallic and writing to input (GL_COLOR_ATTACHMENT0)
	glDrawBuffer(attachmentpoints[0]);
	glowShader.useShader();
	glowShader.setHorizontalUnit(true);
	glowShader.setTextureUnit(hiZTarget->output);
	glowShader.setModelMatrix(mat4(1.0));
	drawMeshDirect(quad);

	//second blur step (vertical)
	//reading from input and writing to output (GL_COLOR_ATTACHMENT1)
	glDrawBuffer(attachmentpoints[1]);
	glowShader.useShader();
	glowShader.setHorizontalUnit(false);
	glowShader.setTextureUnit(blurTarget->input);
	glowShader.setModelMatrix(mat4(1.0));
	drawMeshDirect(quad);

	//bind input
	glDrawBuffer(attachmentpoints[0]);

	//TODO: use a blend shader instead of misusing the old debug shader
	viewportWidth = tmpViewportWidth;
	viewportHeight = tmpViewportHeight;
	setScreenData();
	glViewport(0, 0, viewportWidth, viewportHeight);
//	textureDebugShader.useShader();
//	textureDebugShader.setTextureUnit(hdrTarget->output);
//	textureDebugShader.setModelMatrix(mat4(1));





	blendingTarget->use();
	blendingShader.useShader();
	glDisable(GL_DEPTH_TEST);

	blendingShader.setModelMatrix(mat4(1.0));

	glUniform1i(glGetUniformLocation(blendingShader.shaderID, "image"), 15);
	glActiveTexture(GL_TEXTURE0 + 15);
	glBindTexture(GL_TEXTURE_2D,  hdrTarget->output);

	glUniform1i(glGetUniformLocation(blendingShader.shaderID, "reflection"), 14);
	glActiveTexture(GL_TEXTURE0 + 14);
	glBindTexture(GL_TEXTURE_2D, blurTarget->output);

	drawMeshDirect(quad);
	glDisable(GL_BLEND);
	glEnable(GL_DEPTH_TEST);









  /*  glViewport(0, 0, postProcessingTarget->width, postProcessingTarget->height);
    //TODO set uniformbuffer opject for glowTexture-size in glowShader instead of setting it by overwriting the viewport
    int tmpViewportWidth = viewportWidth;
    int tmpViewportHeight = viewportHeight;
    viewportWidth = postProcessingTarget->width;
    viewportHeight = postProcessingTarget->height;
    setScreenData();

	postProcessingTarget->use();
    GLenum attachments[2] = {GL_COLOR_ATTACHMENT0, GL_COLOR_ATTACHMENT1};
    glDrawBuffers(2, attachments);
	//glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	glDepthMask(GL_FALSE);
	glDisable(GL_DEPTH_TEST);

    bilateralBlurShader.useShader();
  //  glDrawBuffer(attachments[0]);

	glUniform1i(glGetUniformLocation(bilateralBlurShader.shaderID, "texture"), 13);
	glActiveTexture(GL_TEXTURE0 + 13);
	glBindTexture(GL_TEXTURE_2D, hiZTarget->output);

	//vec2 viewSize = vec2(viewportWidth, viewportHeight);
	//glUniform2i(glGetUniformLocation(bilateralBlurShader.shaderID, "sketchSize"), viewSize.x, viewSize.y);

    glDepthMask(GL_FALSE);
    glDisable(GL_DEPTH_TEST);

    bilateralBlurShader.setModelMatrix(mat4(1.0));


    drawMeshDirect(quad);

   // bilateralBlurShader.useShader();
   // glDrawBuffer(attachments[1]);
   // bilateralBlurShader.setModelMatrix(mat4(1.0));
    glUniform1i(glGetUniformLocation(bilateralBlurShader.shaderID, "texture"), 13);
    glActiveTexture(GL_TEXTURE0 + 13);
    glBindTexture(GL_TEXTURE_2D, postProcessingTarget->input);
    drawMeshDirect(quad);
    viewportWidth = tmpViewportWidth;
    viewportHeight = tmpViewportHeight;
    setScreenData();
    glViewport(0, 0, viewportWidth, viewportHeight);
    //glDrawBuffer(attachments[0]);
	glEnable(GL_DEPTH_TEST);
	glDepthMask(GL_TRUE);*/
}

void RenderEngine::drawMeshDirect(const Mesh& mesh) {
	//Bind VAO
	glBindVertexArray(mesh.vao);

	glDrawElements(GL_TRIANGLES, static_cast<GLsizei>(mesh.indices.size()), GL_UNSIGNED_INT, 0);
	drawCallCount++;
}

void RenderEngine::drawMeshInstanced(const Mesh& mesh, const Material* material, vector<mat4> transforms) {
	// material-shader
	if (material == nullptr) {
		material = _defaultMaterial.get();
	}

	if (material->shader->shaderID != Shader::activeShader) {
		material->shader->useShader();
	}

	_skybox->applyToShader(material->shader);

	material->useMaterial();
	material->shader->setModelMatrix(mat4(1));

	if (wireframe) {
		glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
	} else {
		glPolygonMode(GL_FRONT_AND_BACK, material->polygonMode);
	}

	drawMeshInstancedDirect(mesh, transforms);
}

void RenderEngine::drawMesh(const Mesh& mesh, const Material* material, mat4 transform) {
	// material-shader
	if (material == nullptr) {
		material = _defaultMaterial.get();
	}

	if (material->shader->shaderID != Shader::activeShader) {
		material->shader->useShader();
	}

	_skybox->applyToShader(material->shader);

	material->shader->setModelMatrix(transform);
	material->useMaterial();

	if (wireframe) {
		glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
	} else {
		if (material != nullptr) {
			glPolygonMode(GL_FRONT_AND_BACK, material->polygonMode);
		}
	}

	drawMeshDirect(mesh);
}

void RenderEngine::drawDebugMesh(const Mesh& mesh, glm::vec3 color, mat4 transform) {
	glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);

	if (solidColorShader.shaderID != Shader::activeShader) {
		solidColorShader.useShader();
	}

	solidColorShader.setModelMatrix(transform);
	solidColorShader.setColor(color);

	glBindVertexArray(mesh.vao);
	glDrawElements(GL_LINES, static_cast<GLsizei>(mesh.indices.size()), GL_UNSIGNED_INT, 0);

	glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
}

void RenderEngine::hdrPass(float deltaTime) {
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

	hdrTarget->use();
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
	drawMeshDirect(quad);

	glEnable(GL_DEPTH_TEST);
	glDepthMask(GL_TRUE);

	// combine glow and bright areas
	combineCompute.useShader();

	glBindImageTexture(0, hdrTarget->output, 0, GL_FALSE, 0, GL_READ_ONLY, GL_RGBA16F);
	glBindImageTexture(1, luminanceTexture->textureID, 0, GL_FALSE, 0, GL_READ_ONLY, GL_RGBA16F);
	glBindImageTexture(2, deferredTarget->gEmissionMetallic, 0, GL_FALSE, 0, GL_READ_WRITE, GL_RGBA16F);

	combineCompute.compute(groups);
}

void RenderEngine::resolveFrameBufferObject() {
	//auto lastFrameBuffer = motionBlurTarget.get();
	auto lastFrameBuffer = blendingTarget.get();//blurTarget.get();//hiZTarget //screenSpaceReflectionTarget.get();


	deferredTarget->retrieveDepthBuffer((GLuint) 0);
	glBindFramebuffer(GL_FRAMEBUFFER, 0);
	glDepthMask(GL_FALSE);
	glDisable(GL_DEPTH_TEST);

	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	// enable gamma correction
	//glEnable(GL_FRAMEBUFFER_SRGB);

	textureDebugShader.useShader();
	textureDebugShader.setTextureUnit(lastFrameBuffer->output);
	textureDebugShader.setModelMatrix(mat4(1));
	drawMeshDirect(quad);

	glEnable(GL_DEPTH_TEST);
	glDepthMask(true);
	// disable gamma correction
	glDisable(GL_FRAMEBUFFER_SRGB);

	// reset draw call count
	drawCallCount = 0;
}

void RenderEngine::skyboxPass() {
	if (_skybox != nullptr) {
		glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT2, GL_TEXTURE_2D, deferredTarget->gPosition, 0);
        glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT3, GL_TEXTURE_2D, deferredTarget->gLinearDepth, 0);

		GLenum attachments[] = { GL_COLOR_ATTACHMENT0, GL_COLOR_ATTACHMENT1, GL_COLOR_ATTACHMENT2, GL_COLOR_ATTACHMENT3 };
		glDrawBuffers(4, attachments);

		_skybox->drawSkybox();

		glDrawBuffer(attachments[1]);
		glFramebufferTexture2D(GL_FRAMEBUFFER, deferredTarget->gPositionAttachment, GL_TEXTURE_2D, deferredTarget->gPosition, 0);
        glFramebufferTexture2D(GL_FRAMEBUFFER, deferredTarget->gLinearDepthAttachment, GL_TEXTURE_2D, deferredTarget->gLinearDepth, 0);

    }
}

void RenderEngine::setSkybox(Skybox* skybox) {
	_skybox = skybox;
}

void RenderEngine::drawMeshInstancedDirect(const Mesh& mesh, vector<mat4> transforms) {
	glUniform1i(glGetUniformLocation(static_cast<GLuint>(Shader::activeShader), "useInstancing"), true);
	glBindBufferBase(GL_SHADER_STORAGE_BUFFER, ssboInstanceTransformsBindingIndex, ssboInstanceTransforms);
	glNamedBufferSubData(ssboInstanceTransforms, 0, transforms.size() * sizeof(mat4), transforms.data());

	glBindVertexArray(mesh.vao);
	glDrawElementsInstanced(GL_TRIANGLES, static_cast<GLsizei>(mesh.indices.size()), GL_UNSIGNED_INT, nullptr,
							static_cast<GLsizei>(transforms.size()));
	glBindVertexArray(0);

	glUniform1i(glGetUniformLocation(static_cast<GLuint>(Shader::activeShader), "useInstancing"), false);

	drawCallCount++;
}

void RenderEngine::executeCommandBuffer(RenderCommandBuffer commandBuffer) {

	for (int i = 0; i < commandBuffer.size(); ++i) {
		std::visit([&](auto&& arg) {
		  using T = std::decay_t<decltype(arg)>;
		  if constexpr (std::is_same_v<T, DrawInstancedSSBO>) { executeCommand(arg); }
		  else if constexpr (std::is_same_v<T, DrawMesh>) { executeCommand(arg); }
		  else if constexpr (std::is_same_v<T, DrawMeshDirect>) { executeCommand(arg); }
		  else if constexpr (std::is_same_v<T, DrawInstanced>) { executeCommand(arg); }
		  else if constexpr (std::is_same_v<T, DrawInstancedDirect>) { executeCommand(arg); }
		  else if constexpr (std::is_same_v<T, NAISE::RenderCore::DrawText>) { executeCommand(arg); }
		  else if constexpr (std::is_same_v<T, DrawWireframeDirect>) { executeCommand(arg); }
		  else if constexpr (std::is_same_v<T, SetShader>) { executeCommand(arg); }
		  else if constexpr (std::is_same_v<T, BindTexture>) { executeCommand(arg); }
		  else if constexpr (std::is_same_v<T, SetViewProjectionData>) { executeCommand(arg); }
		  else if constexpr (std::is_same_v<T, SetViewport>) { executeCommand(arg); }
		  else if constexpr (std::is_same_v<T, ResetViewport>) { executeCommand(arg); }
		  else if constexpr (std::is_same_v<T, SetRenderProperty>) { executeCommand(arg); }
		  else if constexpr (std::is_same_v<T, SetBlendMode>) { executeCommand(arg); }

		  else if constexpr (std::is_same_v<T, SetRenderTarget>) { executeCommand(arg); }
		  else if constexpr (std::is_same_v<T, ClearRenderTarget>) { executeCommand(arg); }
		  else if constexpr (std::is_same_v<T, RetrieveDepthBuffer>) { executeCommand(arg); }
		  else if constexpr (std::is_same_v<T, SetClearColor>) { executeCommand(arg); }


		  // Lights
		  else if constexpr (std::is_same_v<T, RenderPointLight>) { executeCommand(arg); }
		  else if constexpr (std::is_same_v<T, RenderDirectionalLight>) { executeCommand(arg); }

		  else
			  NAISE_WARN_CONSOL("Render command not handled! ({})", typeid(arg).name())
		}, commandBuffer[i]);
	}

}

void RenderEngine::executeCommand(DrawMesh& command) {
	drawMesh(*command.mesh, command.material, command.transform);
}

void RenderEngine::executeCommand(DrawInstanced& command) {
	drawMeshInstanced(*command.mesh, command.material, command.transforms);
}

void RenderEngine::executeCommand(DrawInstancedDirect& command) {
	drawMeshInstancedDirect(*command.mesh, command.transforms);
}

void RenderEngine::executeCommand(DrawInstancedSSBO& command) {
	if (command.material->shader->shaderID != Shader::activeShader) {
		command.material->shader->useShader();
	}

	command.material->useMaterial();
	command.material->shader->setModelMatrix(command.originTransformation);

	glUniform1i(glGetUniformLocation(Shader::activeShader, "useInstancing"), true);

	glBindBufferBase(GL_SHADER_STORAGE_BUFFER, ssboInstanceTransformsBindingIndex, command.transformSSBO);
	glBindVertexArray(command.mesh->vao);
	glDrawElementsInstanced(GL_TRIANGLES, command.mesh->indices.size(), GL_UNSIGNED_INT, nullptr, command.count);
	glBindVertexArray(0);

	glUniform1i(glGetUniformLocation(Shader::activeShader, "useInstancing"), false);

	drawCallCount++;
}

void RenderEngine::executeCommand(SetRenderTarget& command) {
	command.target->use();
}

void RenderEngine::executeCommand(SetShader& command) {
	if (_activeShader == nullptr
			|| _activeShader->shaderID != command.shader->shaderID
			|| command.shader->shaderID != Shader::activeShader) {
		command.shader->useShader();
		_activeShader = command.shader;
	}
}

void RenderEngine::executeCommand(DrawMeshDirect& command) {
	drawMeshDirect(*command.mesh, command.transform);
}

void RenderEngine::executeCommand(SetViewProjectionData& command) {
	setProjectionData(command.projectionMatrix, command.viewMatrix, command.cameraPosition);
}

void RenderEngine::executeCommand(NAISE::RenderCore::DrawText& command) {
	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
	textRenderer.render(command.text, command.font, vec3(1, 1, 1), command.transform);
	glDisable(GL_BLEND);
}

void RenderEngine::executeCommand(SetRenderProperty& command) {
	switch (command.property) {
	case RenderProperty::BACKFACE_CULLING: {
		if (command.state) {
			glEnable(GL_CULL_FACE);
		} else {
			glDisable(GL_CULL_FACE);
		}

		break;
	}
	case RenderProperty::DEPTH_MASK: {
		if (command.state) {
			glDepthMask(GL_TRUE);
		} else {
			glDepthMask(GL_FALSE);
		}

		break;
	}
	case RenderProperty::DEPTH_TEST: {
		if (command.state) {
			glEnable(GL_DEPTH_TEST);
		} else {
			glDisable(GL_DEPTH_TEST);
		}
		break;
	}
	case RenderProperty::BLEND: {
		if (command.state) {
			glEnable(GL_BLEND);
		} else {
			glDisable(GL_BLEND);
		}
		break;
	}
	default:
		break;
	}
}

void RenderEngine::executeCommand(BindTexture& command) {
	command.texture->useTexture(command.slot);
	glProgramUniform1i((GLuint) command.shader->shaderID, command.location, command.slot);
}

void RenderEngine::executeCommand(SetBlendMode& command) {
	switch (command.mode) {
	case BlendMode::ADD:
		glBlendFunc(GL_ONE, GL_ONE);
		glBlendEquation(GL_FUNC_ADD);
		break;
	case BlendMode::SUBTRACT:
		glBlendFunc(GL_ONE, GL_ONE);
		glBlendEquation(GL_FUNC_SUBTRACT);
		break;
	case BlendMode::OVERLAY:
		glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
		glBlendEquation(GL_FUNC_ADD);
		break;
	default:
		break;
	}
}

void RenderEngine::executeCommand(ClearRenderTarget& command) {
	ClearBufferMask mask = GL_NONE_BIT;

	mask = (command.color) ? mask | GL_COLOR_BUFFER_BIT : mask;
	mask = (command.depth) ? mask | GL_DEPTH_BUFFER_BIT : mask;
	mask = (command.stencil) ? mask | GL_STENCIL_BUFFER_BIT : mask;
	mask = (command.accum) ? mask | GL_ACCUM_BUFFER_BIT : mask;

	glClear(mask);
}

void RenderEngine::executeCommand(RetrieveDepthBuffer& command) {
	if (command.destination == nullptr) {
		command.source->retrieveDepthBuffer((GLuint) 0);
	} else {
		command.source->retrieveDepthBuffer(command.destination);
	}
}

void RenderEngine::executeCommand(SetClearColor& command) {
	glClearColor(command.color.x, command.color.y, command.color.z, command.color.w);
}

void RenderEngine::executeCommand(ResetViewport& command) {
	glViewport(0, 0, viewportWidth, viewportHeight);
}

void RenderEngine::executeCommand(SetViewport& command) {
	glViewport(command.offset.x, command.offset.y, command.size.x, command.size.y);
}

void RenderEngine::drawMeshDirect(const Mesh& mesh, mat4 transform) {
	if(_activeShader != nullptr) {
		_activeShader->setModelMatrix(transform);
	}

	//Bind VAO
	glBindVertexArray(mesh.vao);

	glDrawElements(GL_TRIANGLES, static_cast<GLsizei>(mesh.indices.size()), GL_UNSIGNED_INT, 0);
	drawCallCount++;
}

void RenderEngine::executeCommand(DrawWireframeDirect& command) {
	drawDebugMesh(*command.mesh, command.color, command.transform);
}

void RenderEngine::executeCommand(RenderPointLight& command) {
	// stencil test for light volumes
	glEnable(GL_STENCIL_TEST);

	/* Light volume culling */
	nullShader.useShader();

	glEnable(GL_DEPTH_TEST);

	glDisable(GL_CULL_FACE);

	glClear(GL_STENCIL_BUFFER_BIT);

	// Stencil test succeed always. Only the depth test matters.
	glStencilFunc(GL_ALWAYS, 0, 0);

	glStencilOpSeparate(GL_BACK, GL_KEEP, GL_DECR_WRAP, GL_KEEP);
	glStencilOpSeparate(GL_FRONT, GL_KEEP, GL_INCR_WRAP, GL_KEEP);

	nullShader.setModelMatrix(command.transform);
	drawMeshDirect(sphereLightVolume);

	/* Light volume shading */
	plShader.useShader();
	plShader.setLightVolumeDebugging(lightVolumeDebugging);

	glStencilFunc(GL_NOTEQUAL, 0, 0xFF);

	glDisable(GL_DEPTH_TEST);

	glEnable(GL_CULL_FACE);
	glCullFace(GL_FRONT);

	deferredTarget->setTextureUnits(plShader);

	plShader.setLightProperties(*command.light);
	plShader.setModelMatrix(command.transform);
	drawMeshDirect(sphereLightVolume);

	glEnable(GL_DEPTH_TEST);

	glCullFace(GL_BACK);

	glDisable(GL_STENCIL_TEST);
}

void RenderEngine::executeCommand(RenderDirectionalLight& command) {
	// stencil test for light volumes
	glEnable(GL_STENCIL_TEST);

	dlShader.useShader();

	glDisable(GL_DEPTH_TEST);

	// ignore stencil test
	glStencilFunc(GL_ALWAYS, 0, 0);

	deferredTarget->setTextureUnits(dlShader);

	dlShader.setLightProperties(*command.light);

	for (int i = 0; i < command.cascades.size(); ++i) {
		glUniform1i(dlShader.shadowMapLocation[i], 5 + i);
		glActiveTexture(dlShader.shadowMapUnit + i);
		glBindTexture(GL_TEXTURE_2D, command.cascades[i].shadowMap->shadowMap);

		dlShader.setShadowMapViewProjection(i, command.cascades[i].projectionMatrix * command.cascades[i].viewMatrix);
		dlShader.setCascadeEnd(i, command.cameraProjectionMatrix, command.cascades[i].range.y);
	}

	drawMeshDirect(quad);

	glEnable(GL_DEPTH_TEST);

	glDisable(GL_STENCIL_TEST);
}

void RenderEngine::motionBlurPass(float deltaTime, glm::mat4 previousViewMatrix, glm::mat4 previousProjectionMatrix) {
	glDepthMask(GL_FALSE);
	glDisable(GL_DEPTH_TEST);

	motionBlurTarget->use();

	// blur target image
	motionBlurShader.useShader();

	glUniform1f(glGetUniformLocation(Shader::activeShader, "deltaTime"), deltaTime);
	glUniformMatrix4fv(glGetUniformLocation(Shader::activeShader, "previousViewMatrix"), 1, false,
					   value_ptr(previousViewMatrix));
	glUniformMatrix4fv(glGetUniformLocation(Shader::activeShader, "previousProjectionMatrix"), 1, false,
					   value_ptr(previousProjectionMatrix));

	glActiveTexture(GL_TEXTURE0 + 0);
	glBindTexture(GL_TEXTURE_2D, screenSpaceReflectionTarget->output);

	glBindImageTexture(0, deferredTarget->gPosition, 0, GL_FALSE, 0, GL_READ_ONLY, GL_RGBA32F); // current position

	motionBlurShader.setModelMatrix(mat4(1.0));
	drawMeshDirect(quad);

	glEnable(GL_DEPTH_TEST);
	glDepthMask(GL_TRUE);
}

void RenderEngine::gammaCorrection() {

}
