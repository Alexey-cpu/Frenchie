#include <FrenchieApplicationEditorPreferencesLayer.hpp>

// Frenchie::Core
#include <FrenchieCoreFactory.hpp>
#include <FrenchieCoreHelpers.hpp>

// Frenchie::Application
#include <FrenchieApplicationEditorConfigurationLocalizatorLayer.hpp>

// IMGUI
#include <imgui.h>

using namespace Frenchie::Application;
using namespace Frenchie::Application::Editor;
using namespace Frenchie::Application::Editor::Preferences;
using namespace Frenchie::Application::Editor::Configuration;

// Preferences
Explorer::Explorer() : 
    Layer(Localizator::translation("FRENCHIE_APPLICATION_EDITOR_PREFERENCES_NAME_LOCALIZATION_KEY")){}

Explorer::~Explorer(){}

bool Explorer::awake()
{
    // create layers    
    for(auto&& registry : Frenchie::Core::Factory::registry())
    {
        if(Frenchie::Core::String::contains_substring(
            registry.first, STRINGIFY(Frenchie::Application::Editor::Preferences)))
        {
            m_Topics.push_back(Frenchie::Core::Factory::create<Layer>(registry.first));
            
            if(m_Topics.back() == nullptr) 
                continue;

            m_Topics.back()->awake();
            m_Topics.back()->hide();
        }
    }

    if(m_Topics.front())
        m_Topics.front()->show();

    // call awake
    return Layer::awake();

    return true;
}

void Explorer::frame_update()
{
    // draw content
    ImGui::Begin(get_name().c_str(), &m_Shown);
    {
        if(ImGui::BeginTable(
            fmt::format("{}::{}", STRINGIFY(Frenchie::Application::Editor::Preferences), "Splitter").c_str(), 
            2,
            ImGuiTableFlags_::ImGuiTableFlags_ScrollY      | 
            ImGuiTableFlags_::ImGuiTableFlags_RowBg        | 
            ImGuiTableFlags_::ImGuiTableFlags_BordersOuter | 
            ImGuiTableFlags_::ImGuiTableFlags_BordersV     |
            ImGuiTableFlags_::ImGuiTableFlags_Resizable    |
            ImGuiTableFlags_::ImGuiTableFlags_Reorderable  |
            ImGuiTableFlags_::ImGuiTableFlags_Hideable))
        {
            ImGui::TableNextRow();

            // draw tree
            ImGui::TableSetColumnIndex(0);

            ImGui::BeginChild(fmt::format("{}::{}", STRINGIFY(Frenchie::Application::Editor::Preferences), "Selectable").c_str());
            {
                for(auto&& layer : m_Topics)
                {
                    bool shown = !layer->is_hidden();

                    if(ImGui::Selectable(layer->get_name().c_str(), &shown))
                    {
                        for(auto&& layer : m_Topics) 
                            layer->hide();
                        layer->show();
                    }
                }
            }
            ImGui::EndChild();

            // draw table
            ImGui::TableSetColumnIndex(1);

            ImGui::BeginChild(fmt::format("{}::{}", STRINGIFY(Frenchie::Application::Editor::Preferences), "Table").c_str());
            {
                for(auto&& layer : m_Topics)
                {
                    if(!layer->is_hidden())
                        layer->frame_update();
                }
            }
            ImGui::EndChild();

            ImGui::EndTable();
        }

        ImGui::End();
    }
}

bool Explorer::allows_multiple_instances() const
{
    return false;
}