#include <systems/render-engine/Resources.hpp>

// defining tinygltf macros
#define TINYGLTF_IMPLEMENTATION
#define STB_IMAGE_IMPLEMENTATION
#define STB_IMAGE_WRITE_IMPLEMENTATION

using namespace NAISE::Engine;

std::map<std::type_index, std::shared_ptr<Shader>> Resources::shaders =
		std::map<std::type_index, std::shared_ptr<Shader>>();

//std::map<std::string, std::shared_ptr<ComputeShader>> Resources::computeShaders =
//		std::map<std::string, std::shared_ptr<ComputeShader>>();
//
//std::map<std::string, std::shared_ptr<Texture>> Resources::textures =
//		std::map<std::string, std::shared_ptr<Texture>>();
//
//std::map<pair<type_index, std::string>, std::shared_ptr<Mesh>> Resources::meshes =
//		std::map<pair<type_index, std::string>, std::shared_ptr<Mesh>>();
//
//std::map<pair<type_index, std::string>, std::shared_ptr<Material>> Resources::materials =
//		std::map<pair<type_index, std::string>, std::shared_ptr<Material>>();
//
//std::map<std::string, tinygltf::Model> Resources::models =
//		std::map<std::string, tinygltf::Model>();
//
//std::shared_ptr<Texture> Resources::loadTexture(const std::string& path) {
//	return loadTexture(path, path);
//}
//
//std::shared_ptr<Texture> Resources::loadTexture(const std::string& identifier, const std::string& path) {
//	const auto& key = identifier;
//	auto it = Resources::textures.find(key);
//
//	if (it != Resources::textures.end()) {
//		return it->second;
//	}
//
//	Resources::textures[key] = std::make_shared<ImageTexture>(ImageTexture::loadDDS(path.c_str()));
//	return Resources::textures[key];
//}
//
//std::shared_ptr<Texture> Resources::loadTexture(const tinygltf::Texture& texture, const tinygltf::Model& model) {
//	auto gltfImage = model.images[texture.source];
//
//	const auto& key = gltfImage.uri;
//	auto it = Resources::textures.find(key);
//
//	if (it != Resources::textures.end()) {
//		return it->second;
//	}
//
//	Resources::textures[key] = std::make_shared<GLTFTexture>(gltfImage);
//	return Resources::textures[key];
//}
//
//std::shared_ptr<Texture> Resources::getTexture(const std::string& identifier) {
//	const auto& key = identifier;
//	auto it = Resources::textures.find(key);
//
//	if (it != Resources::textures.end()) {
//		return it->second;
//	}
//
//	spdlog::get("logger")->warn("NAISE::ENGINE::Resources >> Texture '{}' not found.", key);
//	return std::shared_ptr<Texture>();
//}
//
//std::shared_ptr<ComputeShader> Resources::getComputeShader(const std::string& path) {
//	const auto& key = path;
//	auto it = Resources::computeShaders.find(key);
//
//	if (it != Resources::computeShaders.end()) {
//		return it->second;
//	}
//
//	Resources::computeShaders[key] = std::make_shared<ComputeShader>(path);
//	return Resources::computeShaders[key];
//}
//
//shared_ptr<Entity> Resources::loadModel(const std::string& path) {
//	const auto& key = path;
//	auto it = Resources::models.find(key);
//
//	tinygltf::TinyGLTF loader;
//	tinygltf::Model model;
//	std::string error;
//
//	std::shared_ptr<Entity> result = std::make_shared<Entity>();
//
//	if (it != Resources::models.end()) {
//		model = it->second;
//	} else {
//		bool successful = loader.LoadASCIIFromFile(&model, &error, path);
//
//		if (!successful) {
//			spdlog::get("logger")
//					->error("NAISE::ENGINE::Resources::loadModel >> Could not load ASCII model {} ({})", path, error);
//			return result;
//		}
//	}
//
//	for (const auto nodeIdx: model.scenes[model.defaultScene].nodes) {
//		shared_ptr<Entity> entity = objectFromGLTFNode(path, model.nodes[nodeIdx], model);
//		if (entity) {
//			result->addChild(entity);
//		}
//	}
//
//	for (auto& image: model.images) {
//		 remove data from memory
//		image.image.clear();
//	}
//
//	Resources::models[key] = model;
//	return result;
//}
//
//shared_ptr<Entity> Resources::objectFromGLTFNode(const std::string& idPrefix, const tinygltf::Node& node,
//												 const tinygltf::Model& model) {
//	shared_ptr<Entity> entity;
//	string id = idPrefix + "->" + node.name;
//
//	if (node.mesh >= 0) {
//		shared_ptr<Object> obj = make_shared<Object>();
//
//		/* load mesh */
//		tinygltf::Mesh mesh = model.meshes[node.mesh];
//		obj->mesh = getMesh<Mesh>(id, mesh, model);
//
//		/* load material */
//		auto gltfMaterial = model.materials[mesh.primitives[0]
//				.material]; // TODO: support for more than a single material per object
//		obj->material = getMaterial<PhongMaterial>(id, gltfMaterial, model);;
//
//		entity = obj;
//	}
//
//	for (const auto& child: node.children) {
//		entity->addChild(objectFromGLTFNode(id, model.nodes[child], model));
//	}
//
//	return entity;
//}
