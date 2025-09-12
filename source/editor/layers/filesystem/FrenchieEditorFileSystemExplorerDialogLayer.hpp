#pragma once

#include <FrenchieEditorFileSystemExplorerLayer.hpp>

namespace Frenchie
{
    namespace Editor
    {
        namespace FileSystem
        {
            class ExplorerDialog : public FileSystem::Explorer 
            {
            public:
                ExplorerDialog(const std::string& _Name = STRINGIFY(ExplorerDialog));
                virtual ~ExplorerDialog();

                // Frenchie::Application::Layer
                virtual void frame_update() override;
                virtual bool allows_multiple_instances() const override;

                // API
                void on_accepted(const std::function<void()>& _Callback)
                {
                    m_OnAccepted = _Callback;
                }
                
                void on_canceled(const std::function<void()>& _Callback)
                {
                    m_OnCanceled = _Callback;
                }

            protected:
                std::function<void()> m_OnAccepted = nullptr;
                std::function<void()> m_OnCanceled = nullptr;
            };
        }
    }
}