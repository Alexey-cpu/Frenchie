#pragma once

#include <FrenchieApplicationLayer.hpp>

// IMGUI
#include <imgui.h>

namespace Frenchie
{
    namespace Application
    {
        namespace Editor
        {
            class LoaderModel
            {
            public:
                LoaderModel();
                virtual ~LoaderModel();
                
                // virtual API
                virtual bool  awake()   = 0;
                virtual float execute() = 0;
                virtual void  finish()  = 0;
            };

            class LoaderView : public Layer
            {
            public:

                LoaderView(
                    std::shared_ptr<LoaderModel> _Model, 
                    const std::string&           _Name = STRINGIFY(Frenchie::Application::Editor::Loader));
                virtual ~LoaderView();
                
                virtual bool awake() override;
                virtual void frame_update() override;
                virtual void finish() override;

            protected:
                std::shared_ptr<LoaderModel> m_Model = nullptr;
            };
        }
    }
}