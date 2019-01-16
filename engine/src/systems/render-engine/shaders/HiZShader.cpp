#include <systems/render-engine/shaders/HiZShader.hpp>

using namespace NAISE::Engine;

HiZShader::HiZShader()
        : Shader("engine/resources/shaders/post-processing/screen-space-reflections/hi-z-mipmap.vert",
                 "engine/resources/shaders/post-processing/screen-space-reflections/hi-z-mipmap.glsl") {
}

void HiZShader::initUniformLocations() {
    Shader::initUniformLocations();
}
