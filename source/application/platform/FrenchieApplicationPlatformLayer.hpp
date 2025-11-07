#pragma once

// Core
#include <FrenchieCoreReference.hpp>

// Application
#include <FrenchieApplicationLayer.hpp>
#include <FrenchieApplicationPlatformImage.hpp>
#include <FrenchieApplicationPlatformBackend.hpp>

// GLM
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <glm/gtx/euler_angles.hpp>

namespace Frenchie
{
    namespace Application
    {
        class Platform : public Layer
        {
        public:
            Platform();
            virtual ~Platform();

            // getters
            std::unique_ptr<PlatformBackendInstance>& get_backend() const;

            virtual bool awake() override;
            virtual void frame_start() override;
            virtual void frame_update() override;
            virtual void frame_render() override;
            virtual void frame_finish() override;
            virtual void finish() override;
            virtual void quit() override;
            virtual bool allows_multiple_instances() const override;

        protected:
            mutable std::unique_ptr<PlatformBackendInstance> m_Backend{nullptr};
        };
    }
}