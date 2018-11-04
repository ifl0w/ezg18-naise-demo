#pragma once

#include <tiny_gltf.h>
#include <typeindex>

#include <systems/render-engine/materials/shaders/Shader.hpp>
#include <systems/render-engine/meshes/Mesh.hpp>
#include <systems/render-engine/materials/Material.hpp>
#include <systems/render-engine/textures/Texture.hpp>

#include <scene/Entity.hpp>

#include <factories/MeshFactory.hpp>
#include <resource-loader/ModelLoaderAdapter.hpp>

using namespace NAISE::RenderCore;

namespace NAISE {
namespace Engine {

/**
 * All supported image file types should be listed in here
 * source: https://oroboro.com/image-format-magic-bytes/
 */
enum ImageFileType {
  IMAGE_FILE_JPG,
  IMAGE_FILE_PNG,
  IMAGE_FILE_DDS,
  IMAGE_FILE_GIF,
  IMAGE_FILE_BMP,
  IMAGE_FILE_TGA,
  IMAGE_FILE_PSD,
  IMAGE_FILE_HDR,
  IMAGE_FILE_PIC,
  IMAGE_FILE_INVALID,  // unidentified image types.
};

class Resources {
public:

	/**
	 * Frees all resources to enable a clean exit.
	 */
	static void freeAll();

	/**
	 * Returns a shader object for the template type
	 * @tparam T
	 * @return
	 */
	template<typename T>
	static std::shared_ptr<Shader> getShader();

//	static std::shared_ptr<ComputeShader> getComputeShader(const std::string& path);

	/**
	 * Returns a mesh object for the template type, identifier.
	 * If no such mesh object exists, it will be created with the provided arguments.
	 *
	 * @tparam T
	 * @return
	 */
	template<class T, class... Args>
	static std::shared_ptr<Mesh> getMesh(const std::string& identifier, Args&& ... args);

	/**
	 * Returns a material object for the template type, identifier.
	 * If no such object exists, it will be created with the provided arguments.
	 *
	 * @tparam T
	 * @return
	 */
	template<class T, class... Args>
	static std::shared_ptr<Material> getMaterial(const std::string& identifier, Args&& ... args);

	/**
	 * Load a texture by path. The path will be used as identifier. (same as loadTexture(path, path))
	 *
	 * @param path
	 * @return
	 */
	static std::shared_ptr<Texture> loadTexture(const std::string& path);

	/**
	 * Load a texture from tinygltf::Image. The URI in the image will be used as identifier.
	 *
	 * @param path
	 * @return
	 */
	static std::shared_ptr<Texture> loadTexture(const tinygltf::Texture& texture, const tinygltf::Model& model);

	/**
	 * Load a texture and store it under the given identifier.
	 *
	 * @param identifier
	 * @param path
	 * @return
	 */
	static std::shared_ptr<Texture> loadTexture(const std::string& identifier, const std::string& path);

	/**
	 * Load 6 textures for the skybox and store it under the given identifier.
	 *
	 * @param identifier
	 * @param paths
	 * @return
	 */
	static std::shared_ptr<Texture> loadSkyboxTexture(const std::string& identifier,
													  const std::vector<std::string> paths);

	/**
	 * Retrieve a texture by an identifier.
	 * An empty texture is returned, if the identifier does not exist yet.
	 *
	 * @param identifier
	 * @return
	 */
	static std::shared_ptr<Texture> getTexture(const std::string& identifier);

	/**
	 * Returns the type of an image-file
	 * @param path
	 * @return enum
	 */
	static ImageFileType getImageTypeByMagic(const std::string& path);

private:
	static std::map<std::type_index, std::shared_ptr<Shader>> shaders;
//	static std::map<std::string, std::shared_ptr<ComputeShader>> computeShaders;
	static std::map<std::string, std::shared_ptr<Texture>> textures;
	static std::map<pair<type_index, std::string>, std::shared_ptr<Mesh>> meshes;
	static std::map<pair<type_index, std::string>, std::shared_ptr<Material>> materials;

	/**
	 * Loads textures with stbi_load
	 * Supported image-file-types: JPG, PNG, TGA, BMP, PSD, GIF, HDR, PIC
	 */
	static std::shared_ptr<Texture> loadWithSTB(const std::string& path);

};

template<typename T>
std::shared_ptr<Shader> Resources::getShader() {
	auto it = Resources::shaders.find(typeid(T));

	if (it != Resources::shaders.end()) {
		return it->second;
	}

	Resources::shaders[typeid(T)] = std::make_shared<T>();
	return Resources::shaders[typeid(T)];
}

template<class T, class... Args>
std::shared_ptr<Material> Resources::getMaterial(const std::string& identifier, Args&& ... args) {
	auto key = pair<type_index, std::string>(typeid(T), identifier);
	auto it = Resources::materials.find(key);

	if (it != Resources::materials.end()) {
		return it->second;
	}

	Resources::materials[key] = std::make_shared<T>(std::forward<Args>(args) ...);
	return Resources::materials[key];
}

template<class T, class... Args>
std::shared_ptr<Mesh> Resources::getMesh(const std::string& identifier, Args&& ... args) {
	auto key = pair<type_index, std::string>(typeid(T), identifier);
	auto it = Resources::meshes.find(key);

	if (it != Resources::meshes.end()) {
		return it->second;
	}

	Resources::meshes[key] = make_shared<T>(std::forward<Args>(args) ...);
	return Resources::meshes[key];
}

}
}
