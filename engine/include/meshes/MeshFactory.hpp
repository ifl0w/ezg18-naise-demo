#pragma once

#include <components/MeshComponent.hpp>

#include <vector>
#include <memory>

using namespace glm;
using namespace std;

namespace NAISE {
namespace Engine {

class MeshFactory {
public:
	static shared_ptr<MeshComponent> createBox(float width = 1.0, float height = 1.0, float depth = 1.0);


private:
	static void fillBuffers(MeshComponent& c);
};

}
}
