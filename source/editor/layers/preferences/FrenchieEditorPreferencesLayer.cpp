#include <FrenchieEditorPreferencesLayer.hpp>

// Core
#include <FrenchieCoreFactory.hpp>
#include <FrenchieCoreHelpers.hpp>

// Application
#include <FrenchieApplicationCommandsLayer.hpp>

// Editor
#include <FrenchieEditorConfigurationTranslatorLayer.hpp>

// IMGUI
#include <imgui.h>

using namespace Frenchie::Editor;
using namespace Frenchie::Editor::Preferences;
using namespace Frenchie::Editor::Configuration;

// Add to main menu
namespace Frenchie
{
    namespace Editor
    {
        namespace MainMenu
        {
            class OpenPreferencesAction : 
                public Frenchie::Application::Command::Registry<OpenPreferencesAction, void*>
            {
            public:

                OpenPreferencesAction(void* _Sender = nullptr) : 
                    Frenchie::Application::Command::Registry<OpenPreferencesAction, void*>(_Sender){}
                virtual ~OpenPreferencesAction(){}

                // Frenchie::Application::Command
                virtual void execute() override
                {
                    Frenchie::Application::application()->push_layer<Frenchie::Editor::Preferences::Explorer>()->show();
                }

                // Command::TRegistryType
                static std::string factory_id()
                {
                    return fmt::format("{}::{}", STRINGIFY(Frenchie::Editor::MainMenu), "Windows::Preferences");
                }
            };
        }
    }
}

// Preferences
Explorer::Explorer() : 
    Layer(Translator::translate("Preferences")){}

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
    // update name
    set_name(Translator::translate("Preferences"));

    // draw
    ImGui::Begin(fmt::format("{}###Preferences", get_name()).c_str(), &m_Opened);
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
            ImGui::TableSetupColumn("Tree", ImGuiTableColumnFlags_WidthFixed, 100.0f);

            ImGui::TableNextRow();

            // draw tree
            ImGui::TableSetColumnIndex(0);

            ImGui::BeginChild(fmt::format("{}::{}", STRINGIFY(Frenchie::Application::Editor::Preferences), "Selectable").c_str());
            {
                for(auto&& layer : m_Topics)
                {
                    bool shown = !layer->is_hidden();

                    if(ImGui::Selectable(
                        fmt::format("{}##{}", layer->get_name(), layer->get_uuid().to_string()).c_str(), &shown))
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