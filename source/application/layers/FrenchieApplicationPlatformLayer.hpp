#pragma once

// Core
#include <FrenchieCoreReference.hpp>

// Application
#include <FrenchieApplicationLayer.hpp>

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
            template<typename Type>
            Type* get_context() const;
            glm::u32vec2 get_window_size() const;

            // setters
            void set_window_size(const glm::u32vec2& _Value);
            void set_maximized(const bool& _Value);

            virtual bool awake() override;
            virtual void frame_start() override;
            virtual void frame_update() override;
            virtual void frame_render() override;
            virtual void frame_finish() override;
            virtual void finish() override;
            virtual void quit() override;
            virtual bool allows_multiple_instances() const override;

        protected:
            void* m_Context {nullptr};
        };
    }
}