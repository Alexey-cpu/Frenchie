#include <FrenchieApplicationEditorPreferencesLanguageLayer.hpp>
#include <FrenchieApplicationEditorFileSystemExplorerLayer.hpp>
#include <FrenchieApplicationEditorConfigurationTranslatorLayer.hpp>


// IMGUI
#include <imgui_stdlib.h>

// STL
#include <chrono>
#include <regex>
#include <fstream>
#include <iostream>
#include <functional>

using namespace Frenchie;
using namespace Frenchie::Application;
using namespace Frenchie::Application::Editor;
using namespace Frenchie::Application::Editor::FileSystem;
using namespace Frenchie::Application::Editor::Preferences;
using namespace Frenchie::Application::Editor::Configuration;

Languages::Languages() : 
    Frenchie::Application::Layer::Registry<Languages>(
        Translator::translate("FRENCHIE_APPLICATION_EDITOR_PREFERENCES_LANGUAGE")){}
Languages::~Languages(){}

std::string Languages::factory_id()
{
    return STRINGIFY(Frenchie::Application::Editor::Preferences::Languagess);
}

void Languages::frame_update()
{
    set_name(Translator::translate("FRENCHIE_APPLICATION_EDITOR_PREFERENCES_LANGUAGE"));

    auto localizator = 
        Frenchie::Application::Application::instance()->find_or_push<Translator>();

    ImGui::BeginChild(get_name().c_str());
    {
        // draw Languages selector
        for(auto&& supportedLanguage : localizator->get_supported_languages())
        {
            bool isCurrentLanguages = 
                supportedLanguage == localizator->get_current_language();

            if(ImGui::Checkbox(supportedLanguage.c_str(), &isCurrentLanguages))
                localizator->set_language(supportedLanguage);
        }

        // draw localization keys (table)
        if(ImGui::BeginTable("FileSystemContentTable", 2,
            ImGuiTableFlags_::ImGuiTableFlags_ScrollY      | 
            ImGuiTableFlags_::ImGuiTableFlags_RowBg        | 
            ImGuiTableFlags_::ImGuiTableFlags_BordersOuter | 
            ImGuiTableFlags_::ImGuiTableFlags_BordersV     |
            ImGuiTableFlags_::ImGuiTableFlags_Resizable    |
            ImGuiTableFlags_::ImGuiTableFlags_Reorderable  |
            ImGuiTableFlags_::ImGuiTableFlags_Hideable))
        {
            // setup columns
            ImGui::TableSetupColumn(
                Translator::translate("FRENCHIE_APPLICATION_EDITOR_PREFERENCES_LANGUAGE_TABLE_KEY").c_str(),
                ImGuiTableColumnFlags_::ImGuiTableColumnFlags_WidthStretch |
                ImGuiTableColumnFlags_::ImGuiTableColumnFlags_PreferSortAscending);

            ImGui::TableSetupColumn(
                Translator::translate("FRENCHIE_APPLICATION_EDITOR_PREFERENCES_LANGUAGE_TABLE_TRANSLATE").c_str(),
                ImGuiTableColumnFlags_::ImGuiTableColumnFlags_WidthStretch);
            
            ImGui::TableHeadersRow();

            int id = 0;
            for(auto&& translate : localizator->get_translations())
            {
                ImGui::TableNextRow();

                ImGui::TableSetColumnIndex(0);
                ImGui::TextUnformatted(translate.first.c_str());
                ImGui::SameLine();

                ImGui::TableSetColumnIndex(1);
                ImGui::PushID(++id);
                ImGui::InputText("##", &translate.second);
                ImGui::PopID();
            }

            ImGui::EndTable();
        }
    }
    ImGui::EndChild();
}

bool Languages::allows_multiple_instances() const
{
    return false;
}