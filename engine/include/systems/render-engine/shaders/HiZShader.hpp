#pragma once

#include <systems/render-engine/materials/shaders/Shader.hpp>

namespace NAISE {
namespace RenderCore {

        class HiZShader : public Shader {
        public:
            HiZShader();

            void initUniformLocations() override;

        private:

        };

}
}
