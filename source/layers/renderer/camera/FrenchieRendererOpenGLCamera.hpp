#pragma once

// GLAD
#include <glad/glad.h> 

// GLFW
#include <GLFW/glfw3.h>

// GLM
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

namespace Frenchie
{
    namespace Renderer
    {
        namespace OpenGL
        {
            class Camera
            {
            public:

                // constructor with vectors
                Camera(glm::vec3 _WorldPosition = glm::vec3(0.0f, 0.0f, -1.0f), glm::vec3 _WorldUpAxisDirection = glm::vec3(0.f, 0.f, 1.f));
                virtual ~Camera();

                glm::mat4 get_view_matrix() const;
                glm::vec3 get_euler_angles() const;
                glm::vec3 get_camera_world_position() const;

                float get_pitch() const;
                float get_yaw() const;
                float get_roll() const;
                void  set_camera_world_postion(const glm::vec3& _Value);

                void set_yaw(const float& _Value);
                void set_pitch(const float& _Value);
                void set_roll(const float& _Value);
                void set_euler_angles(const glm::vec3& _Value);

                void move_forward(const float& _DeltaTime);
                void move_backward(const float& _DeltaTime);
                void move_right(float _DeltaTime);
                void move_left(float _DeltaTime);

            private:

                // info
                glm::vec3 m_CameraWorldPosition;
                glm::vec3 m_CameraUpAxisDirection;
                glm::vec3 m_CameraFrontAxisDirection;
                glm::vec3 m_CameraRightAxisDirection;
                glm::vec3 m_WorldUpAxisDirection;
                
                float m_CameraMovementSpeed       = 1.f;
                float m_CameraRotationSensitivity = 0.1f;

                // euler Angles
                float m_Yaw   = 0.f;
                float m_Pitch = 0.f;
                float m_Roll  = 0.f;

                void updateCameraVectors();
            };
        }
    }
}