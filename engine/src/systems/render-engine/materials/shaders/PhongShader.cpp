#include <systems/render-engine/materials/shaders/PhongShader.hpp>

using namespace NAISE::Engine;

PhongShader::PhongShader()
		: Shader("engine/resources/shaders/deferred-shading/phong.vert", "engine/resources/shaders/deferred-shading/phong.frag") {
}
