#include <Resources.hpp>

#include <systems/render-engine/textures/ImageTexture.hpp>
#include <systems/render-engine/textures/GLTFTexture.hpp>

#include <components/MaterialComponent.hpp>
#include <components/MeshComponent.hpp>
#include <systems/render-engine/materials/PBRMaterial.hpp>

#define STB_IMAGE_IMPLEMENTATION
#include <stb_image.h>
#undef STB_IMAGE_IMPLEMENTATION

// defining tinygltf macros
#define TINYGLTF_IMPLEMENTATION
#define STB_IMAGE_WRITE_IMPLEMENTATION
#include <tiny_gltf.h>

#include <components/TransformComponent.hpp>

#include <systems/render-engine/textures/SkyboxTexture.hpp>
#include <Utils.hpp>

using namespace NAISE::Engine;

std::map<std::type_index, std::shared_ptr<Shader>> Resources::shaders =
		std::map<std::type_index, std::shared_ptr<Shader>>();

//std::map<std::string, std::shared_ptr<ComputeShader>> Resources::computeShaders =
//		std::map<std::string, std::shared_ptr<ComputeShader>>();

std::map<std::string, std::shared_ptr<Texture>> Resources::textures =
		std::map<std::string, std::shared_ptr<Texture>>();

std::map<pair<type_index, std::string>, std::shared_ptr<Mesh>> Resources::meshes =
		std::map<pair<type_index, std::string>, std::shared_ptr<Mesh>>();

std::map<pair<type_index, std::string>, std::shared_ptr<Material>> Resources::materials =
		std::map<pair<type_index, std::string>, std::shared_ptr<Material>>();

std::map<std::string, tinygltf::Model> Resources::models =
		std::map<std::string, tinygltf::Model>();

std::shared_ptr<Texture> Resources::loadTexture(const std::string& path) {
	return loadTexture(path, path);
}

std::shared_ptr<Texture> Resources::loadTexture(const std::string& identifier, const std::string& path) {
	const auto& key = identifier;
	auto it = Resources::textures.find(key);

	if (it != Resources::textures.end()) {
		return it->second;
	}

	Resources::textures[key] = std::make_shared<ImageTexture>(ImageTexture::loadDDS(path.c_str()));
	return Resources::textures[key];
}

std::shared_ptr<Texture> Resources::loadSkyboxTexture(const std::string& identifier,
													  const std::vector<std::string> paths) {
	assert(paths.size() == 6);

	const auto& key = identifier;
	auto it = Resources::textures.find(key);

	if (it != Resources::textures.end()) {
		return it->second;
	}

	std::vector<SkyboxImageData> skyboxImages(paths.size());

	int width, height, nrChannels;
	for (unsigned int i = 0; i < paths.size(); i++)
	{
		SkyboxImageData data;
		data.data = stbi_load(paths[i].c_str(), &width, &height, &nrChannels, 0);
		data.width = width;
		data.height = height;
		data.nrChannels = nrChannels;

		if (data.data) {
			skyboxImages.push_back(data);
		} else {
			NAISE_ERROR_PERSISTENCE("Resources::Skybox texture failed to load at path: {}", paths[i]);
			//spdlog::get("logger")->error("Resources::Skybox texture failed to load at path: {}", paths[i]);
		}
	}

	Resources::textures[key] = std::make_shared<SkyboxTexture>(skyboxImages);
	for (auto& d: skyboxImages){
		stbi_image_free(d.data);
	}
	return Resources::textures[key];
}

std::shared_ptr<Texture> Resources::loadTexture(const tinygltf::Texture& texture, const tinygltf::Model& model) {
	auto gltfImage = model.images[texture.source];

	const auto& key = gltfImage.uri;
	auto it = Resources::textures.find(key);

	if (it != Resources::textures.end()) {
		return it->second;
	}

	Resources::textures[key] = std::make_shared<GLTFTexture>(gltfImage);
	return Resources::textures[key];
}

std::shared_ptr<Texture> Resources::getTexture(const std::string& identifier) {
	const auto& key = identifier;
	auto it = Resources::textures.find(key);

	if (it != Resources::textures.end()) {
		return it->second;
	}

	spdlog::get("logger")->warn("NAISE::ENGINE::Resources >> Texture '{}' not found.", key);
	return std::shared_ptr<Texture>();
}

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

vector<shared_ptr<Entity>> Resources::loadModel(const std::string& path) {
	vector<shared_ptr<Entity>> ret;

	const auto& key = path;
	auto it = Resources::models.find(key);

	tinygltf::TinyGLTF loader;
	tinygltf::Model model;
	std::string warn;
	std::string error;

	if (it != Resources::models.end()) {
		model = it->second;
	} else {
		bool successful = loader.LoadASCIIFromFile(&model, &error, &warn, path);

		if (!warn.empty()) {
			spdlog::get("logger")
					->warn("Resources::loadModel >> Warning during loading of ASCII model {} ({})", path, warn);
		}

		if (!successful) {
			spdlog::get("logger")->error("Resources::loadModel >> Could not load ASCII model {} ({})", path, error);
			return ret;
		}
	}

	for (const auto nodeIdx: model.scenes[model.defaultScene].nodes) {
		auto result = entityFromGLTFNode(path, model.nodes[nodeIdx], model);
		ret.insert(ret.end(), result.begin(), result.end());
	}

	for (auto& image: model.images) {
		// remove data from memory
		image.image.clear();
	}

	Resources::models[key] = model;
	return ret;
}

vector<shared_ptr<Entity>> Resources::entityFromGLTFNode(const std::string& idPrefix, const tinygltf::Node& node,
												 const tinygltf::Model& model) {
	vector<shared_ptr<Entity>> ret;

	auto entity = make_shared<Entity>();
	entity->add<TransformComponent>();

	string id = idPrefix + "->" + node.name;

	if (node.mesh >= 0) {
		/* load mesh */
		tinygltf::Mesh mesh = model.meshes[node.mesh];
		entity->add<MeshComponent>();
		entity->component<MeshComponent>().mesh = getMesh<Mesh>(id, mesh, model);

		if (node.translation.size() > 0) {
			auto pos = vec3(node.translation[0], node.translation[1], node.translation[3]);
			entity->component<TransformComponent>().position = pos;
		}

		/* load material */
		if (mesh.primitives[0].material >= 0) {
			// TODO: support for more than a single material per object (iflow: probable not soon)
			auto gltfMaterial = model.materials[mesh.primitives[0].material];
			entity->add<MaterialComponent>();
			entity->component<MaterialComponent>().material = getMaterial<PBRMaterial>(id, gltfMaterial, model);
		}

		ret.push_back(entity);
	}

	for (const auto& child: node.children) {
		auto children = entityFromGLTFNode(id, model.nodes[child], model);
		ret.insert(ret.end(), children.begin(), children.end());
	}

	return ret;
}

void Resources::freeAll() {
	shaders.clear();
	textures.clear();
	materials.clear();
	meshes.clear();
	models.clear();
}
