#include <FrenchieApplicationEditorToolsLanguageLayer.hpp>

#include <FrenchieApplication.hpp>
#include <FrenchieApplicationCommandsLayer.hpp>
#include <FrenchieApplicationEditorFileSystemExplorerLayer.hpp>

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
        Frenchie::Core::Logger::instance()->warn(path.string());

        if(!std::filesystem::exists(path)       || 
            std::filesystem::is_directory(path) || 
            Frenchie::Core::FileSystem::get_file_extention(path) != ".xlf") 
        {
            continue;
        }

        m_TranslationFiles.push_back(
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

                auto& path         = translationFile.Path;
                auto& translations = translationFile.Translations;

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
    const std::vector<TranslationFile>& _Translations,
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

                auto& path         = translationFile.Path;
                auto& translations = translationFile.Translations;

                m_Status = m_Status.append(fmt::format("starting saving file {}", path.string())).append("\n");

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

                Frenchie::Core::Logger::instance()->warn("Saving {}", path.string());
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
                        application()->push_layer<TranslationFilesEditor>()->show();
                    }

                    // Command::TRegistryType
                    static std::string factory_id()
                    {
                        return fmt::format("{}::{}", STRINGIFY(Frenchie::Editor::MainMenu), "Tools::Language::translation files editor");
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
    Layer("Translation files editor"){}

TranslationFilesEditor::~TranslationFilesEditor(){}

void TranslationFilesEditor::frame_update()
{
    // lock mutex to guarantee sage access to process data
    std::lock_guard<std::mutex> lock(m_Mutex);

    // draw
    ImGui::Begin(fmt::format("{}##{}", get_name().c_str(), get_uuid().to_string().c_str()).c_str());

    int id = 0;

    ImGui::TextUnformatted("Tools");
    {
        ImGui::PushID(id++);
        
        if(ImGui::Button("Load"))
        {
            Frenchie::Application::application()->push_layer<FileSystem::ExplorerDialog>(
                "Select directory where translation files are...",
                [this]()
                {
                    m_LoadProcess = 
                        Frenchie::Application::application()->push_process<LoadTranslationFilesProcess>(
                            Frenchie::Application::application()->find_layer<FileSystem::ExplorerDialog>()->get_selected_paths());
                }
            );
        }

        ImGui::SameLine();

        if(ImGui::Button("Save"))
        {
            m_SaveProcess = 
                Frenchie::Application::application()->push_process<SaveTranslationFilesProcess>(
                    m_LoadProcess->m_TranslationFiles);
        }

        ImGui::SameLine();

        if(ImGui::Button("Synchronize contents"))
        {
            if(!any_process_is_running())
            {
                // write logic here...
            }
        }

        ImGui::PopID();
    }


    ImGui::SeparatorText("Files");
    {
        if(m_LoadProcess != nullptr)
        {
            if(m_LoadProcess->finished()) // draw table on finished
            {
                for(auto&& translationFile : m_LoadProcess->m_TranslationFiles)
                {
                    auto& path         = translationFile.Path;
                    auto& translations = translationFile.Translations;

                    if(ImGui::TreeNode(path.string().c_str()))
                    {
                        if(ImGui::Button("Save"))
                        {
                            m_SaveProcess = Frenchie::Application::application()->push_process<SaveTranslationFilesProcess>(
                                std::vector<TranslationFile>({translationFile}));
                        }

                        ImGui::SameLine();

                        if(ImGui::Button("Save as"))
                        {
                            Frenchie::Application::application()->push_layer<FileSystem::ExplorerDialog>(
                                "Save as",
                                [this, &translationFile]()
                                {
                                    auto dialog = Frenchie::Application::application()->find_layer<FileSystem::ExplorerDialog>();

                                    if(dialog != nullptr) 
                                    {
                                        translationFile.Path = dialog->get_current_file();

                                        m_SaveProcess = 
                                            Frenchie::Application::application()->push_process<SaveTranslationFilesProcess>(
                                                std::vector<TranslationFile>({translationFile}));
                                    }
                                }
                            );
                        }

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
                            for(auto&& translation : translations)
                            {
                                ImGui::TableNextRow();

                                ImGui::TableSetColumnIndex(0);
                                ImGui::TextUnformatted(translation.first.c_str());
                                ImGui::SameLine();

                                ImGui::TableSetColumnIndex(1);
                                ImGui::PushID(++id);
                                ImGui::InputText("##", &translation.second);
                                ImGui::PopID();

                                ImGui::SameLine();

                                ImGui::PushID(++id);
                                
                                if(ImGui::Button("Remove", ImVec2(ImGui::GetContentRegionAvail().x, 0.f)))
                                {
                                    Frenchie::Application::application()->push_command<Frenchie::Application::CallbackCommand>(
                                        [&translations, &translation](){translations.erase(translation.first);});
                                }

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
                ImGui::TextUnformatted("Loading...");
            }
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

bool TranslationFilesEditor::any_process_is_running()
{
    if(m_LoadProcess == nullptr || m_SaveProcess == nullptr) 
        return false;

    if(m_LoadProcess->finished() || m_LoadProcess->canceled() || m_SaveProcess->finished() || m_SaveProcess->canceled()) 
        return false;

    return true;
}