#pragma once

#include <FrenchieRendererTransformComponent.hpp>
#include <FrenchieRendererMesh.hpp>

namespace Frenchie
{
    namespace Renderer
    {
        // MeshRenderer
        class MeshRenderer : public Component, public IShader
        {
        public:
            MeshRenderer(
                const std::shared_ptr<Mesh>&   _Mesh, 
                const std::shared_ptr<Shader>& _Shader);

            virtual ~MeshRenderer();

            virtual bool awake() override;
            virtual void frame_start() override;
            virtual void frame_update() override;
            virtual void frame_finish() override;

            virtual std::shared_ptr<Shader> get_shader() const override
            {
                return m_Shader;
            }

        protected:

            std::shared_ptr<Mesh>     m_Mesh   = nullptr;
            std::shared_ptr<MeshAABB> m_AABB   = nullptr;
            std::shared_ptr<Shader>   m_Shader = nullptr;
        };
    }
}