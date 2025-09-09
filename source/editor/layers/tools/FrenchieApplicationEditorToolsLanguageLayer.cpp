#include <FrenchieApplicationEditorToolsLanguageLayer.hpp>

#include <FrenchieApplication.hpp>
#include <FrenchieApplicationEditorFileSystemExplorerLayer.hpp>
#include <FrenchieApplicationEditorAsyncProcessLayer.hpp>
#include <FrenchieApplicationEditorCommandsLayer.hpp>

// IMGUI
#include <imgui.h>
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
using namespace Frenchie::Application::Editor::Tools;
using namespace Frenchie::Application::Editor::Async;

// LoadTranslationFiles
LoadTranslationFilesProcess::LoadTranslationFilesProcess(
    const std::set<std::filesystem::path>& _Path,
    const std::function<void()>& _OnFinished,
    const std::function<void()>& _OnCanceled, 
    const std::function<void()>& _OnFailed) : 
    Process(_OnFinished, _OnCanceled, _OnFailed, STRINGIFY(LoadTranslationFilesProcess))
{
    for(auto&& path : _Path)
    {
        if(!std::filesystem::exists(path)       || 
            std::filesystem::is_directory(path) || 
            Frenchie::Core::FileSystem::get_file_extention(path) != ".xlf") 
        {
            continue;
        }

        m_TranslationFiles.insert(
            {
                path, 
                std::map<std::string, std::string>()
            }
        );
    }
}

LoadTranslationFilesProcess::~LoadTranslationFilesProcess(){}

bool LoadTranslationFilesProcess::awake() 
{
    // finish on empty
    if(m_TranslationFiles.empty())
    {
        m_Finished = true;
        return true;
    }

    Frenchie::Core::ThreadPool::instance()->enqueue(
        [this]()
        {
            size_t total    = m_TranslationFiles.size();
            size_t progress = 0;

            for(auto&& translationFile : m_TranslationFiles)
            {
                while(m_Paused)
                {
                    if(m_Canceled) 
                        return;
                }

                if(m_Canceled) 
                    return;

                std::this_thread::sleep_for(std::chrono::milliseconds(1000));

                auto& path         = translationFile.first;
                auto& translations = translationFile.second;

                // load XLIFF
                {
                    // update status
                    m_Status = m_Status.append(fmt::format("Trying to load: {}", path.string())).append("\n");

                    // load translations from .xlf file
                    Frenchie::Core::Serialization::Document document;

                    if(!document.read<Frenchie::Core::Serialization::XMLReader>(path)) 
                    {
                        // update progress
                        m_Progress = (float)(++progress) / (float)(total);
                        m_Status   = m_Status.append("could not load...").append("\n");
                        continue;
                    }

                    auto body = document.find_node("xliff").find_node("file").find_node("body");

                    for (auto item : body)
                    {
                        translations[item.find_node("source").get_value()] = 
                            item.find_node("target").get_value();
                    }

                    // update progress
                    m_Progress = (float)(++progress) / (float)(total);
                    m_Status   = m_Status.append("success").append("\n");
                }
            }

            // finishs
            m_Progress = (float)(total) / (float)(total);
            m_Status   = m_Status.append("completed").append("\n");
            m_Finished = true;
        }
    );

    return true;
}

std::string LoadTranslationFilesProcess::iprocess_status_request_status()
{
    std::unique_lock<std::mutex> lock(m_Mutex);
    return m_Status;
}

float LoadTranslationFilesProcess::iprocess_progress_request_progress()
{
    std::unique_lock<std::mutex> lock(m_Mutex);
    return m_Progress;
}

// SaveTranslationFiles
SaveTranslationFilesProcess::SaveTranslationFilesProcess(
    const std::map<std::filesystem::path, std::map<std::string, std::string>>& _Translations,
    const std::function<void()>& _OnFinished,
    const std::function<void()>& _OnCanceled, 
    const std::function<void()>& _OnFailed) :
    Process(_OnFinished, _OnCanceled, _OnFailed, STRINGIFY(SaveTranslationFilesProcess)), 
    m_TranslationFiles(_Translations){}

SaveTranslationFilesProcess::~SaveTranslationFilesProcess(){}

bool SaveTranslationFilesProcess::awake()
{
    if(m_TranslationFiles.empty())
    {
        m_Finished = true;
        return true;
    }

    Frenchie::Core::ThreadPool::instance()->enqueue(
        [this]()
        {
            size_t total    = m_TranslationFiles.size();
            size_t progress = 0;

            for(auto&& translationFile : m_TranslationFiles)
            {
                while(m_Paused)
                {
                    if(m_Canceled) 
                        return;
                }

                if(m_Canceled) 
                    return;

                auto& path         = translationFile.first;
                auto& translations = translationFile.second;

                m_Status   = m_Status.append(fmt::format("starting saving file {}", path.string())).append("\n");

                Frenchie::Core::Serialization::Document document;

                auto xliff = document.append_node("xliff");
                xliff.append_node("version", "1.2", Frenchie::Core::Serialization::NodeType::ATTRIBUTE);
                xliff.append_node("xmlns", "urn:oasis:names:tc:xliff:document:1.2", Frenchie::Core::Serialization::NodeType::ATTRIBUTE);

                auto file = xliff.append_node("file");
                file.append_node("source-language", "frenchie", Frenchie::Core::Serialization::NodeType::ATTRIBUTE);
                file.append_node("target-language", path.filename().stem().string().c_str(), Frenchie::Core::Serialization::NodeType::ATTRIBUTE);
                file.append_node("datatype", "plaintext", Frenchie::Core::Serialization::NodeType::ATTRIBUTE);

                auto body = file.append_node("body");

                for(auto translation : translations)
                {
                    auto transUnit = body.append_node("trans-unit");
                    transUnit.append_node("source", translation.first.c_str());
                    transUnit.append_node("target", translation.second.c_str());
                }

                m_Progress = (float)(++progress) / (float)(total);
                m_Status   = m_Status.append(fmt::format("finished...")).append("\n");
                document.write<Frenchie::Core::Serialization::XMLBeautifulWriter>(path);
            }

            m_Finished = true;
        }
    );

    return true;
}

std::string SaveTranslationFilesProcess::iprocess_status_request_status()
{
    std::unique_lock<std::mutex> lock(m_Mutex);
    return m_Status;
}

float SaveTranslationFilesProcess::iprocess_progress_request_progress()
{
    std::unique_lock<std::mutex> lock(m_Mutex);
    return m_Progress;
}

namespace Frenchie
{
    namespace Application
    {
        namespace Editor
        {
            namespace MainMenu
            {
                class OpenUpdateLocalizationFilesToolAction : 
                    public Frenchie::Application::Command::Registry<OpenUpdateLocalizationFilesToolAction, void*>
                {
                public:

                    OpenUpdateLocalizationFilesToolAction(void* _Sender = nullptr) : 
                        Frenchie::Application::Command::Registry<OpenUpdateLocalizationFilesToolAction, void*>(_Sender){}
                    virtual ~OpenUpdateLocalizationFilesToolAction(){}

                    // Frenchie::Application::Command
                    virtual void execute() override
                    {
                        Frenchie::Application::Application::instance()->find_or_push<TranslationFilesEditor>()->show();
                    }

                    // Command::TRegistryType
                    static std::string factory_id()
                    {
                        return fmt::format("{}::{}", STRINGIFY(Frenchie::Application::Editor::MainMenu), "Tools::Language::TranslationFilesEditor");
                    }
                };
            }
        }
    }
}

// namespace Frenchie
// {
//     namespace Application
//     {
//         namespace Editor
//         {
//             namespace Async
//             {
//                 class ToolsLoadLocalizationKeys : public Process
//                 {
//                 public:
//                     ToolsLoadLocalizationKeys(
//                         const std::filesystem::path& _SourceFilesPath,
//                         const std::filesystem::path& _XLIFFFilesPath,
//                         const std::function<void()>& _OnFinished, 
//                         const std::function<void()>& _OnCanceled, 
//                         const std::function<void()>& _OnFailed,
//                         const std::set<std::string>& _Extentions = {".cpp",".hpp",".cc", ".hh", ".c",".h"}
//                     ) : Process(
//                         _OnFinished, 
//                         _OnFinished, 
//                         _OnFailed), 
//                         m_SourceFilesPath(_SourceFilesPath),
//                         m_XLIFFFilesPath(_XLIFFFilesPath),
//                         m_Extentions(_Extentions){}

//                     virtual ~ToolsLoadLocalizationKeys(){}

//                     // Frenchie::Application::Layer
//                     bool ToolsLoadLocalizationKeys::awake()
//                     {
//                         Frenchie::Core::ThreadPool::instance()->enqueue(
//                             [this]()
//                             {
//                                 // load localization keys from source files
//                                 load_localization_keys_from_source_files();
//                             }
//                         );

//                         return true;
//                     }

//                 protected:
//                     std::filesystem::path m_SourceFilesPath;
//                     std::filesystem::path m_XLIFFFilesPath;
//                     std::set<std::string> m_LocalizationKeys;
//                     std::set<std::string> m_SupportedLanguages;
//                     std::set<std::string> m_Extentions;
//                     std::string           m_Status;

//                     void load_localization_keys_from_source_files()
//                     {
//                         if(!std::filesystem::exists(m_SourceFilesPath) || 
//                             m_Extentions.empty()) 
//                             return;

//                         m_Status.append(fmt::format("Start searching localization keys in {}", m_SourceFilesPath.string()));

//                         // setup patters...
//                         std::regex translator_pattern(R"(Translator::translate\(\s*\"([^\"]*)\"\s*\))");
//                         std::regex localization_key_pattern(R"(\s*\"([^\"]*)\"\s*)");

//                         try
//                         {
//                             for(auto directoryIterator = std::filesystem::recursive_directory_iterator(m_SourceFilesPath, std::filesystem::directory_options::skip_permission_denied); 
//                                 directoryIterator != std::filesystem::recursive_directory_iterator(); 
//                                     directoryIterator++)
//                             {
//                                 // handle pause and cancel events
//                                 while (m_Paused)
//                                 {
//                                     if(m_Canceled)
//                                         return;
//                                 }

//                                 if(m_Canceled)
//                                     return;

//                                 // get path
//                                 auto entry     = *directoryIterator;
//                                 auto path      = (*directoryIterator).path();
//                                 auto extention = Frenchie::Core::FileSystem::get_file_extention(*directoryIterator);

//                                 // skip not files and files of unsupported extention
//                                 if(entry.is_directory() || 
//                                     m_Extentions.find(extention) == m_Extentions.end())
//                                     continue;

//                                 FILE* file = Frenchie::Core::FileSystem::open_file(path.string(), "rb");

//                                 if(file == nullptr) 
//                                     continue;

//                                 m_Status.append(fmt::format("{}\n", path.filename().string()));

//                                 std::string contents;

//                                 char buffer[1024];

//                                 while (fgets(buffer, sizeof(buffer), file) != NULL)
//                                     contents.append(buffer);

//                                 fclose(file);

//                                 // Using std::regex_iterator (finds all matches)
//                                 for (auto translatorIterator = std::sregex_iterator(contents.begin(), contents.end(), translator_pattern); 
//                                             translatorIterator != std::sregex_iterator();
//                                                 ++translatorIterator) 
//                                 {
//                                     // handle pause and cancel events
//                                     while (m_Paused)
//                                     {
//                                         if(m_Canceled)
//                                             return;
//                                     }

//                                     if(m_Canceled)
//                                         return;

//                                     auto translatorEntry = (*translatorIterator).str();

//                                     for (auto localizationKeyIterator = 
//                                                 std::sregex_iterator(translatorEntry.begin(), translatorEntry.end(), localization_key_pattern); 
//                                                     localizationKeyIterator != std::sregex_iterator(); 
//                                                         ++localizationKeyIterator) 
//                                     {
//                                         // handle pause and cancel events
//                                         while (m_Paused)
//                                         {
//                                             if(m_Canceled)
//                                                 return;
//                                         }

//                                         if(m_Canceled)
//                                             return;

//                                         auto key = Frenchie::Core::String::remove_symbol((*localizationKeyIterator).str(), '"');

//                                         m_LocalizationKeys.insert(key);

//                                         m_Status.append(fmt::format("\t{}\n", key));
//                                     }
//                                 }
//                             }
//                         }
//                         catch(const std::exception& e)
//                         {
//                             Frenchie::Core::Logger::instance()->critical(e.what());
//                         }
//                     }
//                 };
//             }
//         }
//     }
// }

TranslationFilesEditor::TranslationFilesEditor() : 
    Layer("UpdateLocalizationFiles"){}

TranslationFilesEditor::~TranslationFilesEditor(){}

void TranslationFilesEditor::frame_update()
{
    ImGui::Begin(get_name().c_str());
    
    int id = 0;

    ImGui::TextUnformatted("XLIFF files search path");
    {
        auto path   = Frenchie::Core::String::as_utf8(m_TranslationFilesLocation.wstring());
        bool status = std::filesystem::exists(m_TranslationFilesLocation);

        ImGui::PushID(id++);
        ImGui::Checkbox("##", &status);
        ImGui::PopID();

        ImGui::SameLine();

        ImGui::PushID(id++);
        ImGui::InputText("##", &path, ImGuiInputTextFlags_::ImGuiInputTextFlags_ReadOnly);
        ImGui::PopID();
        
        ImGui::SameLine();

        ImGui::PushID(id++);
        
        if(ImGui::Button("Load"))
        {
            Frenchie::Application::Application::instance()->find_or_push<CommandsQueue>()->push<CallbackCommand>(
                [this]()
                {
                    Frenchie::Application::Application::instance()->push<FileSystem::Dialogs::ExplorerDialog>(
                        "Select directory where translation files are...",
                        [this]()
                        {
                            auto dialog = Frenchie::Application::Application::instance()->find<FileSystem::Dialogs::ExplorerDialog>();

                            if(dialog == nullptr) 
                                return;

                            // setup path
                            m_TranslationFilesLocation = dialog->get_path();

                            // finish previously launched process
                            if(m_LoadProcess != nullptr)
                                m_LoadProcess->close();

                            // launch new process
                            m_LoadProcess = Frenchie::Application::Application::instance()->push<LoadTranslationFilesProcess>(dialog->get_selected_paths());
                        }
                    );
                }
            );
        }

        ImGui::PopID();
    }


    ImGui::SeparatorText("Files");
    {
        if(m_LoadProcess != nullptr)
        {
            if(m_LoadProcess->finished()) // draw table on finished
            {
                for(auto&& translationFile : m_LoadProcess->get_translation_files())
                {
                    auto& path         = translationFile.first;
                    auto& translations = translationFile.second;

                    if(ImGui::TreeNode(path.string().c_str()))
                    {
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
                                "Key",
                                ImGuiTableColumnFlags_::ImGuiTableColumnFlags_WidthStretch |
                                ImGuiTableColumnFlags_::ImGuiTableColumnFlags_PreferSortAscending);

                            ImGui::TableSetupColumn(
                                "Translation",
                                ImGuiTableColumnFlags_::ImGuiTableColumnFlags_WidthStretch);
                            
                            ImGui::TableHeadersRow();

                            int id = 0;
                            for(auto translation : translations)
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

                        ImGui::TreePop();
                    }
                }   
            }
            else // draw progress indicator
            {
            }
        }
    }
    
    ImGui::End();
}

void TranslationFilesEditor::finish()
{
}