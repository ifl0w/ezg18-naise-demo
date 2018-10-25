#pragma once

#include <scene/Entity.hpp>

#include <string>
#include <functional>
#include <tiny_gltf.h>

namespace NAISE {
namespace Engine {

class ModelLoaderAdapter {
public:
	/**
	 * Modify the entity based on the node and the model.
	 * The return value determines if the processing of the tree should be stopped and this node should be discarded.
	 *
	 * @param entity
	 * @param node
	 * @param model
	 * @return
	 */
	virtual bool adapt(shared_ptr<Entity> entity, shared_ptr<Entity> parent, const tinygltf::Node& node,
					   const tinygltf::Model& model) const = 0;
};

}
}
