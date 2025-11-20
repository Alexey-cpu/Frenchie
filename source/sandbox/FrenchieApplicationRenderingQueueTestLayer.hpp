#pragma once

#include <FrenchieApplication.hpp>

namespace Frenchie
{
    namespace Application
    {
        class RenderingQueueTestLayer : public Layer
        {
        public:
            RenderingQueueTestLayer(){}
            virtual ~RenderingQueueTestLayer(){}

            virtual void frame_update() override
            {
                Frenchie::Application::application_rendering_queue()->PushTriangleFilled2D(
                    gs_vec3f(-0.f, +0.f, -500.f),
                    gs_vec3f(+100.f, +0.f, -500.f),
                    gs_vec3f(-0.f, -100.f, -500.f),
                    gs_vec4f(150.f, 32.f, 32.f, 128.f),
                    gs_matrix_translate(gs_mat4f(1), gs_vec3f(100.f, -100.f, -100.f))
                );
            }
        };
    }
}