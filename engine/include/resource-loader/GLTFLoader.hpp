#pragma once

#include <vector>

#include <scene/Entity.hpp>
#include <Logger.hpp>

#include <glm/glm.hpp>

#include "ModelLoaderAdapter.hpp"

namespace NAISE {
namespace Engine {

class GLTFLoader {
public:

	/**
	 * Frees all resources for a clean exit.
	 */
	static void freeAll() {
		models.clear();
	}

	/**
	 * Load a model from a gltf source and return all resulting entities as vector.
	 * The models will be cached and identified by the path.
	 * @param path
	 * @return
	 */
	static vector<shared_ptr<Entity>> loadModel(const std::string& path);

	/**
	 * Load a model from a gltf source with a given adapter and return all resulting entities as vector.
	 * The model will be cached and identified by the path.
	 *
	 * @param path
	 * @return
	 */
	static vector<shared_ptr<Entity>> loadModel(const ModelLoaderAdapter* adapter, const std::string& path);

	template<typename T>
	static std::vector<T> dataFromBuffer(int accessorIdx, const tinygltf::Model& model);

private:
	static std::map<std::string, tinygltf::Model> models;

	/**
	 * Creates an entity for every node in the loaded scene with the given adapter.
	 *
	 * @param idPrefix
	 * @param node
	 * @param model
	 * @return
	 */
	static std::vector<std::shared_ptr<Entity>> entityFromGLTFNode(const ModelLoaderAdapter* adapter,
																   const std::string& idPrefix,
																   int nodeIdx,
																   const tinygltf::Node& node,
																   const tinygltf::Model& model,
																   std::shared_ptr<Entity> parent);

	static void addTransformComponents(std::shared_ptr<Entity>& entity, const tinygltf::Node& node,
									   const tinygltf::Model& model);

	static void addVisualComponents(std::shared_ptr<Entity>& entity, const std::string& idPrefix,
									const tinygltf::Node& node, const tinygltf::Model& model);

	static void addAnimationComponents(std::shared_ptr<Entity>& entity,
									   int nodeIdx,
									   const tinygltf::Node& node,
									   const tinygltf::Model& model,
									   const std::string& idPrefix);

	static void addCameraComponents(std::shared_ptr<Entity>& entity,  const tinygltf::Node& node, const tinygltf::Model& model);
};

template<typename T>
vector<T> GLTFLoader::dataFromBuffer(int accessorIdx, const tinygltf::Model& model) {
	tinygltf::Accessor accessor = model.accessors[accessorIdx];
	tinygltf::BufferView bufferView = model.bufferViews[accessor.bufferView];

	if (bufferView.byteStride != 0) {
		NAISE_WARN_LOG("Stride in GLTF data is currently not supported!")
		return std::vector<T>();
	}

	std::vector<T> result = std::vector<T>(accessor.count);

	auto sourceTypeSize = tinygltf::GetTypeSizeInBytes(static_cast<uint32_t>(accessor.type));
	auto sourceComponentSize = tinygltf::GetComponentSizeInBytes(static_cast<uint32_t>(accessor.componentType));
	auto sourceSize = static_cast<size_t>(sourceTypeSize) * static_cast<size_t>(sourceComponentSize);
	auto targetSize = sizeof(T);

	if (sourceSize == targetSize) {

		memcpy(result.data(), model.buffers[bufferView.buffer].data.data() + bufferView.byteOffset,
			   accessor.count * targetSize);

	} else if (sourceSize < targetSize) {
		// IMPORTANT! This probably only works correct on little endian systems (e.g.: x86, amd64)

		auto base = model.buffers[bufferView.buffer].data.data() + bufferView.byteOffset;
		for (int i = 0; i < result.size(); ++i) {
			memcpy(&result[i], base + i * sourceSize, sourceSize);
		}

	} else {
		NAISE_ERROR_LOG("This conversion is not supported! (source size: {}, target size: {})", sourceSize, targetSize)
	}

	return result;
}

}
}

