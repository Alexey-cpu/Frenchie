#pragma once

#include <FrenchieRendererOpenGLTransform.hpp>
#include <FrenchieRendererOpenGLMesh.hpp>

#include <FrenchieCoreFlyweightFactory.hpp>

// STL
#include <filesystem>

namespace Frenchie
{
    namespace Renderer
    {
        namespace OpenGL
        {
            // Rect
            class Rect2D : public Mesh
            {
            public:
                Rect2D();
                virtual ~Rect2D();
            };

            // RectTransform
            class MeshRenderer : public Transform
            {
            public:
                MeshRenderer(
                    Mesh*              _Mesh,
                    ShaderProgram*     _Shader,
                    const std::string& _Name   = std::string(), 
                    Object*            _Parent = nullptr) : 
                    Transform(_Name, _Parent), m_Mesh(_Mesh), m_Shader(_Shader)
                {
                }
                
                virtual ~MeshRenderer()
                {
                }

                bool awake()
                {
                    return Transform::awake();
                }

                void frame_start()
                {
                    Transform::frame_start();
                }

                void frame_update()
                {
                    Transform::frame_update();
                }

                void frame_finish()
                {
                    if(m_Mesh == nullptr || m_Shader == nullptr) 
                        return;

                    // Frenchie::Core::Logger::instance()->info(fmt::format("frame_finish {}", get_name()));
                    // Frenchie::Core::Logger::instance()->info(fmt::format("x: {} y: {} z: {}", get_position().x, get_position().y, get_position().z));
                    
                    // render once
                    m_Shader->begin();
                    m_Shader->set_uniform<glm::mat4>("u_ModelMatrix", m_ModelMatrix);
                    m_Shader->set_uniform<glm::vec4>("u_Color", glm::vec4(0.5f, 0.5f, 0.5f, 1.f));
                    m_Mesh->render();
                    m_Shader->end();

                    Transform::frame_finish();
                }

                Mesh*          m_Mesh   = nullptr;
                ShaderProgram* m_Shader = nullptr;

            protected:
            };
        }
    }
}