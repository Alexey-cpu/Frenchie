#pragma once

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

                Transform(const std::string& _Name = std::string(), Object* _Parent = nullptr) : Object(_Name, _Parent){}

                glm::mat4 get_model_matrix() const
                {
                    glm::mat4 matrix(1.f);

                    return  glm::translate(matrix, m_Position) * 
                            glm::rotate(matrix, glm::radians(m_Rotation.x), glm::vec3(1.f, 0.f, 0.f)) * 
                            glm::rotate(matrix, glm::radians(m_Rotation.x), glm::vec3(0.f, 1.f, 0.f)) * 
                            glm::rotate(matrix, glm::radians(m_Rotation.x), glm::vec3(0.f, 0.f, 1.f)) * 
                            glm::scale(matrix, m_Scale);
                }

                protected:

                    glm::vec3 m_Position = glm::vec3(0.f);
                    glm::vec3 m_Rotation = glm::vec3(0.f);
                    glm::vec3 m_Scale    = glm::vec3(1.f);
            };
        }
    }   
}