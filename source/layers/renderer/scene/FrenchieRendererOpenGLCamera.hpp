#pragma once

#include <FrenchieRendererOpenGLViewport.hpp>

#include <FrenchieCoreObject.hpp>

namespace Frenchie
{
    namespace Renderer
    {
        namespace OpenGL
        {
            class Camera : public Frenchie::Core::Object
            {
            public:

                Camera(glm::vec3 _CameraWorldPosition = glm::vec3(+0.f, +0.f, +1.f),
                        glm::vec3 _CameraWorldUpAxisDirection = glm::vec3(+0.f, +1.f, +0.f), 
                        const std::string& _Name = std::string(), 
                        Frenchie::Core::Object* _Parent = nullptr);
                
                virtual ~Camera();

                glm::vec3 get_position() const;
                float get_pitch() const;
                float get_yaw() const;
                float get_roll() const;
                
                void set_position(const glm::vec3&);
                void set_pitch(const float&);
                void set_yaw(const float&);
                void set_roll(const float&);
                glm::mat4 get_view_matrix() const;

            protected:

                mutable glm::vec3 m_CameraWorldPosition           = glm::vec3(+0.f, +0.f, +1.f);
                mutable glm::vec3 m_CameraWorldUpAxisDirection    = glm::vec3(+0.f, +1.f, +0.f);
                mutable glm::vec3 m_CameraLocalFrontAxisDirection = glm::vec3(0.f);
                mutable glm::vec3 m_CameraLocalRightAxisDirection = glm::vec3(0.f);
                mutable glm::vec3 m_CameraLocalUpAxisDirection    = glm::vec3(0.f);
                mutable float     m_Pitch                         = 0.f;
                mutable float     m_Yaw                           = 0.f;
                mutable float     m_Roll                          = 0.f;
            };
        }
    }
}
