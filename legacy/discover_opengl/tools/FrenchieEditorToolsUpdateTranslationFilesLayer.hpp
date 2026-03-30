#pragma once

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
            class TranslationFilesUpdater : public Frenchie::Application::Layer
            {
            public:
                TranslationFilesUpdater();
                virtual ~TranslationFilesUpdater();

                virtual void frame_update() override;
                virtual void finish() override;
                virtual bool allows_multiple_instances() const;
            
            protected:

                // info
                mutable std::set<std::string> m_LocalizationKeys;
                mutable std::string           m_SourceCodeFilesPath;

                // service methods
                void try_execute_command(std::function<void()> _Function, const std::string& _Name);
                
            };
        }
    }
}