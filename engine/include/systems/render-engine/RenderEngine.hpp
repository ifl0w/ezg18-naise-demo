#pragma once

#include <memory>
#include <vector>
#include <glbinding/gl/gl.h>
#include <glm/glm.hpp>
#include <components/MeshComponent.hpp>
#include <components/CameraComponent.hpp>
#include <systems/render-engine/materials/PhongMaterial.hpp>
#include <components/TransformComponent.hpp>
#include <scene/Entity.hpp>
#include <Skybox.hpp>

//#include "../meshes/Mesh.h"

#include "DeferredRenderTarget.hpp"
#include "PostProcessingTarget.hpp"

#include "shaders/TextureDebugShader.hpp"
#include "shaders/PointLightShader.hpp"
#include "shaders/DirectionalLightShader.hpp"
#include "shaders/SkyboxShader.hpp"
#include "materials/shaders/SolidColorShader.hpp"
#include "meshes/Plane.hpp"
#include "meshes/Sphere.hpp"
#include "systems/render-engine/meshes/SkyboxMesh.hpp"

#include "shadow-map/ShadowMap.hpp"
#include "shadow-map/ShadowShader.hpp"
#include "shaders/GlowShader.hpp"

#include "text/TextRenderer.hpp"
#include "text/Glyph.hpp"

#include <variant>
#include <systems/particle-system/ComputeShader.hpp>

#include "RenderCommands.hpp"

#include <systems/render-engine/shadow-map/CascadedShadowMapper.hpp>
#include "post-processing/HDRPass.hpp"

using namespace gl;
using namespace NAISE::RenderCore;

namespace NAISE {
namespace Engine {

class Scene; // forward declaration to break cyclic dependency
class Object; // forward declaration to break cyclic dependency
class RenderSystem;

class RenderEngine {
	friend RenderSystem;
public:
	RenderEngine();
	RenderEngine(int viewportWidth, int viewportHeight);

	void setResolution(int width, int height, int sampling = 1);
	void setBrightness(float brightness);
	void setViewportSize(int width, int height);
	void setMultiSampling(int sampling);
	void setSkybox(Skybox* skybox);

	void skyboxPass();
	void hdrPass(float deltaTime);
	void motionBlurPass(float deltaTime, glm::mat4 previousViewMatrix, glm::mat4 previousProjectionMatrix);
	void glowPass();
	void gammaCorrection();

	void resolveFrameBufferObject();

	void executeCommandBuffer(RenderCommandBuffer commandBuffer);

	/* Draw commands */
	void executeCommand(DrawMesh& command);
	void executeCommand(DrawMeshDirect& command);
	void executeCommand(DrawInstanced& command);
	void executeCommand(DrawInstancedDirect& command);
	void executeCommand(DrawInstancedSSBO& command);
	void executeCommand(NAISE::RenderCore::DrawText& command);
	void executeCommand(DrawWireframeDirect& command);

	/* Render Target commands */
	void executeCommand(SetRenderTarget& command);
	void executeCommand(RetrieveDepthBuffer& command);
	void executeCommand(ClearRenderTarget& command);
	void executeCommand(SetClearColor& command);

	/* Shader commands */
	void executeCommand(SetShader& command);
	void executeCommand(SetRenderProperty& command);
	void executeCommand(SetBlendMode& command);
	void executeCommand(SetViewProjectionData& command);
	void executeCommand(SetViewport& command);
	void executeCommand(ResetViewport& command);

	/* Texture commands */
	void executeCommand(BindTexture& command);

	/* Lights */
	void executeCommand(RenderPointLight& command);
	void executeCommand(RenderDirectionalLight& command);

	uint8_t debugFlags = 0;
	uint32_t drawCallCount = 0;
private:
	std::unique_ptr<DeferredRenderTarget> deferredTarget;
	std::unique_ptr<PostProcessingTarget> postProcessingTarget;
	glm::ivec2 glowTextureSize;

	std::unique_ptr<PostProcessingTarget> bloomTarget;
	glm::vec2 bloomTextureSize;

	std::unique_ptr<PostProcessingTarget> combineTarget;
	std::unique_ptr<PostProcessingTarget> lightTarget;
	std::unique_ptr<PostProcessingTarget> motionBlurTarget;
	std::unique_ptr<PostProcessingTarget> gammaTarget;

	std::unique_ptr<HDRPass> hdrpass;

	std::unique_ptr<PostProcessingTarget> hdrTarget;
	ComputeShader luminanceReductionCompute = ComputeShader("engine/resources/shaders/post-processing/luminance_reduction.glsl");
	ComputeShader luminanceCompute = ComputeShader("engine/resources/shaders/post-processing/luminance.glsl");
	ComputeShader histogramCompute = ComputeShader("engine/resources/shaders/post-processing/luminance.glsl");
	ComputeShader combineCompute = ComputeShader("engine/resources/shaders/post-processing/combine.glsl");
	std::unique_ptr<Texture> luminanceTexture;
	std::unique_ptr<Texture> luminanceTexture2;

	Shader motionBlurShader = Shader("engine/resources/shaders/post-processing/quad.vert", "engine/resources/shaders/post-processing/motion-blur.frag");

	PointLightShader plShader;
	DirectionalLightShader dlShader;
	NullShader nullShader;
	TextureDebugShader textureDebugShader;
	SolidColorShader solidColorShader; // used for debugging
	GlowShader glowShader;
	Shader hdrShader = Shader("engine/resources/shaders/post-processing/quad.vert", "engine/resources/shaders/post-processing/hdr.frag");

	Sphere sphereLightVolume = Sphere(1.0f, 16, 8);

	Plane quad = Plane(2.0f, 2.0f);
	GLuint uboScreenData;
	int viewportWidth = 1024;
	int viewportHeight = 768;
	int multiSampling = 1;
	float graphicsBrightness = 1.0;

	GLuint uboProjectionData;
	GLuint ssboLightData;

	void setScreenData();
	void setProjectionData(mat4 projectionMatrix, mat4 viewMatrix, vec3 cameraPosition);
//	void setLightData();

	bool wireframe = false;
	bool backfaceCulling = true;
	bool lightVolumeDebugging = false;

	TextRenderer textRenderer;

	/* Default properties */
	unique_ptr<Material> _defaultMaterial;
	Skybox* _skybox;

	Shader* _activeShader = nullptr;

	// instancing properties
	GLuint maxInstanceCount = 100000;
	GLuint ssboInstanceTransformsBindingIndex = 0;
	GLuint ssboInstanceTransforms;

	// command functions
	void drawMeshInstanced(const Mesh& mesh, const Material* material, vector<mat4> transforms);
	void drawMeshInstancedDirect(const Mesh& mesh, vector<mat4> transforms);
	void drawMesh(const Mesh& mesh, const Material* material = nullptr, mat4 transform = mat4(1));
	void drawMeshDirect(const Mesh& mesh);
	void drawMeshDirect(const Mesh& mesh, mat4 transform);
	void drawDebugMesh(const Mesh& mesh, glm::vec3 color, mat4 transform = mat4(1));
};

}
}
