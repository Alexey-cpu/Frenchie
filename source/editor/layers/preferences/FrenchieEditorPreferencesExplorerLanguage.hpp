#pragma once

#include <FrenchieApplicationLayer.hpp>

#include <FrenchieEditorConfigurationLoaderLanguage.hpp>

// STL
#include <vector>

// IMGUI
#include <imgui.h>

namespace Frenchie
{
    namespace Editor
    {
        namespace Preferences
        {
            class Languages : 
                public Frenchie::Application::Layer
            {
            public:
                Languages();
                virtual ~Languages();

                // Frenchie::Application::Layer
                virtual void frame_update() override;
                virtual bool allows_multiple_instances() const override;

            protected:
                std::vector<Frenchie::Editor::Configuration::TranslationUnit> m_NewKeys;

                ImGuiTextFilter m_KeyFilter;
                ImGuiTextFilter m_ValueFilter;
                bool            m_ShowFilters{false};
            };
        }
    }
}