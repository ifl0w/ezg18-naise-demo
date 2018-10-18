#include <systems/render-engine/RenderEngine.hpp>
#include <systems/render-engine/materials/PBRMaterial.hpp>

#include <components/MaterialComponent.hpp>
#include <components/LightComponent.hpp>

using namespace NAISE::Engine;
using namespace std;
using namespace glm;

RenderEngine::RenderEngine()
		: RenderEngine(1024, 768) { };

RenderEngine::RenderEngine(int viewportWidth, int viewportHeight)
		: viewportWidth(viewportWidth),
		  viewportHeight(viewportHeight),
		  _defaultMaterial(make_unique<PBRMaterial>(vec3(0.9, 0, 0.9), 0, 0.5)){
	deferredTarget = make_unique<DeferredRenderTarget>(viewportWidth, viewportHeight, multiSampling);

	//TODO refactor glow, separate into own classes
	float aspectRatio = viewportWidth / viewportHeight;
	float glowTextureHeight;
	float glowTextureWidth;
	float glowTextureMaxSize = 512;
	if(viewportWidth >= viewportHeight){
		glowTextureHeight = glowTextureMaxSize / aspectRatio;
		glowTextureWidth = glowTextureMaxSize;
	} else {
		glowTextureHeight = glowTextureMaxSize;
		glowTextureWidth = glowTextureMaxSize / aspectRatio;
	}

	postProcessingTarget = make_unique<PostProcessingTarget>(glowTextureWidth, glowTextureHeight, multiSampling);

	shadowMap = make_unique<ShadowMap>(1024 * 4, 1024 * 4);

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
}

void RenderEngine::initFrame(const CameraComponent& cameraComponent, const TransformComponent& transform) {
	// enable back face culling
	glEnable(GL_CULL_FACE);
	// enable depth test
	glEnable(GL_DEPTH_TEST);
	deferredTarget->use();
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	setProjectionData(cameraComponent.getProjectionMatrix(), glm::inverse(transform.getModelMatrix()),
					  transform.position);
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

void RenderEngine::geometryPass(const Mesh& mesh, const Material* material, mat4 transform) {
//	activateRenderState();
//	for (auto const& object: scene->retrieveDeferredRenderObjects()) {
//		if (wireframe) {
//			glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
//		} else {
//			object->material->renderPolygonMode();
//		}

	drawMesh(mesh, material, transform);
//	}
//	deactivateRenderState();
}

//void RenderEngine::forwardPass(const shared_ptr<Scene>& scene) {
////	setLightData();
//
////	activateRenderState();
//	deferredTarget->retrieveDepthBuffer();
//
//	object->render();
//
//	//	deactivateRenderState();
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

void RenderEngine::lightPass(const Light& light) {
//	renderLights(light);
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

void RenderEngine::setProjectionData(const mat4 projectionMatrix, const mat4 viewMatrix, const vec3 cameraPosition) {
	glBindBufferBase(GL_UNIFORM_BUFFER, 1, uboProjectionData);

	glBindBuffer(GL_UNIFORM_BUFFER, uboProjectionData);

	glm::mat4 viewProjectionMatrix = projectionMatrix * viewMatrix;

	glBufferSubData(GL_UNIFORM_BUFFER, 0, 64, value_ptr(viewProjectionMatrix));
	glBufferSubData(GL_UNIFORM_BUFFER, 64, 64, value_ptr(projectionMatrix));
	glBufferSubData(GL_UNIFORM_BUFFER, 128, 64, value_ptr(viewMatrix));
	glBufferSubData(GL_UNIFORM_BUFFER, 192, 16, value_ptr(cameraPosition));

	glBindBuffer(GL_UNIFORM_BUFFER, 0);
}

void RenderEngine::setShadowProjectionData(const mat4 projectionMatrix, const mat4 viewMatrix,
										   const vec3 lightPosition) {
	glBindBufferBase(GL_UNIFORM_BUFFER, 1, uboProjectionData);

	glBindBuffer(GL_UNIFORM_BUFFER, uboProjectionData);

	mat4 viewProjectionMatrix = projectionMatrix * viewMatrix;

	glBufferSubData(GL_UNIFORM_BUFFER, 0, 64, value_ptr(viewProjectionMatrix));

	glBufferSubData(GL_UNIFORM_BUFFER, 64, 16, value_ptr(lightPosition));

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
		drawMeshDirect(quad);
	}

	if (debugFlags & DEBUG_NORMAL) {
		textureDebugShader.useShader();
		textureDebugShader.setMSTextureUnit(deferredTarget->gNormal);
		textureDebugShader.setModelMatrix(glm::translate(scaleMatrix, vec3(-1, -2, 1)));
		drawMeshDirect(quad);
	}

	if (debugFlags & DEBUG_ALBEDO) {
		textureDebugShader.useShader();
		textureDebugShader.setMSTextureUnit(deferredTarget->gAlbedoRoughness);
		textureDebugShader.setModelMatrix(glm::translate(scaleMatrix, vec3(1, -2, 1)));
		drawMeshDirect(quad);
	}

	if (debugFlags & DEBUG_GLOW) {
		textureDebugShader.useShader();
		textureDebugShader.setMSTextureUnit(deferredTarget->gEmissionMetallic);
		textureDebugShader.setModelMatrix(glm::translate(scaleMatrix, vec3(+3, -2, 1)));
		drawMeshDirect(quad);
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

void RenderEngine::renderLights(const Light& light, mat4 transform, const Entity& camera) {

	// stencil test for light volumes
	glEnable(GL_STENCIL_TEST);

//	for (const auto& light: lights) {
	if (!light.data.directional) {
		/* Light volume culling */
		nullShader.useShader();

		glEnable(GL_DEPTH_TEST);

		glDisable(GL_CULL_FACE);

		glClear(GL_STENCIL_BUFFER_BIT);

		// Stencil test succeed always. Only the depth test matters.
		glStencilFunc(GL_ALWAYS, 0, 0);

		glStencilOpSeparate(GL_BACK, GL_KEEP, GL_DECR_WRAP, GL_KEEP);
		glStencilOpSeparate(GL_FRONT, GL_KEEP, GL_INCR_WRAP, GL_KEEP);

		nullShader.setModelMatrix(transform);
		drawMeshDirect(sphereLightVolume);

		/* Light volume shading */
		plShader.useShader();
		plShader.setLightVolumeDebugging(lightVolumeDebugging);

		glStencilFunc(GL_NOTEQUAL, 0, 0xFF);

		glDisable(GL_DEPTH_TEST);

		glEnable(GL_CULL_FACE);
		glCullFace(GL_FRONT);

		deferredTarget->setTextureUnits(plShader);

		plShader.setLightProperties(light);
		plShader.setModelMatrix(transform);
		drawMeshDirect(sphereLightVolume);

		glEnable(GL_DEPTH_TEST);

		glCullFace(GL_BACK);
	} else {
		dlShader.useShader();

		glDisable(GL_DEPTH_TEST);

		// ignore stencil test
		glStencilFunc(GL_ALWAYS, 0, 0);

		deferredTarget->setTextureUnits(dlShader);
		shadowMap->setTextureUnits(dlShader);

		dlShader.setLightProperties(light);
		auto& c = camera.component<CameraComponent>();
		dlShader.setShadowMapViewProjection(
				light.getProjectionMatrix(AABB(c.frustum.getBoundingVolume(30))) * light.getShadowMatrix());
		drawMeshDirect(quad);

		glEnable(GL_DEPTH_TEST);
	}
//	}

	glDisable(GL_STENCIL_TEST);
}

void RenderEngine::shadowPass(const Entity& light, const Entity& camera, const vector<Entity*> entities) {
	shadowMap->use();
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	shadowShader.useShader();

	auto& l = *light.component<LightComponent>().light.get();
	auto& t = camera.component<TransformComponent>();
	auto& c = camera.component<CameraComponent>();

	setShadowProjectionData(l.getProjectionMatrix(AABB(c.frustum.getBoundingVolume(30))), l.getShadowMatrix(),
							t.position);

	// disable back face culling
	glDisable(GL_CULL_FACE);
	glViewport(0, 0, shadowMap->width, shadowMap->height);
	for (auto const& e: entities) {
		auto& mesh = *e->component<MeshComponent>().mesh.get();

		Material* material = nullptr;
		if (e->has<MaterialComponent>()) {
			material = e->component<MaterialComponent>().material.get();
		}

		auto transform = e->component<TransformComponent>().getModelMatrix();

		drawMesh(mesh, material, transform);
	}
	glViewport(0, 0, viewportWidth, viewportHeight);
	// enable back face culling
	glEnable(GL_CULL_FACE);
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
	GLenum attachmentpoints[] = {GL_COLOR_ATTACHMENT0, GL_COLOR_ATTACHMENT1};

	//first blur step (horizontal)
	//reading from gEmissionMetallic and writing to ping (GL_COLOR_ATTACHMENT0)
	glowShader.useShader();
	glowShader.setHorizontalUnit(true);
	glowShader.setTextureUnit(deferredTarget->gEmissionMetallic);
	glowShader.setModelMatrix(mat4(1.0));
	drawMeshDirect(quad);

	//second blur step (vertical)
	//reading from ping and writing to pong (GL_COLOR_ATTACHMENT1)
	glDrawBuffer(attachmentpoints[1]);
	glowShader.useShader();
	glowShader.setHorizontalUnit(false);
	glowShader.setTextureUnit(postProcessingTarget->ping);
	glowShader.setModelMatrix(mat4(1.0));
	drawMeshDirect(quad);

	//second blur step (vertical)
	//reading from ping and writing to pong (GL_COLOR_ATTACHMENT1)
	glDrawBuffer(attachmentpoints[0]);
	glowShader.useShader();
	glowShader.setHorizontalUnit(true);
	glowShader.setTextureUnit(postProcessingTarget->pong);
	glowShader.setModelMatrix(mat4(1.0));
	drawMeshDirect(quad);

	//second blur step (vertical)
	//reading from ping and writing to pong (GL_COLOR_ATTACHMENT1)
	glDrawBuffer(attachmentpoints[1]);
	glowShader.useShader();
	glowShader.setHorizontalUnit(false);
	glowShader.setTextureUnit(postProcessingTarget->ping);
	glowShader.setModelMatrix(mat4(1.0));
	drawMeshDirect(quad);

	//bind ping
	glDrawBuffer(attachmentpoints[0]);

	/** DRAW STEP **/
	glBindFramebuffer(GL_FRAMEBUFFER, 0);
	glBlendFunc(GL_ONE, GL_ONE);
	glEnable(GL_BLEND);
	glBlendEquation(GL_FUNC_ADD);

	//TODO: use a blend shader instead of misusing the old debug shader
	viewportWidth = tmpViewportWidth;
	viewportHeight = tmpViewportHeight;
	setScreenData();
	glViewport(0, 0, viewportWidth, viewportHeight);
	textureDebugShader.useShader();
	textureDebugShader.setTextureUnit(postProcessingTarget->pong);
	textureDebugShader.setModelMatrix(mat4(1));
	drawMeshDirect(quad);

	glDisable(GL_BLEND);
	glEnable(GL_DEPTH_TEST);
}
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

void RenderEngine::drawMeshDirect(const Mesh& mesh) {
	//Bind VAO
	glBindVertexArray(mesh.vao);

	glDrawElements(GL_TRIANGLES, static_cast<GLsizei>(mesh.indices.size()), GL_UNSIGNED_INT, 0);
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

void RenderEngine::drawDebugMesh(const Mesh& mesh, glm::vec3 color) {
	glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);

	if (solidColorShader.shaderID != Shader::activeShader) {
		solidColorShader.useShader();
	}

	solidColorShader.setModelMatrix(mat4(1));
	solidColorShader.setColor(color);

	glBindVertexArray(mesh.vao);
	glDrawElements(GL_LINES, static_cast<GLsizei>(mesh.indices.size()), GL_UNSIGNED_INT, 0);

}

void RenderEngine::skyboxPass() {
	if(_skybox != nullptr){
		_skybox->drawSkybox();
	}
}

void RenderEngine::setSkybox(Skybox *skybox) {
	_skybox = skybox;
}
