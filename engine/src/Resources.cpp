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
#include <components/ParentComponent.hpp>
#include <components/TagComponent.hpp>

#include <systems/render-engine/textures/SkyboxTexture.hpp>
#include <Logger.hpp>

using namespace NAISE::Engine;

struct Image {
	ImageFileType imageFileType;
	std::string magic_mnemonic;
	int magic_mnemonicBytes;
	std::string magic_signature;
	int magic_signaturBytes;
};

//source: https://en.wikipedia.org/wiki/List_of_file_signatures || https://blog.netspi.com/magic-bytes-identifying-common-file-formats-at-a-glance/
//TODO test filetypes
struct Image JPG {IMAGE_FILE_JPG, "ÿØÿÛ", 8, "\xFF\xD8\xFF", 3};
struct Image PNG {IMAGE_FILE_PNG, "\x89PNG", 4, "\x89\x50\x4E\x47\x0D\x0A\x1A\x0A", 8}; //"\x89\x50\x4E\x47\x0D\x0A\x1A\x0A""\x50\x89\x47\x4E\x0A\x0D\x0A\x1A"
struct Image DDS {IMAGE_FILE_DDS, "DDS", 3, "\x44\x44\x53\x20", 4};
struct Image BMP {IMAGE_FILE_BMP, "BM", 2, "\x42\x4D", 2};

struct Image GIF {IMAGE_FILE_GIF, "GIF8", 4, "\x47\x49\x46\x38", 6};
struct Image PSD {IMAGE_FILE_PSD, "8BPS", 4, "\x38\x42\x50\x53", 4};
/*struct Image HDR {IMAGE_FILE_HDR, };
struct Image PIC {IMAGE_FILE_PIC, };
struct Image TGA {IMAGE_FILE_TGA, };*/

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

	ImageFileType imageFileType = getImageTypeByMagic(path);
	switch (imageFileType){
		case(IMAGE_FILE_INVALID):
			break;
		case(IMAGE_FILE_JPG):
			Resources::textures[key] = loadWithSTB(path);
			break;
		case(IMAGE_FILE_PNG):
			Resources::textures[key] = loadWithSTB(path);
			break;
		case(IMAGE_FILE_BMP):
			Resources::textures[key] = loadWithSTB(path);
			break;
		case(IMAGE_FILE_TGA):
			Resources::textures[key] = loadWithSTB(path);
			break;
		case(IMAGE_FILE_PSD):
			Resources::textures[key] = loadWithSTB(path);
			break;
		case(IMAGE_FILE_GIF):
			Resources::textures[key] = loadWithSTB(path);
			break;
		case(IMAGE_FILE_HDR):
			Resources::textures[key] = loadWithSTB(path);
			break;
		case(IMAGE_FILE_PIC):
			Resources::textures[key] = loadWithSTB(path);
			break;
		case(IMAGE_FILE_DDS):
			//TODO function should move to resources
			Resources::textures[key] = std::make_shared<ImageTexture>(ImageTexture::loadDDS(path.c_str()));
			break;
			//TODO add other formatloader
	}

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

	std::vector<SkyboxImageData> skyboxImages;

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
			NAISE_ERROR_LOG("Resources::Skybox texture failed to load at path: {}", paths[i]);
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

	NAISE_WARN_LOG("Texture '{}' not found.", key);
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
			NAISE_WARN_LOG("Warning during loading of ASCII model {} ({})", path, warn);
		}

		if (!successful) {
			NAISE_ERROR_LOG("Could not load ASCII model {} ({})", path, error);
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
	entity->add<TagComponent>(node.name);

	string id = idPrefix + "->" + node.name;

	if (node.mesh >= 0) {
		/* load mesh */
		tinygltf::Mesh mesh = model.meshes[node.mesh];
		entity->add<MeshComponent>();
		entity->component<MeshComponent>().mesh = getMesh<Mesh>(id, mesh, model);

		if (node.translation.size() > 0) {
			auto pos = vec3(node.translation[0], node.translation[1], node.translation[2]);
			entity->component<TransformComponent>().position = pos;
		}

		if (node.scale.size() > 0) {
			auto scale = vec3(node.scale[0], node.scale[1], node.scale[2]);
			entity->component<TransformComponent>().scale = scale;
		}

		if (node.rotation.size() > 0) {
			auto rot = quat( node.rotation[3], node.rotation[0], node.rotation[1], node.rotation[2]);
			entity->component<TransformComponent>().rotation = rot;
		}

		/* load material */
		if (mesh.primitives[0].material >= 0) {
			// TODO: support for more than a single material per object (iflow: probable not soon)
			auto gltfMaterial = model.materials[mesh.primitives[0].material];
			entity->add<MaterialComponent>();
			entity->component<MaterialComponent>().material = getMaterial<PBRMaterial>(id, gltfMaterial, model);
		}
	}
	ret.push_back(entity);

	for (const auto& child: node.children) {
		auto children = entityFromGLTFNode(id, model.nodes[child], model);
		children[0]->add<ParentComponent>(entity->id);
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

ImageFileType Resources::getImageTypeByMagic(const std::string& path) {

	// to read 'magic' out of file
	ifstream input(path);

	if (input.good())
	{
		string firstLine;
		char * buffer = new char [20];

		input.read(buffer, 20);
		getline(input, firstLine);

		if(strncmp(buffer, JPG.magic_signature.c_str(), JPG.magic_signaturBytes) == 0){
			NAISE_DEBUG_CONSOL("ITS A JPG");
			return JPG.imageFileType;
		}
		//TODO test different pngs
		if(strncmp(buffer, PNG.magic_signature.c_str(), PNG.magic_signaturBytes) == 0 ||
						strncmp(firstLine.c_str(), PNG.magic_mnemonic.c_str(), PNG.magic_mnemonicBytes) == 0){
			NAISE_DEBUG_CONSOL("ITS A PNG");
			return PNG.imageFileType;
		}
		//TODO test dds
		if(strncmp(buffer, DDS.magic_signature.c_str(), DDS.magic_signaturBytes) == 0){
			NAISE_DEBUG_CONSOL("ITS A DDS");
			return DDS.imageFileType;
		}
		if(strncmp(buffer, BMP.magic_signature.c_str(), BMP.magic_signaturBytes) == 0){
			NAISE_DEBUG_CONSOL("ITS A BMP");
			return BMP.imageFileType;
		}

	} else {
		NAISE_ERROR_LOG("Could not open File: {}", path);
	}

	NAISE_ERROR_LOG("File type is not supported: {}", path);
	return IMAGE_FILE_INVALID;
}

std::shared_ptr<Texture> Resources::loadWithSTB(const std::string& path) {

	auto texture = make_shared<ImageTexture>();

	int width, height, nrChannels;

	unsigned char* data = stbi_load(path.c_str(), &width, &height, &nrChannels, 0);

	if(data) {

	} else {
		NAISE_ERROR_LOG("Texture failed to load at path: {}", path);
	}

	// Create one OpenGL texture
	GLuint textureID;
	glGenTextures(1, &textureID);

	// "Bind" the newly created texture : all future texture functions will modify this texture
	glBindTexture(GL_TEXTURE_2D, textureID);


	//TODO
	// Give the image to OpenGL
	switch (nrChannels){
		case 1:
			glTexImage2D(GL_TEXTURE_2D, 0, GL_RED, width, height, 0,  GL_RED, GL_UNSIGNED_BYTE, data);
			break;
		case 2:
			glTexImage2D(GL_TEXTURE_2D, 0, GL_RG, width, height, 0, GL_RG, GL_UNSIGNED_BYTE, data);
			break;
		case 3:
			glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, data);
			break;
		case 4:
			glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, data);
			break;
	}

	texture->width = width;
	texture->height = height;
	texture->size = width * height * nrChannels;
	texture->data = data;
	texture->format = GL_RGB;
	texture->textureID = textureID;

//	delete[] data;

	// Poor filtering, or ...
	//glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	//glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);

	// ... nice trilinear filtering.
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
	glGenerateMipmap(GL_TEXTURE_2D);

	glBindTexture(GL_TEXTURE_2D, 0);

	return texture;
}
