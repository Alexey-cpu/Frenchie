#pragma once

// Application
#include <FrenchieApplicationLayer.hpp>

// Editor
#include <FrenchieEditorConfigurationTranslatorLayer.hpp>

// IMGUI
#include <imgui.h>

// STL
#include <set>

namespace Frenchie
{
    namespace Editor
    {
        namespace Tools
        {
            // TranslationFilesEditor
            class TranslationFilesEditor : public Frenchie::Application::Layer
            {
            public:
                TranslationFilesEditor();
                virtual ~TranslationFilesEditor();

                virtual void frame_update() override;
                virtual void finish() override;
                virtual bool allows_multiple_instances() const;
            
            protected:

                // info
                std::shared_ptr<Configuration::LoadTranslationFilesProcess> m_LoadProcess;
                std::shared_ptr<Configuration::SaveTranslationFilesProcess> m_SaveProcess;
                std::mutex                                                  m_Mutex;
                std::vector<Configuration::TranslationUnit>                 m_TranslationUnitsToAdd;
                ImGuiTextFilter                                             m_TextFilter;

                // service methods
                void try_execute_command(std::function<void()> _Function, const std::string& _Name);
            };
        }
    }
}