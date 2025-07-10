#pragma once

#include <FrenchieRendererShader.hpp>

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
            virtual void render() = 0;
            virtual unsigned int get_texture() const = 0;
        };
    }
}
