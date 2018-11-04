#include <resource-loader/GLTFLoader.hpp>

#include <components/TransformComponent.hpp>
#include <components/TagComponent.hpp>
#include <components/AABBComponent.hpp>
#include <components/MaterialComponent.hpp>
#include <components/MeshComponent.hpp>
#include <components/ParentComponent.hpp>
#include <components/AnimationComponent.hpp>

#include <systems/render-engine/materials/PBRMaterial.hpp>

#include "Resources.hpp"

using namespace NAISE::Engine;
using namespace std;

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
	entity->add<TransformComponent>();
	entity->add<TagComponent>(node.name);

	string id = idPrefix + "->" + node.name;

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

	if (!node.translation.empty()) {
		auto pos = vec3(node.translation[0], node.translation[1], node.translation[2]);
		entity->component<TransformComponent>().position = pos;
	}

	if (!node.scale.empty()) {
		auto scale = vec3(node.scale[0], node.scale[1], node.scale[2]);
		entity->component<TransformComponent>().scale = scale;
	}

	if (!node.rotation.empty()) {
		auto rot = quat(node.rotation[3], node.rotation[0], node.rotation[1], node.rotation[2]);
		entity->component<TransformComponent>().rotation = rot;
	}

	if (adapter != nullptr) {
		bool stopEvaluation = adapter->adapt(entity, parent, node, model);

		if (stopEvaluation) {
			return ret;
		}
	}

	ret.push_back(entity);

	loadAnimations(entity, nodeIdx, node, model, idPrefix);

	if (parent != nullptr) {
		entity->add<ParentComponent>(parent->id);
	}

	for (const auto& child: node.children) {
		auto children = entityFromGLTFNode(adapter, id, child, model.nodes[child], model, entity);
		ret.insert(ret.end(), children.begin(), children.end());
	}

	return ret;
}

void GLTFLoader::loadAnimations(std::shared_ptr<Entity>& entity, const int nodeIdx, const tinygltf::Node& node,
								const tinygltf::Model& model, const std::string& idPrefix) {
	auto animComp = make_shared<TransformAnimationComponent>();

	for (const auto& animation: model.animations) {
		TransformAnimation transformAnimation;
		transformAnimation.name = animation.name;
		transformAnimation.tMax = 0;
		bool addAnimation = false;

		for (const auto& channel: animation.channels) {

			if (channel.target_node == nodeIdx) {
				auto sampler = animation.samplers[channel.sampler];
				auto interpolation = sampler.interpolation;

				if (interpolation != "LINEAR") {
					NAISE_WARN_LOG("Requested interpolation is currently not supported! ({})", interpolation)
					continue;
				}

				addAnimation = true;

				int input = animation.samplers[channel.sampler].input;
				int output = animation.samplers[channel.sampler].output;

				if (channel.target_path == "translation") {
					vector<float> inputValues = dataFromBuffer<float>(input, model);
					vector<glm::vec3> outputValues = dataFromBuffer<glm::vec3>(output, model);

					assert(inputValues.size() == outputValues.size());
					for (size_t i = 0; i < inputValues.size(); ++i) {
						transformAnimation.position[inputValues[i]] = outputValues[i];
					}

					auto lastT = inputValues[inputValues.size() - 1];
					if (transformAnimation.tMax < lastT) {
						transformAnimation.tMax = lastT;
					}
				} else if (channel.target_path == "rotation") {
					vector<float> inputValues = dataFromBuffer<float>(input, model);
					vector<glm::quat> outputValues = dataFromBuffer<glm::quat>(output, model);

					assert(inputValues.size() == outputValues.size());
					for (size_t i = 0; i < inputValues.size(); ++i) {
						transformAnimation.rotation[inputValues[i]] = outputValues[i];
					}

					auto lastT = inputValues[inputValues.size() - 1];
					if (transformAnimation.tMax < lastT) {
						transformAnimation.tMax = lastT;
					}
				} else if (channel.target_path == "scale") {
					vector<float> inputValues = dataFromBuffer<float>(input, model);
					vector<glm::vec3> outputValues = dataFromBuffer<glm::vec3>(output, model);

					assert(inputValues.size() == outputValues.size());
					for (size_t i = 0; i < inputValues.size(); ++i) {
						transformAnimation.scale[inputValues[i]] = outputValues[i];
					}

					auto lastT = inputValues[inputValues.size() - 1];
					if (transformAnimation.tMax < lastT) {
						transformAnimation.tMax = lastT;
					}
				} else if (channel.target_path == "weights") {
					NAISE_WARN_LOG("Vertex skinning is currently not implemented")
				}
			}
		}

		if(addAnimation) {
			animComp->animations.push_back(transformAnimation);
		}
	}

	if (!animComp->animations.empty()) {
		entity->add(animComp);
	}
}
