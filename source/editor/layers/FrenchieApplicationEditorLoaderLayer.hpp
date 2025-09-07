#pragma once

#include <FrenchieCoreThreadPool.hpp>

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
            class AsyncLoaderView : public Dialog
            {
            public:

                AsyncLoaderView(
                    const std::function<void(AsyncLoaderView*)>& _LoadFunction,
                    const std::function<void()>&            _OnFinished,
                    const std::string&                      _Name = STRINGIFY(Frenchie::Application::Editor::AsyncLoaderView));
                virtual ~AsyncLoaderView();
                
                // setters
                void set_progress(const float&);

                // Dialog
                virtual bool awake() override;
                virtual void finish() override;
                virtual void frame_update() override;
                virtual void draw_content() override;
                virtual void draw_buttons() override;

            protected:
                std::function<void(AsyncLoaderView*)>      m_LoadFunction = nullptr;
                std::function<void()>                 m_OnFinished   = nullptr;
                std::shared_ptr<Frenchie::Core::Task> m_Task         = nullptr;
                float                                 m_Progress     = 0.f;
            };
        }
    }
}