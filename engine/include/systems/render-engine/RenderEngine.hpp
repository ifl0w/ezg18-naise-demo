#pragma once

#include <memory>
#include <vector>
#include <glbinding/gl/gl.h>
#include <glm/glm.hpp>
#include <components/MeshComponent.hpp>
#include <components/CameraComponent.hpp>
#include <systems/render-engine/materials/PhongMaterialComponent.hpp>
#include <components/TransformComponent.hpp>

//#include "../meshes/Mesh.h"

#include "DeferredRenderTarget.hpp"

#include "shaders/TextureDebugShader.hpp"
//#include "shaders/PointLightShader.hpp"
#include "shaders/DirectionalLightShader.hpp"
#include "meshes/Plane.hpp"
#include "meshes/Sphere.hpp"

//#include "shadow-map/ShadowMap.hpp"
//#include "shadow-map/ShadowShader.hpp"
//#include "shaders/glow_shader/GlowShader.hpp"
//#include "PostProcessingTarget.hpp"
//#include "../text/Text.h"

#define BIT(x) (1<<(x))

using namespace gl;

namespace NAISE {
namespace Engine {

enum DebugState {
  DEBUG_NONE = 0,
  DEBUG_POSITION = BIT(0),
  DEBUG_NORMAL = BIT(1),
  DEBUG_ALBEDO = BIT(2),
  DEBUG_GLOW = BIT(3)
};

class Scene; // forward declaration to break cyclic dependency
class Object; // forward declaration to break cyclic dependency
class RenderSystem;

class RenderEngine {
	friend RenderSystem;
public:
	RenderEngine();
	RenderEngine(int viewportWidth, int viewportHeight);

	void initFrame(const CameraComponent& cameraComponent);
	void render(const std::shared_ptr<Scene>& scene);

	void setResolution(int width, int height, int sampling = 1);

	void setBrightness(float brightness);
	void setViewportSize(int width, int height);
	void setMultiSampling(int sampling);

	void toggleBackfaceCulling();
	void toggleWireframe();
	void toggleLightVolumeDebugging();

	uint8_t debugFlags = 0;

private:
	std::unique_ptr<DeferredRenderTarget> deferredTarget;
//	std::unique_ptr<PostProcessingTarget> postProcessingTarget;
//	std::unique_ptr<ShadowMap> shadowMap;

//	PointLightShader plShader;
//	ShadowShader shadowShader;
	DirectionalLightShader dlShader;
	NullShader nullShader;
	TextureDebugShader textureDebugShader;
//	GlowShader glowShader;

	Sphere sphereLightVolume = Sphere(1.0f, 16, 8);
	Plane quad = Plane(2.0f, 2.0f);

	GLuint uboScreenData;
	int viewportWidth = 1024;
	int viewportHeight = 768;
	int multiSampling = 1;
	float graphicsBrightness = 1.0;
	void setScreenData();

	GLuint uboProjectionData;
	void setProjectionData(const CameraComponent& camera);
	void setShadowProjectionData(const CameraComponent& camera, const LightComponent& light);

	GLuint ssboLightData;
//	void setLightData();

	bool wireframe = false;
	bool backfaceCulling = true;
	bool lightVolumeDebugging = false;

	void geometryPass(const MeshComponent& mesh, const TransformComponent& transform, const PhongMaterialComponent& materialComponent);
//	void shadowPass(const Scene& scene);

	void prepareLightPass();
	void lightPass(const LightComponent& light);
	void cleanupLightPass();

	void renderLights(const LightComponent& light);
//	void forwardPass(const std::shared_ptr<Scene>& scene);
//	void glowPass(const std::shared_ptr<Scene>& scene);
//	void textPass(const std::shared_ptr<Scene>& scene);
//	void skyboxPass(const std::shared_ptr<Scene>& scene);

//	std::shared_ptr<Text> text;

	void activateRenderState();
	void deactivateRenderState();

	void displayDebugQuads();

	glm::mat4 projectionMatrix;
	glm::mat4 viewMatrix;
};

}
}
