#pragma once

#include "RenderTarget.hpp"
#include "systems/render-engine/shaders/GlowShader.hpp"
#include "PingPongTarget.hpp"

namespace NAISE {
    namespace Engine {

        class HiZRenderTarget: public RenderTarget {
        public:
            HiZRenderTarget();
            HiZRenderTarget(int width, int height, int samples);
            ~HiZRenderTarget() override;

            void use() override;

            GLuint gLinearDepth;
            GLuint output;
            GLuint depth_rbo;

        private:
            int width;
            int height;
            int samples;
        };

    }
}

