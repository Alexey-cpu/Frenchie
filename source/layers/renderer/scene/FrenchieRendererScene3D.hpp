#pragma once

#include <FrenchieRendererScene3DCamera.hpp>
#include <FrenchieRendererMesh2D.hpp>
#include <FrenchieRendererShader.hpp>

#include <FrenchieRendererScene3DCursor.hpp>
#include <FrenchieRendererTransformComponent.hpp>

#include <FrenchieCoreCache.hpp>
#include <FrenchieRendererIEditor.hpp>

// GLM
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

// STL
#include <iostream>

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

            Reference<Camera> get_active_camera() const
            {
                // fill cameras
                if(m_CameraInfos.empty())
                {
                    // get self camera
                    CameraInfo selfCamera =             
                    {
                        get_name(),
                        get_component<Camera>(), 
                        true
                    };

                    if(selfCamera.Reference != nullptr)
                        m_CameraInfos.push_back(selfCamera);

                    // get child cameras
                    apply_to_children(
                        [this](Object* _Object)
                        {
                            auto camera = _Object->get_component<Camera>();

                            if(camera == nullptr) 
                                return;
                                
                            m_CameraInfos.push_back(
                            {
                                _Object->get_name(),
                                camera, 
                                false
                            }
                            );
                        }
                    );

                    if(selfCamera.Reference != nullptr) 
                        return selfCamera.Reference;

                    if(!m_CameraInfos.empty()) 
                        return m_CameraInfos.begin()->Reference;

                    return nullptr;
                }

                // remove expired references
                for(auto it = m_CameraInfos.begin(); it != m_CameraInfos.end(); it++) 
                {
                    if(it->Reference == nullptr)
                    {
                        auto rm = it;
                        it++;
                        m_CameraInfos.erase(rm);
                    }
                }

                // update existing cameras
                auto selfCamera = get_component<Camera>();

                if(selfCamera != nullptr)
                {
                    CameraInfo info =             
                    {
                        get_name(),
                        selfCamera, 
                        false
                    };

                    if(std::find_if(
                        m_CameraInfos.begin(), 
                        m_CameraInfos.end(),
                        [&info](const CameraInfo& _Info)->bool{return _Info.Reference == info.Reference;}) == m_CameraInfos.end())
                    {
                        m_CameraInfos.push_back(info);   
                    }
                }

                apply_to_children(
                    [this](Object* _Object)
                    {
                        auto camera = _Object->get_component<Camera>();

                        if(camera == nullptr) 
                            return;

                        CameraInfo info =             
                        {
                            _Object->get_name(),
                            camera, 
                            false
                        };
                            
                        if(std::find_if(
                            m_CameraInfos.begin(), 
                            m_CameraInfos.end(),
                            [&info](const CameraInfo& _Info)->bool{return _Info.Reference == info.Reference;}) == m_CameraInfos.end())
                        {
                            m_CameraInfos.push_back(info);   
                        }
                    }
                );

                // find an active camera
                for(auto&& cameraInfo : m_CameraInfos)
                {
                    if(cameraInfo.Active)
                    {
                        return cameraInfo.Reference;
                    }
                }

                if(m_CameraInfos.empty()) 
                    return nullptr;

                auto first = m_CameraInfos.begin();
                first->Active = true;

                return first->Reference;
            }


            // virtual API override
            virtual void frame_start() override;
            virtual void frame_update() override;
            virtual void frame_finish() override;

            virtual void draw_self() override;

            protected:

                Reference<Transform>     m_Transform   = nullptr;
                Reference<Scene3DCursor> m_MousePicker = nullptr;

                struct CameraInfo
                {
                    mutable std::string       Name      = std::string();
                    mutable Reference<Camera> Reference = nullptr;
                    mutable bool              Active    = false;

                    // nested types
                    struct TransparentComparator
                    {
                        using is_transparent = CameraInfo;

                    public:

                        bool operator()(const CameraInfo& _A, const CameraInfo& _B) const
                        {
                            return  _A.Reference < _B.Reference;
                        }
                    };
                };

                mutable std::list<CameraInfo> m_CameraInfos = std::list<CameraInfo>();
        };

        // class Scene3DBHV : public Frenchie::Core::Component, public IEditor
        // {
        // public:
        //     Scene3DBHV(){}
            
        //     virtual ~Scene3DBHV(){}

        //     // IRenderer
        //     virtual bool awake() override
        //     {
        //         return m_Shader != nullptr && m_Mesh != nullptr && m_Shader->instantiate() && m_Mesh->instantiate();
        //     }
            
        //     virtual void frame_start()  override
        //     {
        //     }
            
        //     virtual void frame_update() override
        //     {
        //     }
            
        //     virtual void frame_finish() override
        //     {
        //         auto scene = get_object<Scene3D>();
                
        //         if(scene == nullptr || m_Mesh == nullptr || m_Shader == nullptr) 
        //             return;

        //         // render children
        //         Aabb aabb = Aabb(glm::vec3(0.f, 0.f, 0.f), glm::vec3(0.f, 0.f, 0.f));
        //         Aabb aabb1 = Aabb(glm::vec3(0.f, 0.f, 0.f), glm::vec3(0.f, 0.f, 0.f));

        //         scene->apply_to_children_recursive(
        //             [this, &aabb, &aabb1](Object* _Objcet)
        //             {
        //                 // compute scene bounding box
        //                 {
        //                     auto component = _Objcet->get_component<IMesh>();
        //                     auto transform = _Objcet->get_component<Transform>();

        //                     if(component != nullptr && 
        //                         transform != nullptr && 
        //                         component->get_mesh() != nullptr)
        //                     {
        //                         aabb = aabb.grow(component->get_mesh()->get_aabb().get_size() * 0.5f);

        //                         aabb1 = aabb1.unite(component->get_mesh()->get_aabb().transform(transform->get_model_matrix()));
        //                     }
        //                 }
        //             }
        //         );

        //         // draw scene mesh mesh
        //         //aabb = Aabb(glm::vec3(0.f, 0.f, 0.f), glm::vec3(0.f, 0.f, 0.f)).grow(aabb.get_size() * 0.5f);

        //        // std::cout << "size " << aabb.get_size().z << "\n";

        //         auto scaleX =  aabb.get_size() / m_Mesh->get_aabb().get_size();

        //         // auto matrix = 
        //         //     glm::translate(glm::mat4(1.f), aabb1.get_center()) * 
        //         //     glm::scale(glm::mat4(1.f), scaleX);

        //         glm::mat4 matrix(1.f);

        //     // return glm::translate(matrix, m_Position) * 
        //     //     glm::rotate(matrix, glm::radians(m_Rotation.x), glm::vec3(1.f, 0.f, 0.f)) * 
        //     //     glm::rotate(matrix, glm::radians(m_Rotation.y), glm::vec3(0.f, 1.f, 0.f)) * 
        //     //     glm::rotate(matrix, glm::radians(m_Rotation.z), glm::vec3(0.f, 0.f, 1.f)) * 
        //     //     glm::scale(matrix, m_Scale);

        //         m_Shader->use();
        //         m_Shader->set_uniform<glm::mat4>("u_ScaleMatrix", matrix);
        //         m_Shader->set_uniform<glm::mat4>("u_ViewMatrix", matrix);
        //         m_Shader->set_uniform<glm::mat4>("u_ProjectionMatrix", matrix);
        //         m_Shader->set_uniform<glm::mat4>("u_ModelMatrix", matrix);
        //         m_Shader->set_uniform<glm::vec4>("u_Color", glm::vec4(1.f, 0.f, 0.f, 0.1f));
        //         m_Mesh->render();
        //         m_Shader->unuse();
        //     }
            
        //     virtual void draw_editor() override
        //     {
        //     }

        // protected:
            
        //     std::shared_ptr<Mesh> m_Mesh = 
        //         Cache<Rectangle3D>::request("Frenchie/Mesh/Scene3DBHV");

        //     std::shared_ptr<Shader> m_Shader = 
        //         Cache<Shader>::request(
        //             "Frenchie/Shader/Scene3DBHV", 
        //             "C:/SDK/Qt_Projects/OpenGL/shared/shaders/Default/Default.vert",
        //             "C:/SDK/Qt_Projects/OpenGL/shared/shaders/Default/Default.frag"); // C:\SDK\Qt_Projects\OpenGL\shared\shaders\Default
        // };
    }
}