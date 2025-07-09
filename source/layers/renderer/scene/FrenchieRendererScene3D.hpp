#pragma once

#include <FrenchieRendererTransformComponent.hpp>
#include <FrenchieRendererSizeComponent.hpp>
#include <FrenchieRendererCamera.hpp>
#include <FrenchieRendererMesh2D.hpp>

#include <FrenchieCoreCache.hpp>

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
            glm::mat4 get_viewport_scale_matrix() const;
            glm::vec3 get_viewport_scale() const;
            glm::vec3 get_cursor_position() const;

            // setters
            void set_cursor_position(const glm::vec3& _Value);

            // virtual API override
            virtual void frame_start() override;

            protected:

                // info
                glm::vec3  m_CursorPosition = glm::vec3(0.f, 0.f, +1);
                Size*      m_Size           = nullptr;
                Camera*    m_Camera         = nullptr;
                Transform* m_Transform      = nullptr;
        };

        class Scene3DBHV : public Frenchie::Core::Component
        {
        public:
            Scene3DBHV(){}
            
            virtual ~Scene3DBHV(){}

            // IRenderer
            virtual bool awake() override
            {
                return m_Shader != nullptr && m_Mesh != nullptr && m_Shader->instantiate() && m_Mesh->instantiate();
            }
            
            virtual void frame_start()  override
            {
            }
            
            virtual void frame_update() override
            {
            }
            
            virtual void frame_finish() override
            {
                auto scene = get_object<Scene3D>();
                
                if(scene == nullptr || m_Mesh == nullptr || m_Shader == nullptr) 
                    return;

                // render children
                Aabb aabb = Aabb(glm::vec3(0.f, 0.f, 0.f), glm::vec3(0.f, 0.f, 0.f));
                Aabb aabb1 = Aabb(glm::vec3(0.f, 0.f, 0.f), glm::vec3(0.f, 0.f, 0.f));

                scene->apply_to_children_recursive(
                    [this, &aabb, &aabb1](Object* _Objcet)
                    {
                        // compute scene bounding box
                        {
                            auto component = _Objcet->get_component<IMesh>();
                            auto transform = _Objcet->get_component<Transform>();

                            if(component != nullptr && 
                                transform != nullptr && 
                                component->get_mesh() != nullptr)
                            {
                                aabb = aabb.grow(component->get_mesh()->get_aabb().get_size() * 0.5f);

                                aabb1 = aabb1.unite(component->get_mesh()->get_aabb().transform(transform->get_model_matrix()));
                            }
                        }
                    }
                );

                // draw scene mesh mesh
                //aabb = Aabb(glm::vec3(0.f, 0.f, 0.f), glm::vec3(0.f, 0.f, 0.f)).grow(aabb.get_size() * 0.5f);

               // std::cout << "size " << aabb.get_size().z << "\n";

                auto scaleX =  aabb.get_size() / m_Mesh->get_aabb().get_size();

                // auto matrix = 
                //     glm::translate(glm::mat4(1.f), aabb1.get_center()) * 
                //     glm::scale(glm::mat4(1.f), scaleX);

                glm::mat4 matrix(1.f);

            // return glm::translate(matrix, m_Position) * 
            //     glm::rotate(matrix, glm::radians(m_Rotation.x), glm::vec3(1.f, 0.f, 0.f)) * 
            //     glm::rotate(matrix, glm::radians(m_Rotation.y), glm::vec3(0.f, 1.f, 0.f)) * 
            //     glm::rotate(matrix, glm::radians(m_Rotation.z), glm::vec3(0.f, 0.f, 1.f)) * 
            //     glm::scale(matrix, m_Scale);

                m_Shader->use();
                m_Shader->set_uniform<glm::mat4>("u_ScaleMatrix", matrix);
                m_Shader->set_uniform<glm::mat4>("u_ViewMatrix", matrix);
                m_Shader->set_uniform<glm::mat4>("u_ProjectionMatrix", matrix);
                m_Shader->set_uniform<glm::mat4>("u_ModelMatrix", matrix);
                m_Shader->set_uniform<glm::vec4>("u_Color", glm::vec4(1.f, 0.f, 0.f, 0.1f));
                m_Mesh->render();
                m_Shader->unuse();
            }
            
            virtual void draw() override
            {
            }

        protected:
            
            std::shared_ptr<Mesh> m_Mesh = 
                Cache<Rectangle3D>::request("Frenchie/Mesh/Scene3DBHV");

            std::shared_ptr<Shader> m_Shader = 
                Cache<Shader>::request(
                    "Frenchie/Shader/Scene3DBHV", 
                    "C:/SDK/Qt_Projects/OpenGL/shared/shaders/Default/Default.vert",
                    "C:/SDK/Qt_Projects/OpenGL/shared/shaders/Default/Default.frag"); // C:\SDK\Qt_Projects\OpenGL\shared\shaders\Default
        };
    }
}