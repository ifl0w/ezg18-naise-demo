#include <systems/render-engine/RenderEngine.hpp>
#include <systems/render-engine/lights/LightComponent.hpp>
#include <spdlog/spdlog.h>

using namespace NAISE::Engine;
using namespace std;
using namespace glm;

RenderEngine::RenderEngine() : RenderEngine(1024, 768) {};

RenderEngine::RenderEngine(int viewportWidth, int viewportHeight)
	: viewportWidth(viewportWidth)
	, viewportHeight(viewportHeight) {
	deferredTarget = make_unique<DeferredRenderTarget>(viewportWidth, viewportHeight, multiSampling);
//	postProcessingTarget = make_unique<PostProcessingTarget>(viewportWidth, viewportHeight, multiSampling);
//	shadowMap = make_unique<ShadowMap>(1024 * 4, 1024 * 4);

	// enable back face culling
	glEnable(GL_CULL_FACE);
	// enable depth test
	glEnable(GL_DEPTH_TEST);

	glGenBuffers(1, &uboScreenData);
	glBindBuffer(GL_UNIFORM_BUFFER, uboScreenData);
	glBufferData(GL_UNIFORM_BUFFER, 4 * 4, nullptr, GL_STATIC_DRAW); // allocate 16 bytes of memory
	glBindBuffer(GL_UNIFORM_BUFFER, 0);
	setScreenData();

	glGenBuffers(1, &uboProjectionData);
	glBindBuffer(GL_UNIFORM_BUFFER, uboProjectionData);
	glBufferData(GL_UNIFORM_BUFFER, 80, nullptr, GL_STATIC_DRAW); // allocate 80 bytes of memory
	glBindBuffer(GL_UNIFORM_BUFFER, 0);

	glGenBuffers(1, &ssboLightData);
	glBindBuffer(GL_SHADER_STORAGE_BUFFER, ssboLightData);
	glBufferData(GL_SHADER_STORAGE_BUFFER, sizeof(LightData) * 1000, nullptr, GL_STATIC_DRAW);
	glBindBuffer(GL_SHADER_STORAGE_BUFFER, 0);
}

void RenderEngine::initFrame(const CameraComponent& cameraComponent, const TransformComponent& transform) {
	deferredTarget->use();
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	setProjectionData(cameraComponent, transform);
}

void RenderEngine::render(const shared_ptr<Scene>& scene) {
	/* Shadow Pass */
	// uses its own projection data
//	shadowPass(*scene.get());



	/* Geometry Pass */
//	geometryPass(scene);

	/* Lighting Pass */
//	lightPass(scene);

	/* Post-processing Passes */
//	glowPass(scene);

	/* Forward Pass */
//	forwardPass(scene);

	/* Skybox Pass*/
//	skyboxPass(scene);

	/* Text Pass */
//	textPass(scene);

	/* display continuous debug */
	if (debugFlags != DEBUG_NONE) {
		displayDebugQuads();
	}
}

void RenderEngine::geometryPass(const MeshComponent& mesh, const TransformComponent& transform, const PhongMaterialComponent& material) {
//	activateRenderState();
//	for (auto const& object: scene->retrieveDeferredRenderObjects()) {
//		if (wireframe) {
//			glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
//		} else {
//			object->material->renderPolygonMode();
//		}

	// material-shader
	if (material.shader->shaderID != Shader::activeShader) {
		material.shader->useShader();
	}
	material.shader->setModelMatrix(transform.calculateModelMatrix());
	material.useMaterial();

	if (wireframe) {
		glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
	} else {
//		object->material->renderPolygonMode();
	}

	//Bind VAO
	glBindVertexArray(mesh.vao);

	glDrawElements(GL_TRIANGLES, mesh.indices.size(), GL_UNSIGNED_INT, 0);

	//Unbind VAO
	glBindVertexArray(0);

//	}
//	deactivateRenderState();
}

//void RenderEngine::forwardPass(const shared_ptr<Scene>& scene) {
//	setLightData();
//
//	activateRenderState();
//	deferredTarget->retrieveDepthBuffer();
//	for (auto const& object: scene->retrieveForwardRenderObjects()) {
//		object->render();
//	}
//	deactivateRenderState();
//}

void RenderEngine::prepareLightPass() {
	glBindFramebuffer(GL_FRAMEBUFFER, 0);
	glClearColor(0, 0, 0, 0);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	deferredTarget->retrieveDepthBuffer();

	// do not write to the depth buffer
	glDepthMask(GL_FALSE);

	glBlendFunc(GL_ONE, GL_ONE);
	glEnable(GL_BLEND);
	glBlendEquation(GL_FUNC_ADD);
}

void RenderEngine::lightPass(const LightComponent& light) {
	renderLights(light);
}


void RenderEngine::cleanupLightPass() {
	// write to the depth buffer again
	glDepthMask(GL_TRUE);

	glDisable(GL_BLEND);
}

void RenderEngine::activateRenderState() {
	Shader::activeShader = -1;

	if (wireframe) {
		glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
	} else {

	}

	if (backfaceCulling) {
		glEnable(GL_CULL_FACE);
	}
}

void RenderEngine::deactivateRenderState() {
	glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
	glDisable(GL_CULL_FACE);
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

void RenderEngine::setProjectionData(const CameraComponent& camera, const TransformComponent& transform) {
	glBindBufferBase(GL_UNIFORM_BUFFER, 1, uboProjectionData);

	glBindBuffer(GL_UNIFORM_BUFFER, uboProjectionData);

	projectionMatrix = camera.getProjectionMatrix();
	viewMatrix = glm::inverse(transform.calculateModelMatrix());
	glm::mat4 viewProjectionMatrix = projectionMatrix * viewMatrix;

	glBufferSubData(GL_UNIFORM_BUFFER, 0, 64, value_ptr(viewProjectionMatrix));

	auto camPos = camera.getCameraPosition();
	glBufferSubData(GL_UNIFORM_BUFFER, 64, 16, value_ptr(transform.position));

	glBindBuffer(GL_UNIFORM_BUFFER, 0);
}

//void RenderEngine::setShadowProjectionData(const CameraComponent& camera, const LightComponent& light) {
//	glBindBufferBase(GL_UNIFORM_BUFFER, 1, uboProjectionData);
//
//	glBindBuffer(GL_UNIFORM_BUFFER, uboProjectionData);
//
//	mat4 viewProjectionMatrix = light.getProjectionMatrix() * light.getShadowMatrix();
//
//	glBufferSubData(GL_UNIFORM_BUFFER, 0, 64, value_ptr(viewProjectionMatrix));
//
//	auto camPos = vec3(glm::mat4(light.getOriginTransformation() * light.getModelMatrix()) * vec4(1));
//	glBufferSubData(GL_UNIFORM_BUFFER, 64, 16, value_ptr(camPos));
//
//	glBindBuffer(GL_UNIFORM_BUFFER, 0);
//}

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

void RenderEngine::toggleBackfaceCulling() {
	if (backfaceCulling) {
		spdlog::get("logger")->info("NAISE::ENGINE :: backface culling disabled");
	} else {
		spdlog::get("logger")->info("NAISE::ENGINE :: backface culling enabled");
	}

	backfaceCulling = !backfaceCulling;
}

void RenderEngine::toggleWireframe() {
	if (wireframe) {
		spdlog::get("logger")->info("NAISE::ENGINE :: wireframe rendering disabled");
	} else {
		spdlog::get("logger")->info("NAISE::ENGINE :: wireframe rendering enabled");
	}

	wireframe = !wireframe;
}

void RenderEngine::displayDebugQuads() {
	glDisable(GL_DEPTH_TEST);

	float scale = 1 / 4.0f;
	glm::mat4 scaleMatrix = glm::scale(mat4(), vec3(scale, scale, 1));

	if (debugFlags & DEBUG_POSITION) {
		textureDebugShader.useShader();
		textureDebugShader.setMSTextureUnit(deferredTarget->gPosition);
		textureDebugShader.setModelMatrix(glm::translate(scaleMatrix, vec3(-3, -2, 1)));
		quad.draw();
	}

	if (debugFlags & DEBUG_NORMAL) {
		textureDebugShader.useShader();
		textureDebugShader.setMSTextureUnit(deferredTarget->gNormal);
		textureDebugShader.setModelMatrix(glm::translate(scaleMatrix, vec3(-1, -2, 1)));
		quad.draw();
	}

	if (debugFlags & DEBUG_ALBEDO) {
		textureDebugShader.useShader();
		textureDebugShader.setMSTextureUnit(deferredTarget->gAlbedoSpec);
		textureDebugShader.setModelMatrix(glm::translate(scaleMatrix, vec3(1, -2, 1)));
		quad.draw();
	}

	if (debugFlags & DEBUG_GLOW) {
		textureDebugShader.useShader();
		textureDebugShader.setMSTextureUnit(deferredTarget->gGlow);
		textureDebugShader.setModelMatrix(glm::translate(scaleMatrix, vec3(+3, -2, 1)));
		quad.draw();
	}

	glEnable(GL_DEPTH_TEST);
}

void RenderEngine::setViewportSize(int width, int height) {
	glViewport(0, 0, width, height);
	viewportWidth = width;
	viewportHeight = height;
	setScreenData();
	deferredTarget = make_unique<DeferredRenderTarget>(viewportWidth, viewportHeight, multiSampling);
//	postProcessingTarget = make_unique<PostProcessingTarget>(viewportWidth, viewportHeight, multiSampling);
}

void RenderEngine::setMultiSampling(int sampling) {
	multiSampling = sampling;
	setScreenData();
	deferredTarget = make_unique<DeferredRenderTarget>(viewportWidth, viewportHeight, multiSampling);
//	postProcessingTarget = make_unique<PostProcessingTarget>(viewportWidth, viewportHeight, multiSampling);
}

void RenderEngine::setResolution(int width, int height, int sampling) {
	setViewportSize(width, height);
	setMultiSampling(sampling);
}

void RenderEngine::setBrightness(float brightness) {
	graphicsBrightness = brightness;
	setScreenData();
}

void RenderEngine::renderLights(const LightComponent& light) {

	// stencil test for light volumes
	glEnable(GL_STENCIL_TEST);

//	for (const auto& light: lights) {
		if (!light.data.directional) {
//			/* Light volume culling */
//			nullShader.useShader();
//
//			glEnable(GL_DEPTH_TEST);
//
//			glDisable(GL_CULL_FACE);
//
//			glClear(GL_STENCIL_BUFFER_BIT);
//
//			// Stencil test succeed always. Only the depth test matters.
//			glStencilFunc(GL_ALWAYS, 0, 0);
//
//			glStencilOpSeparate(GL_BACK, GL_KEEP, GL_DECR_WRAP, GL_KEEP);
//			glStencilOpSeparate(GL_FRONT, GL_KEEP, GL_INCR_WRAP, GL_KEEP);
//
//			nullShader.setModelMatrix(glm::mat4(light->getOriginTransformation() * light->getModelMatrix()));
//			sphereLightVolume.draw();
//
//			/* Light volume shading */
//			plShader.useShader();
//			plShader.setLightVolumeDebugging(lightVolumeDebugging);
//
//			glStencilFunc(GL_NOTEQUAL, 0, 0xFF);
//
//			glDisable(GL_DEPTH_TEST);
//
//			glEnable(GL_CULL_FACE);
//			glCullFace(GL_FRONT);
//
//			deferredTarget->setTextureUnits(plShader);
//
//			plShader.setLightProperties(light);
//			plShader.setModelMatrix(glm::mat4(light->getOriginTransformation() * light->getModelMatrix()));
//			sphereLightVolume.draw();
//
//			glEnable(GL_DEPTH_TEST);
//
//			glCullFace(GL_BACK);
		} else {
			dlShader.useShader();

			glDisable(GL_DEPTH_TEST);

			// ignore stencil test
			glStencilFunc(GL_ALWAYS, 0, 0);

			deferredTarget->setTextureUnits(dlShader);
//			shadowMap->setTextureUnits(dlShader);

			dlShader.setLightProperties(light);
			dlShader.setShadowMapViewProjection(light.getProjectionMatrix() * light.getShadowMatrix());
			quad.draw();

			glEnable(GL_DEPTH_TEST);
		}
//	}

	glDisable(GL_STENCIL_TEST);
}

//void RenderEngine::shadowPass(const Scene& scene) {
//	shadowMap->use();
//	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
//	shadowShader.useShader();
//
//	setShadowProjectionData(*Shader::activeCamera.get(), *Shader::activeSun.get());
//
//	glViewport(0, 0, shadowMap->width, shadowMap->height);
//	for (auto const& object: scene.retrieveShadowRenderObjects()) {
//		if (object->material->renderShadowMode()) {
//			object->render();
//		}
//	}
//	glViewport(0, 0, viewportWidth, viewportHeight);
//}
//
//void RenderEngine::glowPass(const std::shared_ptr<Scene>& scene) {
//
//	glDisable(GL_DEPTH_TEST);
//
//	float scale = 1.0f;
//	glm::mat4 scaleMatrix = glm::scale(mat4(), vec3(scale, scale, 1));
//
//	/** BLUR STEPS **/
//	postProcessingTarget->use();
//	GLenum attachmentpoints[] = {GL_COLOR_ATTACHMENT0, GL_COLOR_ATTACHMENT1};
//
//	 first blur step (horizontal)
//	 reading from gGlow and writing to ping (GL_COLOR_ATTACHMENT0)
//	glowShader.useShader();
//	glowShader.setHorizontalUnit(true);
//	glowShader.setMSTextureUnit(deferredTarget->gGlow);
//	glowShader.setModelMatrix(glm::translate(scaleMatrix, vec3(0, 0, 0)));
//	quad.draw();
//
//	 second blur step (vertical)
//	 reading from ping and writing to pong (GL_COLOR_ATTACHMENT1)
//	glDrawBuffer(attachmentpoints[1]);
//	glowShader.useShader();
//	glowShader.setHorizontalUnit(false);
//	glowShader.setMSTextureUnit(postProcessingTarget->ping);
//	glowShader.setModelMatrix(glm::translate(scaleMatrix, vec3(0, 0, 0)));
//	quad.draw();
//
///*		// third blur step (horizontal)
//		 reading from gGlow and writing to ping (GL_COLOR_ATTACHMENT0)
//		glDrawBuffer (attachmentpoints[0]);
//		glowShader.useShader();
//		glowShader.setHorizontalUnit(true);
//		glowShader.setMSTextureUnit(postProcessingTarget->pong);
//		glowShader.setModelMatrix(glm::translate(scaleMatrix, vec3(0, 0,0)));
//		quad.draw();
//
//		 fourth blur step (vertical)
//		 reading from ping and writing to pong (GL_COLOR_ATTACHMENT1)
//		glDrawBuffer (attachmentpoints[1]);
//		glowShader.useShader();
//		glowShader.setHorizontalUnit(false);
//		glowShader.setMSTextureUnit(postProcessingTarget->ping);
//		glowShader.setModelMatrix(glm::translate(scaleMatrix, vec3(0, 0,0)));
//		quad.draw();*/
//
//	 bind ping
//	glDrawBuffer(attachmentpoints[0]);
//
//	/** DRAW STEP **/
//	glBindFramebuffer(GL_FRAMEBUFFER, 0);
//	glBlendFunc(GL_ONE, GL_ONE);
//	glEnable(GL_BLEND);
//	glBlendEquation(GL_FUNC_ADD);
//
//	textureDebugShader.useShader();
//	textureDebugShader.setMSTextureUnit(postProcessingTarget->pong);
//	textureDebugShader.setModelMatrix(glm::translate(scaleMatrix, vec3(0, 0, 0)));
//	quad.draw();
//
//	glDisable(GL_BLEND);
//	glEnable(GL_DEPTH_TEST);
//}
//
//void RenderEngine::textPass(const std::shared_ptr<Scene>& scene) {
//
//	if(!scene->retrieveTextElements().empty()){
//		glBindFramebuffer(GL_FRAMEBUFFER, 0);
//
//		glEnable(GL_BLEND);
//		glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
//		glDisable(GL_DEPTH_TEST);
//
//		for (auto const& t: scene->retrieveTextElements()) {
//			t->renderText();
//		}
//
//		quad.draw();
//
//		glDisable(GL_BLEND);
//		glEnable(GL_DEPTH_TEST);
//	}
//
//}
//
//void RenderEngine::skyboxPass(const std::shared_ptr<Scene>& scene) {
//
//	if(scene->skybox){
//		glm::mat4 matrix = projectionMatrix * glm::mat4(glm::mat3(viewMatrix)) ;
//		scene->skybox->useSkybox(matrix);
//	}
//
//}

void RenderEngine::toggleLightVolumeDebugging() {
	if (lightVolumeDebugging) {
		spdlog::get("logger")->info("NAISE::ENGINE :: Light volume debugging disabled");
	} else {
		spdlog::get("logger")->info("NAISE::ENGINE :: Light volume debugging enabled");
	}

	lightVolumeDebugging = !lightVolumeDebugging;
}
