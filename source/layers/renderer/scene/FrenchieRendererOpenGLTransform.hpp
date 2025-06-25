#pragma once

// Frenchie::Core::Object
#include <FrenchieCoreObject.hpp>

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
            using namespace Frenchie::Core;

            class Transform : public Object
            {
                public:

                Transform(const std::string& _Name = std::string(), Object* _Parent = nullptr);
                virtual ~Transform();

                glm::mat4 get_model_matrix() const;

                // API
                virtual bool awake();
                virtual void frame_start();
                virtual void frame_update();
                virtual void frame_finished();

                protected:

                    glm::vec3 m_Position    = glm::vec3(0.f);
                    glm::vec3 m_Rotation    = glm::vec3(0.f);
                    glm::vec3 m_Scale       = glm::vec3(1.f);
                    glm::mat4 m_ModelMatrix = glm::mat4(1.f);

                    // service methods
                    glm::mat4 compute_local_model_matrix() const;
            };
        }
    }   
}