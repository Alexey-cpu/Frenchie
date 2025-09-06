#pragma once

#include <FrenchieApplicationLayer.hpp>
#include <FrenchieApplicationEditorAbstractDialogLayer.hpp>

// IMGUI
#include <imgui.h>

namespace Frenchie
{
    namespace Application
    {
        namespace Editor
        {
            // Loader
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

            class LoaderView : public Dialog
            {
            public:

                LoaderView(
                    std::shared_ptr<LoaderModel> _Model, 
                    const std::string&           _Name = STRINGIFY(Frenchie::Application::Editor::LoaderView));
                virtual ~LoaderView();
                
                virtual bool awake() override;
                virtual void finish() override;
                virtual void draw_content() override;
                virtual void draw_buttons() override;

            protected:
                std::shared_ptr<LoaderModel> m_Model = nullptr;
            };
        }
    }
}