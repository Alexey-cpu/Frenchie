#include <FrenchieApplicationEditorPreferencesLayer.hpp>

// Frenchie::Core
#include <FrenchieCoreFactory.hpp>
#include <FrenchieCoreHelpers.hpp>

// IMGUI
#include <imgui.h>

using namespace Frenchie::Application;
using namespace Frenchie::Application::Editor;

// Preferences
Preferences::Preferences() : Layer(STRINGIFY(Preferences)){}
Preferences::~Preferences(){}

bool Preferences::awake()
{
    // create layers    
    for(auto&& registry : Frenchie::Core::Factory::registry())
    {
        if(Frenchie::Core::Helpers::String::contains_substring(
            registry.first, STRINGIFY(Frenchie::Application::Editor::Preferences)))
        {
            m_Layers.push_back(Frenchie::Core::Factory::create<Layer>(registry.first));
            
            if(m_Layers.back())
                m_Layers.back()->hide();
        }
    }

    if(m_Layers.front())
        m_Layers.front()->show();

    // call awake
    return Layer::awake();
}

void Preferences::frame_update()
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
                for(auto&& layer : m_Layers)
                {
                    bool shown = !layer->is_hidden();

                    if(ImGui::Selectable(layer->get_name().c_str(), &shown))
                    {
                        for(auto&& layer : m_Layers) 
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
                for(auto&& layer : m_Layers)
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

bool Preferences::serialize(const Frenchie::Core::Serialization::Node& _Parent)
{
    auto preferences = _Parent.append_node(STRINGIFY(Preferences));

    for(auto&& layer : m_Layers)
    {
        auto serializer = 
            std::dynamic_pointer_cast<ISerializer>(layer);

        if(serializer != nullptr) 
            serializer->serialize(preferences);
    }

    return true;
}

bool Preferences::deserialize(const Frenchie::Core::Serialization::Node& _Parent)
{
    auto preferences = _Parent.find_node(STRINGIFY(Preferences)); 

    if(!preferences.is_valid())
        return false;

    for(auto&& layer : m_Layers)
    {
        auto serializer = 
            std::dynamic_pointer_cast<ISerializer>(layer);

        if(serializer != nullptr) 
            serializer->deserialize(preferences);
    }

    return true;
}