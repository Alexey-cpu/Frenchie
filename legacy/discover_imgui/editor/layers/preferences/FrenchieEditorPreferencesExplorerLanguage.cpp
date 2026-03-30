#include <FrenchieEditorPreferencesExplorerLanguage.hpp>

// Core
#include <FrenchieCoreFileSystem.hpp>

// Application
#include <FrenchieApplication.hpp>

// Editor
#include <FrenchieApplicationConfigurationLoaderTranslatorLayer.hpp>
#include <FrenchieEditorFileSystemExplorerDialog.hpp>
#include <FrenchieEditorFileSystemExplorerPathScannerDialog.hpp>
#include <FrenchieApplicationEditorDialog.hpp>

// IMGUI
#include <imgui.h>
#include <imgui_stdlib.h>

// STL
#include <chrono>
#include <regex>
#include <fstream>
#include <iostream>
#include <functional>

using namespace Frenchie::Application;
using namespace Frenchie::Application::Configuration;

using namespace Frenchie::Editor;
using namespace Frenchie::Editor::FileSystem;
using namespace Frenchie::Editor::Preferences;

// Add to main menu
namespace Frenchie
{
    namespace Editor
    {
        namespace MainMenu
        {
            class OpenLanguageSettingsAction : 
                public Frenchie::Application::Command::Registry<OpenLanguageSettingsAction, const CommandPayloads&>
            {
            public:

                OpenLanguageSettingsAction(const CommandPayloads& _Sender = CommandPayloads()) : 
                    Frenchie::Application::Command::Registry<OpenLanguageSettingsAction, const CommandPayloads&>(_Sender){}
                
                virtual ~OpenLanguageSettingsAction(){}

                // Frenchie::Application::Command
                virtual void execute() override
                {
                    Frenchie::Application::application()->push_layer<Frenchie::Editor::Preferences::Languages>()->show();
                }

                // Command::TRegistryType
                static std::string factory_id()
                {
                    return fmt::format("{}::{}", STRINGIFY(Frenchie::Editor::MainMenu), "Windows::Preferences::Language settings");
                }
            };

            const bool openLanguageSettingsAction = OpenLanguageSettingsAction::registerFactory();
        }
    }
}

namespace Frenchie
{
    namespace Editor
    {
        namespace Preferences
        {
            class RenameKeysDialog : public Dialog
            {
            public:
                RenameKeysDialog(
                    Frenchie::Core::Reference<Frenchie::Application::Configuration::Language> _Language, 
                    const std::vector<Frenchie::Application::Configuration::TranslationUnit>& _Units) : 
                    Dialog(translator()->translate("RenameKeys")),
                    m_Language(_Language),
                    m_Cache(_Units),
                    m_Units(_Units){}

                virtual ~RenameKeysDialog(){}

                // Frenchie::Application::Editor::Dialog
                virtual void draw_content() override
                {
                    if(ImGui::BeginTable("New translation keys", 2, ImGuiTableFlags_::ImGuiTableFlags_Resizable))
                    {
                        // setup columns
                        ImGui::TableSetupColumn(translator()->translate("Key").c_str(), 
                            ImGuiTableColumnFlags_::ImGuiTableColumnFlags_WidthStretch |
                            ImGuiTableColumnFlags_::ImGuiTableColumnFlags_PreferSortAscending);
                        
                        ImGui::TableSetupColumn(translator()->translate("Value").c_str(), 
                            ImGuiTableColumnFlags_::ImGuiTableColumnFlags_WidthFixed |
                            ImGuiTableColumnFlags_::ImGuiTableColumnFlags_PreferSortAscending);
                        
                        ImGui::TableHeadersRow();

                        int id = 0;

                        for(auto&& newKey : m_Units)
                        {
                            ImGui::TableNextRow();

                            ImGui::TableSetColumnIndex(0);

                            ImGui::SetNextItemWidth(ImGui::GetContentRegionAvail().x);
                            ImGui::PushID(++id);
                            ImGui::InputText("##", &newKey.Key);
                            ImGui::PopID();

                            ImGui::TableSetColumnIndex(1);

                            ImGui::PushID(++id);
                            ImGui::InputText("##", &newKey.Value);
                            ImGui::PopID();
                        }

                        ImGui::EndTable();
                    }
                }

                virtual void draw_buttons() override
                {
                    if(ImGui::Button(translator()->translate("Ok").c_str()))
                    {
                        m_Accepted = true;
                        close();
                    }
                    
                    ImGui::SameLine();
                    
                    if(ImGui::Button(translator()->translate("Cancel").c_str()))
                        close();
                }

                virtual void finish() override
                {
                    if(m_Language == nullptr || !m_Accepted) 
                        return;

                    // erase cached values
                    for(auto&& translation : m_Cache)
                        m_Language->get_translation_file().Translations.erase(translation);

                    // insert new values
                    for(auto&& translation : m_Units)
                        m_Language->get_translation_file().Translations.insert(translation);
                }

            protected:

                bool m_Accepted{false};

                Frenchie::Core::Reference<Frenchie::Application::Configuration::Language> m_Language;

                std::vector<Frenchie::Application::Configuration::TranslationUnit> m_Cache;
                std::vector<Frenchie::Application::Configuration::TranslationUnit> m_Units;
            };
        }
    }
}

Languages::Languages() : Frenchie::Application::Layer(translator()->translate("Language settings")){}
Languages::~Languages(){}

void Languages::frame_update()
{
    // update name
    set_name(translator()->translate("Language settings"));

    // draw
    ImGui::Begin(fmt::format("{}###Language settings", get_name()).c_str(), &m_Opened);
    {
        ImGui::SeparatorText(translator()->translate("Translation file").c_str());

        // draw combobox
        auto supportedLanguages = translator()->get_supported_languages();
        auto currentLanguage    = translator()->get_current_language();
        int  id                 = 0;

        if(ImGui::BeginCombo(
            "##", 
            (currentLanguage != nullptr ? currentLanguage->get_name().c_str() : translator()->translate("There are no translation files loaded...").c_str())))
        {
            for(auto&& supportedLanguage : supportedLanguages)
            {
                ImGui::PushID(++id);
                bool selected = supportedLanguage->is_current();
                
                if(ImGui::Checkbox(supportedLanguage->get_name().c_str(), &selected))
                {
                    // setup current language
                    Frenchie::Application::application_command_queue()
                        ->push<Frenchie::Application::CallbackCommand>(
                            [this, supportedLanguage]()
                            {
                                if(supportedLanguage != nullptr)
                                    supportedLanguage->setup();
                            }
                        );

                    // close current popup
                    ImGui::CloseCurrentPopup();
                }

                ImGui::SetItemTooltip(Frenchie::Core::String::convert_utf32_to_utf8(supportedLanguage->get_path().u32string()).c_str());

                ImGui::PopID();
            }

            ImGui::EndCombo();
        }

        // draw buttons
        ImGui::SameLine();
        
        if(ImGui::Button(translator()->translate("Browse").c_str()))
        {
            auto dialog = Frenchie::Application::application()->push_layer<FileSystem::PathScannerDialog>(
                [](const std::filesystem::path& _Entry)->bool
                {
                    return !std::filesystem::is_directory(_Entry) && 
                        Frenchie::Core::FileSystem::get_file_extention(_Entry) == ".xlf";
                },
                translator()->translate("Select directory where to look for translation files ...")
            );

            dialog->on_accepted(
                [this](const std::map<std::filesystem::path, bool>& _Paths)
                {
                    std::set<std::filesystem::path> paths;

                    for(auto&& entry : _Paths)
                    {
                        if(entry.second)
                            paths.insert(entry.first);
                    }

                    translator()->set_supported_languages(paths);
                }
            );
        }

        ImGui::SameLine();

        if(ImGui::Button(translator()->translate("Create new").c_str()))
        {
            auto dialog = Frenchie::Application::application()->push_layer<FileSystem::ExplorerDialog>(
                translator()->translate("Select folder where to save new translation file...")
            );

            dialog->on_accepted(
                [this]()
                {
                    Frenchie::Core::Reference<FileSystem::ExplorerDialog> dialog = 
                        Frenchie::Application::application()->find_layer<FileSystem::ExplorerDialog>();

                    if(dialog == nullptr) 
                        return;
                    
                    auto language = translator()->create_new_translation_file(dialog->get_current_file());
                    language->setup();
                    language->save();
                }
            );
        }

        ImGui::SameLine();
        
        if(ImGui::Button(translator()->translate("Remove all").c_str()))
        {
            Frenchie::Application::application_command_queue()->push<Frenchie::Application::CallbackCommand>(
                [this]()
                {
                    auto currentLanguage = translator()->get_current_language();

                    if(currentLanguage != nullptr)
                        currentLanguage->get_translation_file().Translations.clear();
                }
            );
        }

        ImGui::SameLine();
        
        if(ImGui::Button(translator()->translate("Clear all").c_str()))
        {
            Frenchie::Application::application_command_queue()->push<Frenchie::Application::CallbackCommand>(
                [this]()
                {
                    auto currentLanguage = translator()->get_current_language();

                    if(currentLanguage != nullptr)
                    {
                        for(auto&& translation : currentLanguage->get_translation_file().Translations)
                            translation.Value = std::string();
                    }
                }
            );
        }

        ImGui::SameLine();
        
        if(ImGui::Button(translator()->translate("Save").c_str()))
        {
            Frenchie::Application::application_command_queue()->push<Frenchie::Application::CallbackCommand>(
                [this]()
                {
                    auto currentLanguage = 
                        translator()->get_current_language();

                    if(currentLanguage != nullptr)
                        currentLanguage->save();
                }
            );
        }

        // m_NewKeys
        ImGui::SeparatorText(translator()->translate("New localization keys").c_str());

        ImGui::PushID(++id);
        if(ImGui::Button(translator()->translate("Add").c_str()))
        {
            Frenchie::Application::application_command_queue()->push<Frenchie::Application::CallbackCommand>(
                [this]()
                {
                    m_NewKeys.push_back(Frenchie::Application::Configuration::TranslationUnit());
                }
            );
        }
        ImGui::PopID();

        ImGui::SameLine();

        ImGui::PushID(++id);
        if(ImGui::Button(translator()->translate("Clear all").c_str()))
        {
            Frenchie::Application::application_command_queue()->push<Frenchie::Application::CallbackCommand>(
                [this]()
                {
                    for(auto&& newKey : m_NewKeys)
                        newKey.Value = std::string();
                }
            );
        }
        ImGui::PopID();

        ImGui::SameLine();

        ImGui::PushID(++id);
        if(ImGui::Button(translator()->translate("Remove all").c_str()))
        {
            Frenchie::Application::application_command_queue()->push<Frenchie::Application::CallbackCommand>(
                [this]()
                {
                    m_NewKeys.clear();
                }
            );
        }
        ImGui::PopID();
        
        ImGui::SameLine();

        ImGui::PushID(++id);
        if(ImGui::Button(translator()->translate("Insert in file").c_str()))
        {
            Frenchie::Application::application_command_queue()->push<Frenchie::Application::CallbackCommand>(
                [this]()
                {
                    auto currentLanguage  = translator()->get_current_language();

                    if(currentLanguage == nullptr) 
                        return;

                    for(auto&& newKey : m_NewKeys)
                    {
                        currentLanguage->get_translation_file().Translations.insert(
                            {
                                newKey.Key,
                                newKey.Value
                            }
                        );
                    }

                    m_NewKeys.clear();
                }
            );
        }
        ImGui::PopID();

        if(!m_NewKeys.empty())
        {
            if(ImGui::BeginTable("New translation keys", 2, ImGuiTableFlags_::ImGuiTableFlags_Resizable))
            {
                // setup columns
                ImGui::TableSetupColumn(translator()->translate("Key").c_str(), 
                    ImGuiTableColumnFlags_::ImGuiTableColumnFlags_WidthStretch |
                    ImGuiTableColumnFlags_::ImGuiTableColumnFlags_PreferSortAscending);
                
                ImGui::TableSetupColumn(translator()->translate("Value").c_str(), 
                    ImGuiTableColumnFlags_::ImGuiTableColumnFlags_WidthFixed |
                    ImGuiTableColumnFlags_::ImGuiTableColumnFlags_PreferSortAscending);
                
                ImGui::TableHeadersRow();

                for(auto&& newKey : m_NewKeys)
                {
                    ImGui::TableNextRow();

                    ImGui::TableSetColumnIndex(0);

                    ImGui::SetNextItemWidth(ImGui::GetContentRegionAvail().x);
                    ImGui::PushID(++id);
                    ImGui::InputText("##", &newKey.Key);
                    ImGui::PopID();

                    ImGui::TableSetColumnIndex(1);

                    ImGui::PushID(++id);
                    ImGui::InputText("##", &newKey.Value);
                    ImGui::PopID();

                    ImGui::SameLine();

                    ImGui::PushID(++id);
                    if(ImGui::Button(translator()->translate("Remove").c_str(), ImVec2(ImGui::GetContentRegionAvail().x, 0.f)))
                    {
                        Frenchie::Application::application_command_queue()->push<Frenchie::Application::CallbackCommand>(
                            [this, newKey]()
                            {
                                auto iterator = 
                                    std::find_if(
                                        m_NewKeys.begin(), 
                                        m_NewKeys.end(), 
                                        [newKey](const Frenchie::Application::Configuration::TranslationUnit& _Unit)->bool
                                        {
                                            return newKey.Key == _Unit.Key;
                                        }
                                    );

                                if(iterator != m_NewKeys.end()) 
                                    m_NewKeys.erase(iterator);
                            }
                        );
                    }
                    ImGui::PopID();
                }

                ImGui::EndTable();
            }
        }
        
        ImGui::SeparatorText(translator()->translate("Translation file contents").c_str());

        ImGui::Checkbox(translator()->translate("Show column filters").c_str(), &m_ShowFilters);

        if(ImGui::BeginTable("TranslationFile",
            3,
            ImGuiTableFlags_::ImGuiTableFlags_ScrollY      | 
            ImGuiTableFlags_::ImGuiTableFlags_RowBg        | 
            ImGuiTableFlags_::ImGuiTableFlags_BordersOuter | 
            ImGuiTableFlags_::ImGuiTableFlags_BordersV     |
            ImGuiTableFlags_::ImGuiTableFlags_Resizable    |
            ImGuiTableFlags_::ImGuiTableFlags_Reorderable  |
            ImGuiTableFlags_::ImGuiTableFlags_Hideable
        )
    )
        {
            // setup columns
            ImGui::TableSetupColumn(translator()->translate("Key").c_str(), 
                ImGuiTableColumnFlags_::ImGuiTableColumnFlags_WidthStretch |
                ImGuiTableColumnFlags_::ImGuiTableColumnFlags_PreferSortAscending);
            
            ImGui::TableSetupColumn(translator()->translate("Buttons").c_str(), 
                ImGuiTableColumnFlags_::ImGuiTableColumnFlags_WidthFixed |
                ImGuiTableColumnFlags_::ImGuiTableColumnFlags_PreferSortAscending);

            ImGui::TableSetupColumn(translator()->translate("Value").c_str(), 
                ImGuiTableColumnFlags_::ImGuiTableColumnFlags_WidthFixed |
                ImGuiTableColumnFlags_::ImGuiTableColumnFlags_PreferSortAscending);
            
            ImGui::TableHeadersRow();

            // filters
            if(m_ShowFilters)
            {
                ImGui::TableNextRow();

                ImGui::TableSetColumnIndex(0);

                ImGui::PushID(++id);
                m_KeyFilter.Draw("##", ImGui::GetContentRegionAvail().x);
                ImGui::PopID();

                ImGui::PushID(++id);
                ImGui::TableSetColumnIndex(1);
                auto nonFilterable = translator()->translate("This column can't be filtered...");
                ImGui::SetNextItemWidth(ImGui::GetContentRegionAvail().x);
                ImGui::InputText("##", &nonFilterable, ImGuiInputTextFlags_::ImGuiInputTextFlags_ReadOnly);
                ImGui::PopID();

                ImGui::TableSetColumnIndex(2);

                ImGui::PushID(++id);
                m_ValueFilter.Draw("##", ImGui::GetContentRegionAvail().x);
                ImGui::PopID();
            }

            if(currentLanguage != nullptr)
            {
                for(auto&& translation : currentLanguage->get_translation_file().Translations)
                {
                    if(!m_KeyFilter.PassFilter(translation.Key.c_str()) || 
                        !m_ValueFilter.PassFilter(translation.Value.c_str())) continue;

                    if(translation.Key.empty()) 
                    {
                        ImGui::PushStyleColor(ImGuiCol_::ImGuiCol_Text, IM_COL32(255, 0, 0, 255));
                        ImGui::PushStyleColor(ImGuiCol_::ImGuiCol_TextDisabled, IM_COL32(255, 0, 0, 255));
                        ImGui::PushStyleColor(ImGuiCol_::ImGuiCol_TextSelectedBg, IM_COL32(255, 0, 0, 255));
                        ImGui::PushStyleColor(ImGuiCol_::ImGuiCol_TextLink, IM_COL32(255, 0, 0, 255));
                    }
                    else
                    {
                        ImGui::PushStyleColor(ImGuiCol_::ImGuiCol_Text, IM_COL32(0, 255, 0, 255));
                        ImGui::PushStyleColor(ImGuiCol_::ImGuiCol_TextDisabled, IM_COL32(0, 255, 0, 255));
                        ImGui::PushStyleColor(ImGuiCol_::ImGuiCol_TextSelectedBg, IM_COL32(0, 255, 0, 255));
                        ImGui::PushStyleColor(ImGuiCol_::ImGuiCol_TextLink, IM_COL32(0, 255, 0, 255));
                    }

                    ImGui::TableNextRow();

                    // draw key
                    ImGui::TableSetColumnIndex(0);
                    ImGui::PushID(++id);
                    ImGui::TextUnformatted(translation.Key.c_str());
                    ImGui::PopID();

                    // draw buttons
                    ImGui::TableSetColumnIndex(1);
                    ImGui::PushID(++id);
                    auto buttonWidth = ImGui::GetContentRegionAvail().x * 0.5f;

                    if(ImGui::Button(translator()->translate("Remove").c_str(), ImVec2(buttonWidth, 0.f)))
                    {
                        Frenchie::Application::application_command_queue()->push<Frenchie::Application::CallbackCommand>(
                            [this, translation]()
                            {
                                auto currentLanguage = translator()->get_current_language();

                                if(currentLanguage != nullptr)
                                    currentLanguage->get_translation_file().Translations.erase(translation);
                            }
                        );
                    }
                    ImGui::PopID();

                    ImGui::SameLine();

                    ImGui::PushID(++id);
                    if(ImGui::Button(translator()->translate("Rename key").c_str(), ImVec2(buttonWidth, 0.f)))
                    {
                        Frenchie::Application::application_command_queue()->push<Frenchie::Application::CallbackCommand>(
                            [this, translation]()
                            {
                                Frenchie::Application::application()->push_layer<Frenchie::Editor::Preferences::RenameKeysDialog>(
                                    translator()->get_current_language(),
                                    std::vector<Frenchie::Application::Configuration::TranslationUnit>({translation})
                                );
                            }
                        );
                    }
                    ImGui::PopID();

                    // draw value
                    ImGui::TableSetColumnIndex(2);

                    ImGui::PushID(++id);
                    ImGui::InputText("##", &translation.Value);
                    ImGui::PopID();

                    ImGui::PopStyleColor(4);
                }
            }

            ImGui::EndTable();
        }
    }
    ImGui::End();
}

bool Languages::allows_multiple_instances() const
{
    return false;
}