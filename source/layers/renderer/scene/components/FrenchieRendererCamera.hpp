#pragma once

#include <FrenchieCoreObject.hpp>
#include <FrenchieRendererIEditor.hpp>

// GLM
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

namespace Frenchie
{
    namespace Renderer
    {
        class Camera : public Core::Component::Registry<Camera>, public IEditor
        {
        public:

            Camera(glm::vec3  _CameraWorldPosition        = glm::vec3(+0.f, +0.f, +1.f),
                    glm::vec3 _CameraWorldUpAxisDirection = glm::vec3(+0.f, +1.f, +0.f));
            
            virtual ~Camera();

            glm::mat4 get_projection_matrix() const;
            glm::mat4 get_view_matrix() const;
            glm::vec3 get_position() const;

            glm::vec3 get_object_perspective_scale(glm::mat4 _Transform) const;

            glm::vec3 get_axis() const;
            glm::vec3 get_front() const;
            float get_movement_speed() const;
            float get_pitch() const;
            float get_yaw() const;
            float get_roll() const;
            float get_aspect() const;
            float get_near() const;
            float get_far() const;
            float get_fovy() const;
            
            void set_position(const glm::vec3&);
            void set_movement_speed(const float&);
            void set_pitch(const float&);
            void set_yaw(const float&);
            void set_roll(const float&);
            void set_axis(const glm::vec3&);
            void set_size(const glm::vec2&);
            void set_aspect(const float&);
            void set_near(const float&);
            void set_far(const float&);
            void set_fovy(const float&);

            // IEditor
            virtual void draw_editor() override;

            // Component::Register<Transform>
            static TReturnType create();

        protected:

            mutable glm::vec3 m_CameraWorldPosition           = glm::vec3(+0.f, +0.f, +1.f);
            mutable glm::vec3 m_CameraWorldUpAxisDirection    = glm::vec3(+0.f, +1.f, +0.f);
            mutable glm::vec3 m_CameraLocalFrontAxisDirection = glm::vec3(0.f);
            mutable glm::vec3 m_CameraLocalRightAxisDirection = glm::vec3(0.f);
            mutable glm::vec3 m_CameraLocalUpAxisDirection    = glm::vec3(0.f);
            
            mutable float     m_MovementSpeed = 2.f;
            mutable float     m_Pitch         = 0.f;
            mutable float     m_Yaw           = 0.f;
            mutable float     m_Roll          = 0.f;
            mutable float     m_Near          = +0.1f;
            mutable float     m_Far           = -1.f;
            mutable float     m_Aspect        = 1.f;
            mutable float     m_Fovy          = 90.f;
            mutable glm::vec3 m_Axis          = glm::vec3(1.f, 1.f, 1.f);
        };
    }
}
