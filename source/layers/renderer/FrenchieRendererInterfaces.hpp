#pragma once

#include <FrenchieRendererShader.hpp>

#include <memory>

namespace Frenchie
{
    namespace Renderer
    {
        class IRenderer
        {
        public:
            IRenderer(){}
            virtual ~IRenderer(){}

            // virtual API
            virtual bool awake()        = 0;
            virtual void frame_start()  = 0;
            virtual void frame_update() = 0;
            virtual void frame_finish() = 0;
            virtual void draw() = 0;
        };
    }
}
