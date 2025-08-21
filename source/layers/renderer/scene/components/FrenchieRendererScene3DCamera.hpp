#pragma once

#include <FrenchieCoreObject.hpp>
#include <FrenchieRendererIEditor.hpp>
#include <FrenchieRendererIRenderer.hpp>

// GLM
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

namespace Frenchie
{
    namespace Renderer
    {
        class Camera : public Frenchie::Core::Component, public IRenderer, public IEditor
        {
        public:

            Camera(glm::vec3  _CameraWorldPosition        = glm::vec3(+0.f, +0.f, +1.f),
                    glm::vec3 _CameraWorldUpAxisDirection = glm::vec3(+0.f, +1.f, +0.f));
            
            virtual ~Camera();

            glm::mat4 get_projection_matrix() const;
            glm::mat4 get_scale_matrix() const;
            glm::vec3 get_position() const;
            glm::vec2 get_resolution() const;

            glm::vec3 get_axis() const;
            glm::vec3 get_front() const;
            glm::vec3 get_up() const;
            glm::vec3 get_right() const;
            float get_movement_speed() const;
            float get_sensitivity() const;
            float get_pitch() const;
            float get_yaw() const;
            float get_roll() const;
            float get_aspect() const;
            float get_near() const;
            float get_far() const;
            float get_fovy() const;
            
            void set_position(const glm::vec3&);
            void set_resolution(const glm::vec2&);
            void set_movement_speed(const float&);
            void set_sensitivity(const float&);
            void set_pitch(const float&);
            void set_yaw(const float&);
            void set_roll(const float&);
            void set_axis(const glm::vec3&);
            void set_near(const float&);
            void set_far(const float&);
            void set_fovy(const float&);

            // public Frenchie::Core::Component
            virtual bool awake() override;
            virtual void frame_start()  override;
            virtual void frame_update() override;
            virtual void frame_finish() override;

            // IEditor
            virtual void draw_editor() override;

            // IRenderer
            virtual unsigned int get_texture() const override;
            virtual void render() override;

        protected:

            glm::mat4 get_view_matrix() const;

            mutable glm::vec3 m_CameraWorldPosition           = glm::vec3(+0.f, +0.f, +1.f);
            mutable glm::vec3 m_CameraWorldUpAxisDirection    = glm::vec3(+0.f, +1.f, +0.f);
            mutable glm::vec3 m_CameraLocalFrontAxisDirection = glm::vec3(0.f);
            mutable glm::vec3 m_CameraLocalRightAxisDirection = glm::vec3(0.f);
            mutable glm::vec3 m_CameraLocalUpAxisDirection    = glm::vec3(0.f);
            
            mutable float     m_MovementSpeed = 1.0f;
            mutable float     m_Sensitivity   = 0.1f;
            mutable float     m_Pitch         = 0.f;
            mutable float     m_Yaw           = 0.f;
            mutable float     m_Roll          = 0.f;
            mutable float     m_Near          = +0.1f;
            mutable float     m_Far           = -1.f;
            mutable float     m_Fovy          = 90.f;
            mutable glm::vec3 m_Axis          = glm::vec3(1.f, 1.f, 1.f);
            mutable glm::vec2 m_Resolution    = glm::vec2(2048.f, 1024.f);

            glm::vec4    m_ClearColor         = glm::vec4(0.25f, 0.25f, 0.25f, 0.5f);
            unsigned int m_Framebuffer        = 0;
            unsigned int m_TextureColorBuffer = 0;
            unsigned int m_TextureDepthBuffer = 0;
        };
    }
}
