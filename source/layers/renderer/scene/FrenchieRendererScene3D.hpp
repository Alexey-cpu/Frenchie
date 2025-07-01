#pragma once

#include <FrenchieRendererTransformComponent.hpp>
#include <FrenchieRendererCamera.hpp>

// GLM
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

namespace Frenchie
{
    namespace Renderer
    {
        class Scene3D : public Frenchie::Core::Object
        {
        public:

            Scene3D(
                const glm::vec2&   _Size = glm::vec2(2048.f, 1024.f),
                const std::string& _Name = "Scene3D");
            
            virtual ~Scene3D();

            // getters
            glm::vec3 get_viewport_scale() const;
            glm::vec3 get_cursor_position() const;
            glm::vec2 get_size() const;

            // setters
            void set_size(const glm::vec2& _Value);
            void set_cursor_postion(const glm::vec3& _Value);

            // virtual API override
            virtual void frame_start() override;

            protected:

                // info
                glm::vec2  m_Size           = glm::vec2(2048.f, 1024.f);
                glm::vec3  m_CursorPosition = glm::vec3(0.f);
                Camera*    m_Camera         = nullptr;
                Transform* m_Transform      = nullptr;
        };
    }
}