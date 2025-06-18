#pragma once

// GLAD
#include <glad/glad.h> 

// GLFW
#include <GLFW/glfw3.h>

// GLM
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>


// Defines several possible options for camera movement. Used as abstraction to stay away from window-system specific input methods
enum Camera_Movement {
    FORWARD,
    BACKWARD,
    LEFT,
    RIGHT,
    CLOCKWISE,
    COUNTERCLOCKWISE
};

// Default camera values
const float YAW         = -90.0f;
const float PITCH       =  0.0f;
const float SPEED       =  2.5f;
const float SENSITIVITY =  0.1f;
const float ZOOM        =  45.0f;


// An abstract camera class that processes input and calculates the corresponding Euler Angles, Vectors and Matrices for use in OpenGL
class Camera
{
public:
    // camera Attributes
    glm::vec3 Position;
    glm::vec3 Front;
    glm::vec3 Up;
    glm::vec3 Right;
    glm::vec3 WorldUp;
    
    // euler Angles
    float Yaw  = 0.f;
    float Pitch  = 0.f;
    float Roll  = 0.f;

    // camera options
    float MovementSpeed;
    float MouseSensitivity;
    float Zoom;

    glm::quat Orientation;

    // constructor with vectors
    Camera(
        glm::vec3 position = glm::vec3(0.0f, 0.0f, 0.0f), 
        glm::vec3 up       = glm::vec3(0.0f, 1.0f, 0.0f), 
        float     yaw      = YAW, 
        float     pitch    = PITCH) : Front(glm::vec3(0.0f, 0.0f, -1.0f)), MovementSpeed(SPEED), MouseSensitivity(SENSITIVITY), Zoom(ZOOM)
    {
        Position = position;
        WorldUp = up;
        Yaw = yaw;
        Pitch = pitch;
        updateCameraVectors();
    }

    // returns the view matrix calculated using Euler Angles and the LookAt Matrix
    glm::mat4 GetViewMatrix()
    {
        return glm::lookAt(Position, Position + Front, Up);
    }

    // processes input received from any keyboard-like input system. Accepts input parameter in the form of camera defined ENUM (to abstract it from windowing systems)
    void ProcessKeyboard(Camera_Movement direction, float deltaTime)
    {
        float velocity = MovementSpeed * deltaTime;
        if (direction == FORWARD)
            Position += Front * velocity;
        if (direction == BACKWARD)
            Position -= Front * velocity;
        if (direction == LEFT)
            Position -= Right * velocity;
        if (direction == RIGHT)
            Position += Right * velocity;
        if (direction == CLOCKWISE) 
            Roll += 5.f * velocity;
        if (direction == COUNTERCLOCKWISE) 
            Roll -= 5.f * velocity;

        // update Front, Right and Up Vectors using the updated Euler angles
        updateCameraVectors();
    }

    // processes input received from a mouse input system. Expects the offset value in both the x and y direction.
    void ProcessMouseMovement(float xoffset, float yoffset, GLboolean constrainPitch = true)
    {
        xoffset *= MouseSensitivity;
        yoffset *= MouseSensitivity;

        Yaw   += xoffset;
        Pitch += yoffset;

        // make sure that when pitch is out of bounds, screen doesn't get flipped
        if (constrainPitch)
        {
            if (Pitch > 89.0f)
                Pitch = 89.0f;
            if (Pitch < -89.0f)
                Pitch = -89.0f;
        }

        // update Front, Right and Up Vectors using the updated Euler angles
        updateCameraVectors();
    }

    // processes input received from a mouse scroll-wheel event. Only requires input on the vertical wheel-axis
    void ProcessMouseScroll(float yoffset)
    {
        Zoom -= (float)yoffset;
        if (Zoom < 1.0f)
            Zoom = 1.0f;
        if (Zoom > 45.0f)
            Zoom = 45.0f;
    }

private:
    // calculates the front vector from the Camera's (updated) Euler Angles
    void updateCameraVectors()
    {
        // rotate aroung X/Y axix
        glm::mat4 uinitMatrix = glm::mat4(1.f);
        glm::mat4 transformX  = glm::rotate(uinitMatrix, glm::radians(Pitch), glm::vec3(1.f, 0.f, 0.f));
        glm::mat4 transformY  = glm::rotate(uinitMatrix, -(glm::radians(Yaw) + glm::half_pi<float>()), glm::vec3(0.f, 1.f, 0.f));
        glm::vec4 direction   = transformY * transformX * glm::vec4(0.f, 0.f, -1.f, 1.f);
        Front = glm::normalize(direction);
        Right = glm::normalize(glm::cross(Front, WorldUp));
        Up    = glm::normalize(glm::cross(Right, Front));

        // rotate aroung Z axis
        glm::mat4 transformZ = glm::rotate(glm::mat4(1.f), glm::radians(Roll), glm::vec3(0.f, 0.f, 1.f));
        Front = glm::normalize(glm::vec3(transformZ * glm::vec4(Front, 1.f)));
        Up = glm::normalize(glm::vec3(transformZ * glm::vec4(Up, 1.f)));
    }
};