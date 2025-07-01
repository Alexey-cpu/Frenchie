#pragma once

#include <FrenchieApplicationLayer.hpp>

#include <FrenchieRendererScene3D.hpp>

namespace Frenchie
{
    namespace Application
    {
        using namespace Frenchie::Renderer;

        class InspectorView : public Layer
        {
        public:

            InspectorView(const std::string& _Name, std::shared_ptr<Scene3D> _Scene3D);
            virtual ~InspectorView();

            virtual bool awake() override;
            virtual void frame_start() override;
            virtual void frame_update() override;
            virtual void frame_finish() override;
            virtual void finish() override;
            virtual void close() override;
            virtual bool is_closed() override;

        protected:
            std::shared_ptr<Scene3D> m_Scene = nullptr;

            class SerachPanel
            {
            public:
                void draw(bool _Draw);

            protected:
                char m_Search[512]{};

                void drawRegistryItem(const std::vector<std::string>& _Tree, int& _Index);

            } m_SeachPanel;
        };
    }
};