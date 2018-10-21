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
//#include "../text/Text.h"

#define BIT(x) (1<<(x))

using namespace gl;
using namespace NAISE::RenderCore;

namespace NAISE {
namespace Engine {

enum DebugState {
  DEBUG_NONE = 0,
  DEBUG_POSITION = BIT(0),
  DEBUG_NORMAL = BIT(1),
  DEBUG_ALBEDO = BIT(2),
  DEBUG_GLOW = BIT(3)
};

struct RenderCommand {};

struct DrawMesh: public RenderCommand {
  Mesh* mesh;
  Material* material;
  mat4 transform;
};

struct SetRenderTarget: public RenderCommand {
  using RenderTargetID = type_index;
  RenderTargetID renderTargetID;
};

struct SetViewProjectionData: public RenderCommand {
  mat4 viewMatrix;
  mat4 projectionMatrix;
  mat4 cameraPosition;
};

struct CommandBuffer {
  vector<RenderCommand> commands;
};

class Scene; // forward declaration to break cyclic dependency
class Object; // forward declaration to break cyclic dependency
class RenderSystem;

class RenderEngine {
	friend RenderSystem;
public:
	RenderEngine();
	RenderEngine(int viewportWidth, int viewportHeight);

	void initFrame(const CameraComponent& cameraComponent, const TransformComponent& transform);
	void render(const std::shared_ptr<Scene>& scene);

	void setResolution(int width, int height, int sampling = 1);

	void setBrightness(float brightness);
	void setViewportSize(int width, int height);
	void setMultiSampling(int sampling);
	void setSkybox(Skybox* skybox);

	void toggleBackfaceCulling();
	void toggleWireframe();
	void toggleLightVolumeDebugging();

	void skyboxPass();

	uint8_t debugFlags = 0;
	uint32_t drawCallCount = 0;
private:
	std::unique_ptr<DeferredRenderTarget> deferredTarget;
	std::unique_ptr<PostProcessingTarget> postProcessingTarget;

	std::unique_ptr<ShadowMap> shadowMap;
	ShadowShader shadowShader;
	PointLightShader plShader;
	DirectionalLightShader dlShader;
	NullShader nullShader;
	TextureDebugShader textureDebugShader;
	SolidColorShader solidColorShader; // used for debugging
	GlowShader glowShader;

	Sphere sphereLightVolume = Sphere(1.0f, 16, 8);

	Plane quad = Plane(2.0f, 2.0f);
	GLuint uboScreenData;
	int viewportWidth = 1024;
	int viewportHeight = 768;
	int multiSampling = 1;
	float graphicsBrightness = 1.0;

	void setScreenData();
	GLuint uboProjectionData;
	void setProjectionData(mat4 projectionMatrix, mat4 viewMatrix, vec3 cameraPosition);

	void setShadowProjectionData(mat4 projectionMatrix, mat4 viewMatrix, vec3 lightPosition);
	GLuint ssboLightData;

//	void setLightData();
	bool wireframe = false;
	bool backfaceCulling = true;

	bool lightVolumeDebugging = false;
	void geometryPass(const Mesh& mesh, const Material* material, mat4 transform);

	void activateShadowPass(const Entity& light, const Entity& camera);
	void shadowPass(vector<Entity*> entities);
	void deactivateShadowPass();
	void prepareLightPass();
	void lightPass(const Light& light);

	void cleanupLightPass();

	void renderLights(const Light& light, mat4 transform, const Entity& camera);
//	void forwardPass(const std::shared_ptr<Scene>& scene);
	void glowPass();
//	void textPass(const std::shared_ptr<Scene>& scene);
//	void skyboxPass(const std::shared_ptr<Scene>& scene);

//	std::shared_ptr<Text> text;

	void activateRenderState();
	void deactivateRenderState();

	void displayDebugQuads();

	glm::mat4 projectionMatrix;
	glm::mat4 viewMatrix;

	/* Default properties */
	unique_ptr<Material>  _defaultMaterial;
	Skybox* _skybox;

	// instancing properties
	GLuint maxInstanceCount = 100000;
	GLuint ssboInstanceTransformsBindingIndex = 0;
	GLuint ssboInstanceTransforms;

	// command functions
	void drawMeshInstanced(const Mesh& mesh, const Material* material, vector<mat4> transforms);
	void drawMeshInstancedDirect(const Mesh& mesh, vector<mat4> transforms);
	void drawMesh(const Mesh& mesh, const Material* material = nullptr, mat4 transform = mat4(1));
	void drawMeshDirect(const Mesh& mesh);
	void drawDebugMesh(const Mesh& mesh, glm::vec3 color);
};

}
}
