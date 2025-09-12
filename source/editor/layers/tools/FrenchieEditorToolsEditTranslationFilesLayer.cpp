#include <FrenchieEditorToolsEditTranslationFilesLayer.hpp>

// Application
#include <FrenchieApplication.hpp>
#include <FrenchieApplicationCommandsLayer.hpp>
#include <FrenchieApplicationAsynchronousProcessesLayer.hpp>

// Editor
#include <FrenchieEditorFileSystemExplorerDialogLayer.hpp>

// IMGUI
#include <imgui.h>
#include <imgui_stdlib.h>

// STL
#include <chrono>
#include <regex>
#include <fstream>
#include <iostream>
#include <functional>

using namespace Frenchie::Editor;
using namespace Frenchie::Editor::Tools;
using namespace Frenchie::Editor::Configuration;

// Add to main menu
namespace Frenchie
{
    namespace Editor
    {
        namespace MainMenu
        {
            class OpenTranslationFilesEditorToolAction : 
                public Frenchie::Application::Command::Registry<OpenTranslationFilesEditorToolAction, void*>
            {
            public:

                OpenTranslationFilesEditorToolAction(void* _Sender = nullptr) : 
                    Frenchie::Application::Command::Registry<OpenTranslationFilesEditorToolAction, void*>(_Sender){}
                virtual ~OpenTranslationFilesEditorToolAction(){}

                // Frenchie::Application::Command
                virtual void execute() override
                {
                    Frenchie::Application::application()->push_layer<TranslationFilesEditor>()->show();
                }

                // Command::TRegistryType
                static std::string factory_id()
                {
                    return fmt::format("{}::{}", STRINGIFY(Frenchie::Editor::MainMenu), "Tools::Translation files::Translation files editor");
                }
            };
        }
    }
}

// TranslationFilesEditor
TranslationFilesEditor::TranslationFilesEditor() : 
    Layer(Translator::translate("Translation files editor")){}

TranslationFilesEditor::~TranslationFilesEditor(){}

void TranslationFilesEditor::frame_update()
{
    // lock mutex to guarantee safe access to process data
    std::lock_guard<std::mutex> lock(m_Mutex);

    // update name
    set_name(Translator::translate("Translation files editor"));

    // draw
    ImGui::Begin(fmt::format("{}###Translation files editor", get_name()).c_str(), &m_Opened);

    // draw 'Toolbar' panel
    ImGui::SeparatorText(Translator::translate("Toolbar").c_str());
    {
        if(ImGui::Button(Translator::translate("Load files").c_str())) 
        {
            try_execute_command([this]()
            {
                Frenchie::Application::application()->push_layer<FileSystem::ExplorerDialog>(
                    Translator::translate("Select directory where translation files are..."))
                    ->on_accepted(
                    [this]()
                    {
                        m_LoadProcess = 
                            Frenchie::Application::ProcessQueue::instance()->push<LoadTranslationFilesProcess>(
                                Frenchie::Application::application()->find_layer<FileSystem::ExplorerDialog>()->get_selected_paths());
                    }
                );
            }, "Load files");
        }

        ImGui::SameLine();

        if(ImGui::Button(Translator::translate("Save files").c_str())) 
        {
            try_execute_command([this]()
            {
                if(m_LoadProcess != nullptr)
                {
                    m_SaveProcess = 
                        Frenchie::Application::ProcessQueue::instance()->push<SaveTranslationFilesProcess>(
                            m_LoadProcess->m_TranslationFiles);
                }
            }, 
            "Save files");
        }

        ImGui::SameLine();

        if(ImGui::Button(Translator::translate("Close files").c_str()))
        {
            try_execute_command([this]()
            {
                m_SaveProcess.reset();
                m_LoadProcess.reset();
            }, 
            "Close files");
        }
    }

    ImGui::SeparatorText(Translator::translate("Files").c_str());
    {
        if(m_LoadProcess != nullptr && 
                m_LoadProcess->finished()) // draw table on finished
        {
            for(auto&& translationFile : m_LoadProcess->m_TranslationFiles)
            {
                auto& path         = translationFile.Path;
                auto& translations = translationFile.Translations;
                int   widgetID     = 0;

                // draw file content tree node
                ImGui::PushID(++widgetID);

                if(ImGui::TreeNode(path.string().empty() ? Translator::translate("empty path").c_str() : path.string().c_str()))
                {
                    // draw file 'Save' button
                    if(ImGui::Button(Translator::translate("Save").c_str()))
                    {
                        try_execute_command([this, &translationFile]()
                        {
                            m_SaveProcess = Frenchie::Application::ProcessQueue::instance()->push<SaveTranslationFilesProcess>(
                            std::vector<TranslationFile>({translationFile}));
                        }, "Save");
                    }

                    ImGui::SameLine();

                    // draw file 'Save as' button
                    if(ImGui::Button(Translator::translate("Save as").c_str()))
                    {
                        try_execute_command([this, &translationFile]()
                        {
                            Frenchie::Application::application()->push_layer<FileSystem::ExplorerDialog>(
                                Translator::translate("Save file"))->on_accepted(
                                [this, &translationFile]()
                                {
                                    auto dialog = Frenchie::Application::application()->find_layer<FileSystem::ExplorerDialog>();

                                    if(dialog == nullptr) 
                                        return;

                                    translationFile.Path = dialog->get_current_file();

                                    m_SaveProcess = 
                                        Frenchie::Application::ProcessQueue::instance()->push<SaveTranslationFilesProcess>(
                                            std::vector<TranslationFile>({translationFile}));
                                }
                            );
                        }, "Save as");
                    }

                    ImGui::SameLine();

                    // draw button 'Use all as is'
                    if(ImGui::Button(Translator::translate("Use all as is").c_str()))
                    {
                        try_execute_command([this, &translations]()
                        {
                            for(auto&& translation : translations)
                                translation.Value = translation.Key;                      
                        }, 
                        "Use all as is");
                    }

                    ImGui::SameLine();

                    // draw file 'Clear' button
                    if(ImGui::Button(Translator::translate("Clear").c_str()))
                    {
                        try_execute_command([this, &translationFile]()
                        {
                            if(!translationFile.Translations.empty())
                                translationFile.Translations.clear();
                        }, 
                        "Clear");
                    }

                    ImGui::SameLine();

                    // draw file 'Close' button
                    if(ImGui::Button(Translator::translate("Close").c_str()))
                    {
                        try_execute_command([this, &translationFile]()
                        {
                            for(auto it = m_LoadProcess->m_TranslationFiles.begin(); it != m_LoadProcess->m_TranslationFiles.end(); it++)
                            {
                                if(translationFile.Path == it->Path)
                                {
                                    m_LoadProcess->m_TranslationFiles.erase(it);
                                    break;
                                }
                            }                                    
                        }, 
                        "Close");
                    }

                    ImGui::SameLine();

                    // draw 'Add translation unit' button
                    if(ImGui::Button(Translator::translate("Add translation unit").c_str()))
                        m_TranslationUnitsToAdd.push_back({"NEW_KEY", "NEY_VALUE"});

                    if(!m_TranslationUnitsToAdd.empty())
                    {
                        ImGui::SameLine();

                        // draw 'Insert translation units' button
                        if(ImGui::Button(Translator::translate("Insert translation units").c_str()))
                        {
                            try_execute_command(
                                [this, &translations]()
                                {
                                    for(auto&& newKey : m_TranslationUnitsToAdd)
                                        translations.insert({newKey.Key, newKey.Value});

                                    m_TranslationUnitsToAdd.clear();
                                }, 
                                "Insert translation units");
                        }

                        for(auto&& newKey : m_TranslationUnitsToAdd)
                        {
                            auto width = ImGui::GetContentRegionAvail().x;

                            ImGui::SetNextItemWidth(0.05f * width);
                            ImGui::PushID(++widgetID);
                            ImGui::Checkbox("##", &newKey.Selected);
                            ImGui::PopID();

                            ImGui::SameLine();

                            ImGui::SetNextItemWidth(0.45f * width);
                            ImGui::PushID(++widgetID);
                            ImGui::InputText("##", &newKey.Key);
                            ImGui::PopID();

                            ImGui::SameLine();

                            ImGui::SetNextItemWidth(0.45f * width);
                            ImGui::PushID(++widgetID);
                            ImGui::InputText("##", &newKey.Value);
                            ImGui::PopID();

                            ImGui::SetNextItemWidth(ImGui::GetContentRegionAvail().x);
                            ImGui::SameLine();
                            ImGui::PushID(++widgetID);
                            if(ImGui::Button(Translator::translate("Remove").c_str()))
                            {
                                try_execute_command(
                                    [this, newKey]()
                                    {
                                        for(auto it = m_TranslationUnitsToAdd.begin(); it != m_TranslationUnitsToAdd.end(); it++)
                                        {
                                            if(it->Key == newKey.Key)
                                            {
                                                m_TranslationUnitsToAdd.erase(it);
                                                break;
                                            }
                                        }
                                    }, 
                                    "Remove");
                            }
                            ImGui::PopID();
                        }
                    }

                    auto filterTitle = Translator::translate("Key filter");
                    ImGui::SetNextItemWidth(ImGui::GetContentRegionAvail().x - ImGui::CalcTextSize(filterTitle.c_str()).x);
                    m_TextFilter.Draw(filterTitle.c_str());

                    // draw localization keys (table)
                    if(ImGui::BeginTable("FileSystemContentTable", 
                        2,
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
                            Translator::translate("Key").c_str(),
                            ImGuiTableColumnFlags_::ImGuiTableColumnFlags_WidthStretch |
                            ImGuiTableColumnFlags_::ImGuiTableColumnFlags_PreferSortAscending);

                        ImGui::TableSetupColumn(
                            Translator::translate("Translation").c_str(),
                            ImGuiTableColumnFlags_::ImGuiTableColumnFlags_WidthStretch);
                        
                        ImGui::TableHeadersRow();

                        for(auto&& translation : translations)
                        {
                            if(!m_TextFilter.PassFilter(translation.Key.c_str())) 
                                continue;

                            if(translation.Value.empty())
                                ImGui::PushStyleColor(ImGuiCol_Text, IM_COL32(200, 0, 0, 255));
                            else 
                                ImGui::PushStyleColor(ImGuiCol_Text, IM_COL32(0, 200, 0, 255));

                            ImGui::TableNextRow();

                            ImGui::TableSetColumnIndex(0);
                            ImGui::TextUnformatted(translation.Key.c_str());
                            ImGui::SameLine();

                            ImGui::PopStyleColor();

                            ImGui::TableSetColumnIndex(1);
                            ImGui::PushID(++widgetID);
                            ImGui::InputText("##", &translation.Value);
                            ImGui::PopID();

                            ImGui::SameLine();

                            ImVec2 size = ImVec2(ImGui::GetContentRegionAvail().x * 0.5f, 0.f);

                            ImGui::PushID(++widgetID);
                            if(ImGui::Button(Translator::translate("Use as is").c_str(), size))
                            {
                                try_execute_command([&translation]()
                                {
                                    translation.Value = translation.Key;
                                }, 
                                "Use as is");
                            }
                            ImGui::PopID();

                            ImGui::SameLine();

                            ImGui::PushID(++widgetID);
                            if(ImGui::Button(Translator::translate("Remove").c_str(), size))
                            {
                                try_execute_command([&translations, &translation]()
                                {
                                    Frenchie::Application::CommandsQueue::instance()->push<Frenchie::Application::CallbackCommand>(
                                        [&translations, &translation](){translations.erase(translation);});
                                }, 
                                "Remove");
                            }
                            ImGui::PopID();
                        }

                        ImGui::EndTable();
                    }

                    ImGui::TreePop();
                }
            
                ImGui::PopID();
            }   
        }
        else // draw progress indicator
        {
            if(m_LoadProcess != nullptr && !m_LoadProcess->failed() && !m_LoadProcess->canceled() && !m_LoadProcess->canceled() && !m_LoadProcess->finished()) 
                ImGui::TextUnformatted(Translator::translate("Loading...").c_str());
        }
    }
    
    ImGui::End();
}

void TranslationFilesEditor::finish()
{
}

bool TranslationFilesEditor::allows_multiple_instances() const
{
    return false;
}

void TranslationFilesEditor::try_execute_command(std::function<void()> _Function, const std::string& _Name)
{
    if(m_LoadProcess != nullptr && !m_LoadProcess->finished() && !m_LoadProcess->canceled())
    {
        Frenchie::Core::Logger::instance()->error("{}: cannot '{}' as someting is loading...", get_name(), _Name);
        return;
    }

    if(m_SaveProcess != nullptr && !m_SaveProcess->finished() && !m_SaveProcess->canceled())
    {
        Frenchie::Core::Logger::instance()->error("{}: cannot '{}' as someting is saving...", get_name(), _Name);
        return;
    }

    // execute
    if(_Function != nullptr)
        _Function();
}