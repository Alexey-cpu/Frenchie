#pragma once

#include <FrenchieRendererTransformComponent.hpp>
#include <FrenchieRendererMesh.hpp>

namespace Frenchie
{
    namespace Renderer
    {
        // MeshRenderer
        class MeshRenderer : public Component
        {
        public:
            MeshRenderer(Mesh* _Mesh, Shader* _Shader);
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