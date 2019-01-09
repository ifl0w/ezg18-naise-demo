#include <systems/render-engine/shaders/ScreenSpaceReflectionsShader.hpp>

using namespace NAISE::Engine;

ScreenSpaceReflectionsShader::ScreenSpaceReflectionsShader()
        : Shader("engine/resources/shaders/post-processing/screen_space_reflections.vert",
                 "engine/resources/shaders/post-processing/screen_space_reflections.frag") {

   // this->debugTexturePosition = uniformLocation(this->shaderID, "emissionInput");
   // this->horizontalLocation = uniformLocation(this->shaderID, "horizontal");
}

void ScreenSpaceReflectionsShader::initUniformLocations() {
    Shader::initUniformLocations();
}
