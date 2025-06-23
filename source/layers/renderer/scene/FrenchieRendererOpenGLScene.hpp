#include <FrenchieCoreObject.hpp>
#include <FrenchieRendererOpenGLCamera.hpp>

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
            struct Transform
            {
                glm::vec3 Position = glm::vec3(0.f);
                glm::vec3 Rotation = glm::vec3(0.f);
                glm::vec3 Scale    = glm::vec3(1.f);
                glm::vec3 Offset   = glm::vec3(0.f);

                glm::mat4 get_model_matrix() const
                {
                    glm::mat4 matrix(1.f);

                    return  glm::translate(matrix, Position + Offset) * 
                            glm::rotate(matrix, glm::radians(Rotation.x), glm::vec3(1.f, 0.f, 0.f)) * 
                            glm::rotate(matrix, glm::radians(Rotation.x), glm::vec3(0.f, 1.f, 0.f)) * 
                            glm::rotate(matrix, glm::radians(Rotation.x), glm::vec3(0.f, 0.f, 1.f)) * 
                            glm::scale(matrix, Scale);
                }
            };

            class TransformObject : public Frenchie::Core::Object
            {
                public:

                TransformObject(std::string _Name = std::string(), Frenchie::Core::Object* _Parent = nullptr) : 
                    Frenchie::Core::Object(_Name, _Parent){}

                void updateSelfAndChild()
                {
                    if (get_parent<TransformObject>() != nullptr)
                        m_ModelMatrix = get_parent<TransformObject>()->m_ModelMatrix * m_Transform.get_model_matrix();
                    else
                        m_ModelMatrix = m_Transform.get_model_matrix();

                    for (auto&& child : m_Children)
                    {
                        TransformObject* object = 
                            dynamic_cast<TransformObject*>(child);

                        if(object != nullptr)
                            object->updateSelfAndChild();
                    }
                }

                protected:

                    Transform m_Transform   = Transform();
                    glm::mat4 m_ModelMatrix = glm::mat4(1.0f);
            };
        }
    }
}