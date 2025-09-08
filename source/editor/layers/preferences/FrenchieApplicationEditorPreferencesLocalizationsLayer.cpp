#include <FrenchieApplicationEditorPreferencesLocalizationsLayer.hpp>

#include <FrenchieApplicationEditorConfigurationLocalizatorLayer.hpp>
#include <FrenchieApplicationEditorFileSystemExplorerLayer.hpp>

#include <imgui_stdlib.h>

using namespace Frenchie;
using namespace Frenchie::Application;
using namespace Frenchie::Application::Editor;
using namespace Frenchie::Application::Editor::FileSystem;
using namespace Frenchie::Application::Editor::Preferences;
using namespace Frenchie::Application::Editor::Configuration;

Localizations::Localizations() : 
    Frenchie::Application::Layer::Registry<Localizations>(
        Localizator::translation("FRENCHIE_APPLICATION_EDITOR_PREFERENCES_TOPIC_LOCALIZATIONS_NAME_LOCALIZATION_KEY")){}
Localizations::~Localizations(){}

std::string Localizations::factory_id()
{
    return STRINGIFY(Frenchie::Application::Editor::Preferences::Localizations);
}

void Localizations::frame_update()
{
    set_name(Localizator::translation("FRENCHIE_APPLICATION_EDITOR_PREFERENCES_TOPIC_LOCALIZATIONS_NAME_LOCALIZATION_KEY"));

    auto localizator = 
        Frenchie::Application::Application::instance()->find_or_push<Localizator>();

    ImGui::BeginChild(get_name().c_str());
    {
        // draw language selector
        for(auto&& supportedLanguage : localizator->get_supported_languages())
        {
            bool isCurrentLanguage = 
                supportedLanguage == localizator->get_current_language();

            if(ImGui::Checkbox(supportedLanguage.c_str(), &isCurrentLanguage))
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
                Localizator::translation("FRENCHIE_APPLICATION_EDITOR_PREFERENCES_LOCALIZATIONS_TABLE_KEY_LOCALIZATION_KEY").c_str(),
                ImGuiTableColumnFlags_::ImGuiTableColumnFlags_WidthStretch |
                ImGuiTableColumnFlags_::ImGuiTableColumnFlags_PreferSortAscending);

            ImGui::TableSetupColumn(
                Localizator::translation("FRENCHIE_APPLICATION_EDITOR_PREFERENCES_LOCALIZATIONS_TABLE_TRANSLATION_LOCALIZATION_KEY").c_str(),
                ImGuiTableColumnFlags_::ImGuiTableColumnFlags_WidthStretch);
            
            ImGui::TableHeadersRow();

            int id = 0;
            for(auto&& translation : localizator->get_translations())
            {
                ImGui::TableNextRow();

                ImGui::TableSetColumnIndex(0);
                ImGui::TextUnformatted(translation.first.c_str());
                ImGui::SameLine();

                ImGui::TableSetColumnIndex(1);
                ImGui::PushID(++id);
                ImGui::InputText("##", &translation.second);
                ImGui::PopID();
            }

            ImGui::EndTable();
        }
    }
    ImGui::EndChild();
}

bool Localizations::allows_multiple_instances() const
{
    return false;
}