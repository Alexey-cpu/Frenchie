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

            virtual bool on_awake() override;
            virtual void on_frame_start() override;
            virtual void on_frame_update() override;
            virtual void on_frame_finish() override;
            virtual void on_finish() override;

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