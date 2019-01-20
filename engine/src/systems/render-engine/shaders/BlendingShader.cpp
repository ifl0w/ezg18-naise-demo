#include <systems/render-engine/shaders/BlendingShader.hpp>

using namespace NAISE::RenderCore;

BlendingShader::BlendingShader()
        : Shader("engine/resources/shaders/post-processing/screen-space-reflections/hi-z-mipmap.vert",
                 "engine/resources/shaders/post-processing/screen-space-reflections/blending.frag") {
}

void BlendingShader::initUniformLocations() {
    Shader::initUniformLocations();
}