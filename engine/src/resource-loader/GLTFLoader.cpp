#include <resource-loader/GLTFLoader.hpp>

#include <components/TransformComponent.hpp>
#include <components/TagComponent.hpp>
#include <components/AABBComponent.hpp>
#include <components/MaterialComponent.hpp>
#include <components/MeshComponent.hpp>
#include <components/ParentComponent.hpp>
#include <components/AnimationComponent.hpp>
#include <components/CameraComponent.hpp>

#include <glm/gtx/spline.hpp>
#include <glm/gtx/string_cast.hpp>

#include <systems/render-engine/materials/PBRMaterial.hpp>

#include "Resources.hpp"

using namespace NAISE::Engine;
using namespace std;

/* Local function declarations */

std::string INTERPOLATION_CUBIC = "CUBICSPLINE";
std::string INTERPOLATION_LINEAR = "LINEAR";
std::string INTERPOLATION_STEP = "STEP";

std::string TARGET_ROTATION = "rotation";
std::string TARGET_POSITION = "translation";
std::string TARGET_SCALE = "scale";

void getAnimationInterpolations(InterpolationFunction<vec3>& vec3Interpolation,
								InterpolationFunction<quat>& quatInterpolation,
								std::string& interpolationMode);

void generateAnimation(TransformAnimation& transformAnimation, int input, int output, const std::string& target_path,
					   const std::string& interpolationMode, const tinygltf::Model& model);

/* Cache definitions */
std::map<std::string, tinygltf::Model> GLTFLoader::models =
		std::map<std::string, tinygltf::Model>();

vector<shared_ptr<Entity>> GLTFLoader::loadModel(const std::string& path) {
	return loadModel(nullptr, path);
}

vector<shared_ptr<Entity>> GLTFLoader::loadModel(const ModelLoaderAdapter* adapter, const std::string& path) {
	vector<shared_ptr<Entity>> ret;

	const auto& key = path;
	auto it = models.find(key);

	tinygltf::TinyGLTF loader;
	tinygltf::Model model;
	std::string warn;
	std::string error;

	if (it != models.end()) {
		model = it->second;
	} else {
		bool successful = loader.LoadASCIIFromFile(&model, &error, &warn, path);

		if (!warn.empty()) {
			NAISE_WARN_LOG("Warning during loading of ASCII model {} ({})", path, warn);
		}

		if (!successful) {
			NAISE_ERROR_LOG("Could not load ASCII model {} ({})", path, error);
			return ret;
		}
	}

	for (const auto nodeIdx: model.scenes[model.defaultScene].nodes) {
		auto result = entityFromGLTFNode(adapter, path, nodeIdx, model.nodes[nodeIdx], model, nullptr);
		ret.insert(ret.end(), result.begin(), result.end());
	}

	for (auto& image: model.images) {
		// remove data from memory
		image.image.clear();
	}

	models[key] = model;
	return ret;
}

vector<shared_ptr<Entity>> GLTFLoader::entityFromGLTFNode(const ModelLoaderAdapter* adapter,
														  const std::string& idPrefix,
														  const int nodeIdx,
														  const tinygltf::Node& node,
														  const tinygltf::Model& model,
														  shared_ptr<Entity> parent) {
	vector<shared_ptr<Entity>> ret;

	auto entity = make_shared<Entity>();
	entity->add<TagComponent>(node.name);

	if (parent != nullptr) {
		entity->add<ParentComponent>(parent->id);
	}

	string id = idPrefix + "->" + node.name;

	addVisualComponents(entity, idPrefix, node, model);

	addTransformComponents(entity, node, model);

	for (auto& skin: model.skins) {
		for (auto& joint: skin.joints) {
			for (auto& child: model.nodes[joint].children) {
				if (child == nodeIdx && model.nodes[child].mesh >= 0) {
					// TODO: find elegant solution and implement vertex skinning?
//					auto test = GLTFLoader::dataFromBuffer<mat4>(skin.inverseBindMatrices, model);
//					for (auto m: test) {
//						NAISE_INFO_CONSOL("{}", glm::to_string(m))
//					}
					entity->add<TransformComponent>(); // reset
				}
			}
		}
	}

	addAnimationComponents(entity, nodeIdx, node, model, idPrefix);

	addCameraComponents(entity, node, model);

	if (adapter != nullptr) {
		bool stopEvaluation = adapter->adapt(entity, parent, node, model);

		if (stopEvaluation) {
			return ret;
		}
	}

	ret.push_back(entity);

	for (const auto& child: node.children) {
		auto children = entityFromGLTFNode(adapter, id, child, model.nodes[child], model, entity);
		ret.insert(ret.end(), children.begin(), children.end());
	}

	return ret;
}

void GLTFLoader::addTransformComponents(std::shared_ptr<Entity>& entity, const tinygltf::Node& node, const tinygltf::Model& model) {
	entity->add<TransformComponent>();

	if (!node.translation.empty()) {
		auto pos = vec3(node.translation[0], node.translation[1], node.translation[2]);
		entity->component<TransformComponent>().position = pos;
	}

	if (!node.scale.empty()) {
		auto scale = vec3(node.scale[0], node.scale[1], node.scale[2]);
		entity->component<TransformComponent>().scale = scale;
	}

	if (!node.rotation.empty()) {
		auto rot = quat(static_cast<float>(node.rotation[3]),
						static_cast<float>(node.rotation[0]),
						static_cast<float>(node.rotation[1]),
						static_cast<float>(node.rotation[2]));
		entity->component<TransformComponent>().rotation = rot;
	}

}

void GLTFLoader::addVisualComponents(std::shared_ptr<Entity>& entity, const std::string& idPrefix,
									 const tinygltf::Node& node, const tinygltf::Model& model) {
	if (node.mesh >= 0) {
		/* load mesh */
		tinygltf::Mesh mesh = model.meshes[node.mesh];
		string meshID = idPrefix + "::mesh::" + mesh.name;

		auto meshObject = Resources::getMesh<Mesh>(meshID, mesh, model);
		entity->add<MeshComponent>(meshObject);
		entity->add<AABBComponent>(AABB(meshObject->vertices));

		/* load material */
		if (mesh.primitives[0].material >= 0) {
			// TODO: support for more than a single material per object (iflow: probable not soon)
			auto gltfMaterial = model.materials[mesh.primitives[0].material];
			string matID = idPrefix + "::material::" + gltfMaterial.name;

			entity->add<MaterialComponent>();
			entity->component<MaterialComponent>().material = Resources::getMaterial<PBRMaterial>(matID, gltfMaterial,
																								  model);
		}
	}
}

void GLTFLoader::addCameraComponents(std::shared_ptr<Entity>& entity, const tinygltf::Node& node,
									 const tinygltf::Model& model) {

	if (node.camera >= 0) {
		auto camera = model.cameras[node.camera];

		if (camera.type == "orthographic") {
			NAISE_ERROR_LOG("Orthographic cameras are currently not supported.")
			return;
		}

		double fovY = camera.perspective.yfov;
		double aspectRatio = camera.perspective.aspectRatio;
		double far = camera.perspective.zfar;
		double near = camera.perspective.znear;

		entity->add<CameraComponent>(fovY, near, far, aspectRatio);
	}

}

void GLTFLoader::addAnimationComponents(std::shared_ptr<Entity>& entity, int nodeIdx, const tinygltf::Node& node,
										const tinygltf::Model& model, const std::string& idPrefix) {
	auto animComp = make_shared<TransformAnimationComponent>();

	for (const auto& animation: model.animations) {
		TransformAnimation transformAnimation;
		transformAnimation.name = animation.name;
		transformAnimation.tMax = 0;
		bool addAnimation = false;

		for (const auto& channel: animation.channels) {

			if (channel.target_node == nodeIdx) {
				addAnimation = true;

				auto sampler = animation.samplers[channel.sampler];
				int input = animation.samplers[channel.sampler].input;
				int output = animation.samplers[channel.sampler].output;

				generateAnimation(transformAnimation, input, output, channel.target_path, sampler.interpolation,
								  model);
			}
		}

		if (addAnimation) {
			animComp->animations.push_back(transformAnimation);
		}
	}

	if (!animComp->animations.empty()) {
		entity->add(animComp);
	}
}

void getAnimationInterpolations(InterpolationFunction<vec3>& vec3Interpolation,
								InterpolationFunction<quat>& quatInterpolation,
								const std::string& interpolationMode) {

	if (interpolationMode == INTERPOLATION_LINEAR) {

		vec3Interpolation = [](KeyFrame<vec3> k1, KeyFrame<vec3> k2, float factor) {
		  return glm::mix(std::get<vec3>(k1.value), std::get<vec3>(k2.value), factor);
		};

		quatInterpolation = [](KeyFrame<quat> k1, KeyFrame<quat> k2, float factor) {
		  quat v1 = std::get<quat>(k1.value);
		  quat v2 = std::get<quat>(k2.value);

		  return glm::slerp(v1, v2, factor);
		};

	} else if (interpolationMode == INTERPOLATION_CUBIC) {

		vec3Interpolation = [](KeyFrame<vec3> k1, KeyFrame<vec3> k2, float factor) {
		  auto v1 = std::get<std::tuple<vec3, vec3, vec3>>(k1.value);
		  auto v2 = std::get<std::tuple<vec3, vec3, vec3>>(k2.value);

		  return glm::hermite(std::get<1>(v1), std::get<0>(v1), std::get<1>(v2), std::get<2>(v2), factor);
		};

		quatInterpolation = [](KeyFrame<quat> k1, KeyFrame<quat> k2, float factor) {
		  std::tuple<quat, quat, quat> v1 = std::get<std::tuple<quat, quat, quat>>(k1.value);
		  std::tuple<quat, quat, quat> v2 = std::get<std::tuple<quat, quat, quat>>(k2.value);

		  return glm::hermite(std::get<1>(v1), std::get<0>(v1), std::get<1>(v2), std::get<2>(v2), factor);
		};

	} else {

		vec3Interpolation = [](KeyFrame<vec3> k1, KeyFrame<vec3> k2, float factor) {
		  return std::get<vec3>(k1.value);
		};

		quatInterpolation = [](KeyFrame<quat> k1, KeyFrame<quat> k2, float factor) {
		  return std::get<quat>(k1.value);
		};

	}
}

void generateAnimation(TransformAnimation& transformAnimation, int input, int output, const std::string& target_path,
					   const std::string& interpolationMode, const tinygltf::Model& model) {
	InterpolationFunction<glm::vec3> vec3Interpolation;
	InterpolationFunction<glm::quat> quatInterpolation;

	getAnimationInterpolations(vec3Interpolation, quatInterpolation, interpolationMode);

	if (target_path == TARGET_POSITION) {

		vector<float> inputValues = GLTFLoader::dataFromBuffer<float>(input, model);
		auto outputValues = GLTFLoader::dataFromBuffer<vec3>(output, model);

		transformAnimation.position = AnimationProperty(inputValues, outputValues);
		transformAnimation.position.interpolate = vec3Interpolation;

		auto lastT = inputValues[inputValues.size() - 1];
		if (transformAnimation.tMax < lastT) {
			transformAnimation.tMax = lastT;
		}

	} else if (target_path == TARGET_ROTATION) {

		auto inputValues = GLTFLoader::dataFromBuffer<float>(input, model);
		auto outputValues = GLTFLoader::dataFromBuffer<quat>(output, model);

		transformAnimation.rotation = AnimationProperty(inputValues, outputValues);
		transformAnimation.rotation.interpolate = quatInterpolation;

		auto lastT = inputValues[inputValues.size() - 1];
		if (transformAnimation.tMax < lastT) {
			transformAnimation.tMax = lastT;
		}

	} else if (target_path == TARGET_SCALE) {

		auto inputValues = GLTFLoader::dataFromBuffer<float>(input, model);
		auto outputValues = GLTFLoader::dataFromBuffer<vec3>(output, model);

		transformAnimation.scale = AnimationProperty(inputValues, outputValues);
		transformAnimation.scale.interpolate = vec3Interpolation;

		auto lastT = inputValues[inputValues.size() - 1];
		if (transformAnimation.tMax < lastT) {
			transformAnimation.tMax = lastT;
		}

	} else if (target_path == "weights") {
		NAISE_WARN_LOG("Vertex skinning is currently not implemented")
	}
}
