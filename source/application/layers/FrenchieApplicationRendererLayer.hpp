#pragma once

// Core
#include <FrenchieCoreReference.hpp>

// Application
#include <FrenchieApplicationLayer.hpp>

namespace Frenchie
{
    namespace Application
    {
        class Renderer : public Layer
        {
        public:
            Renderer();
            virtual ~Renderer();

            virtual bool awake() override;
            virtual void frame_start() override;
            virtual void frame_update() override;
            virtual void frame_render() override;
            virtual void frame_finish() override;
            virtual void finish() override;
            virtual void quit() override;
            virtual bool allows_multiple_instances() const override;
        };
    }
}