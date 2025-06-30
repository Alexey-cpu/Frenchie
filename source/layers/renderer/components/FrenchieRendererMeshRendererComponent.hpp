#pragma once

#include <FrenchieRendererTransformComponent.hpp>
#include <FrenchieRendererMesh.hpp>

namespace Frenchie
{
    namespace Renderer
    {
        // MeshRenderer
        class MeshRenderer : public Transform
        {
        public:
            MeshRenderer(
                Mesh*              _Mesh,
                Shader*            _Shader,
                const std::string& _Name = std::string());
            
            virtual ~MeshRenderer();

            virtual bool awake() override;
            virtual void frame_start() override;
            virtual void frame_update() override;
            virtual void frame_finish() override;

        protected:

            Mesh*   m_Mesh   = nullptr;
            Shader* m_Shader = nullptr;
        };
    }
}