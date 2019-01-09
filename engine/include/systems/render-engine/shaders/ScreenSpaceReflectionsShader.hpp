#pragma once

#include <systems/render-engine/materials/shaders/Shader.hpp>

namespace NAISE {
    namespace Engine {

        class ScreenSpaceReflectionsShader : public Shader {
        public:
            ScreenSpaceReflectionsShader();

            void initUniformLocations() override;

           // void setTextureUnit(GLint textureHandle);

            //void setHorizontalUnit(bool horizontal);

        private:

          //  GLint debugTexturePosition = -1;
           // GLint horizontalLocation = -1;

        };

    }
}
