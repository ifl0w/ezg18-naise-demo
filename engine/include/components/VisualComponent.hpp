#pragma once

#include "Component.hpp"

#include <utility>
#include <vector>
#include <memory>
#include <glm/glm.hpp>
#include <glbinding/gl/gl.h>

#include <systems/render-engine/meshes/Mesh.hpp>
#include <systems/render-engine/materials/Material.hpp>
#include <systems/render-engine/materials/PBRMaterial.hpp>

#include "Resources.hpp"

using namespace glm;
using namespace gl;
using namespace std;
using namespace NAISE::RenderCore;

namespace NAISE {
namespace Engine {

class VisualComponent: public Component {
public:
	VisualComponent() = default;

	VisualComponent(const shared_ptr<Mesh>& mesh, const shared_ptr<Material>& material) {
		add(mesh, material);
	};

	explicit VisualComponent(const shared_ptr<Mesh>& mesh) {

	};

	void add(const shared_ptr<Mesh>& mesh) {
		add(mesh, Resources::getMaterial<PBRMaterial>("NAISE::ENGINE::DEFAULT_VISUAL_COMPONENT_MATERIAL"));
	};
	void add(const shared_ptr<Mesh>& mesh, const shared_ptr<Material>& material) {
		meshes.push_back(mesh);
		materials.push_back(material);
	};


	std::vector<shared_ptr<Mesh>> meshes;
	std::vector<shared_ptr<Material>> materials;
};

}
}
