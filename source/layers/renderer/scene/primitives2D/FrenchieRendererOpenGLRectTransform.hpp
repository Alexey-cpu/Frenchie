#pragma once

#include <FrenchieRendererOpenGLTransform.hpp>
#include <FrenchieRendererOpenGLMesh.hpp>

// STL
#include <filesystem>

namespace Frenchie
{
    namespace Renderer
    {
        namespace OpenGL
        {
            // Rect
            class Rect : public Mesh
            {
            public:
                Rect();
                virtual ~Rect();
            };

            // RectTransform
            class RectTransform : public Transform
            {
            public:
                RectTransform(const std::string& _Name = std::string(), Object* _Parent = nullptr);
                virtual ~RectTransform();

                // virtual API
                virtual bool awake() override;
                virtual void frame_start() override;
                virtual void frame_update() override;
                virtual void frame_finish() override;

            protected:
                Mesh* m_Mesh = nullptr;
            };
        }
    }
}