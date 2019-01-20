#pragma once

#include <systems/render-engine/materials/shaders/Shader.hpp>

namespace NAISE {
    namespace RenderCore {

        class BlendingShader : public Shader {
        public:
            BlendingShader();

            void initUniformLocations() override;

        private:

        };

    }
}
