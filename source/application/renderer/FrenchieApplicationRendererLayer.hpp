#pragma once

// Core
#include <FrenchieCoreReference.hpp>

// Application
#include <FrenchieApplicationLayer.hpp>

// STL
#include <filesystem>

// GLM
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

namespace Frenchie
{
    namespace Application
    {
        // Renderer
        class Renderer : public Layer
        {
        public:
            Renderer();
            virtual ~Renderer();

            // Layer
            virtual bool awake() override;
            virtual void frame_start() override;
            virtual void frame_update() override;
            virtual void frame_render() override;
            virtual void frame_finish() override;
            virtual void finish() override;
            virtual void quit() override;
            virtual bool allows_multiple_instances() const override;

        protected:
        };
    }
}