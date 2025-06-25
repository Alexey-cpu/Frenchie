#pragma once

// Renderer
#include <FrenchieRendererTransform.hpp>

// OpenGL
#include <FrenchieRendererOpenGLShader.hpp>
#include <FrenchieRendererOpenGLMesh.hpp>

// STL
#include <filesystem>

namespace Frenchie
{
    namespace Renderer
    {
        namespace OpenGL
        {
            class MeshRenderer : public Transform
            {
            public:
                MeshRenderer(
                    Mesh*              _Mesh,
                    ShaderProgram*     _Shader,
                    const std::string& _Name   = std::string(), 
                    Object*            _Parent = nullptr);
                
                virtual ~MeshRenderer();

                virtual bool awake() override;
                virtual void frame_start() override;
                virtual void frame_update() override;
                virtual void frame_finish() override;

            protected:

                Mesh*          m_Mesh   = nullptr;
                ShaderProgram* m_Shader = nullptr;
            };
        }
    }
}