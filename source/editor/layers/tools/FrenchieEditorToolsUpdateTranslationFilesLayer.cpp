#include <FrenchieEditorToolsUpdateTranslationFilesLayer.hpp>

// Core
#include <FrenchieCoreThreadPool.hpp>

// Application
#include <FrenchieApplicationCommandsLayer.hpp>

// Editor
#include <FrenchieEditorFileSystemExplorerLayer.hpp>
#include <FrenchieEditorConfigurationTranslatorLayer.hpp>

// STL
#include <regex>

// IMGUI
#include <imgui.h>
#include <imgui_stdlib.h>

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
            class OpenTranslationFilesUpdaterToolAction : 
                public Frenchie::Application::Command::Registry<OpenTranslationFilesUpdaterToolAction, void*>
            {
            public:

                OpenTranslationFilesUpdaterToolAction(void* _Sender = nullptr) : 
                    Frenchie::Application::Command::Registry<OpenTranslationFilesUpdaterToolAction, void*>(_Sender){}
                virtual ~OpenTranslationFilesUpdaterToolAction(){}

                // Frenchie::Application::Command
                virtual void execute() override
                {
                    Frenchie::Application::application()->push_layer<TranslationFilesUpdater>()->show();
                }

                // Command::TRegistryType
                static std::string factory_id()
                {
                    return fmt::format("{}::{}", STRINGIFY(Frenchie::Editor::MainMenu), "Tools::Language::Translation files updater");
                }
            };
        }
    }
}

namespace Frenchie
{
    namespace Editor
    {
        class LoadLocalizationKeysFromSourceCodeProcess : public Frenchie::Application::Process, public Frenchie::Application::IProcessStatus
        {
        public:
            LoadLocalizationKeysFromSourceCodeProcess(
                const std::filesystem::path& _SourceFilesPath,
                const std::set<std::string>& _Extentions = {".cpp",".hpp",".cc", ".hh", ".c",".h"}
            ) : Process(STRINGIFY(LoadLocalizationKeysFromSourceCodeProcess)), 
                m_SourceFilesPath(_SourceFilesPath),
                m_Extentions(_Extentions){}

            virtual ~LoadLocalizationKeysFromSourceCodeProcess(){}

            // Frenchie::Application::Layer
            virtual bool awake() override
            {
                Frenchie::Core::ThreadPool::instance()->enqueue(
                    [this]()
                    {
                        if(!std::filesystem::exists(m_SourceFilesPath) || 
                            m_Extentions.empty()) 
                            return;

                        m_Status.append(fmt::format("Start searching localization keys in {}", m_SourceFilesPath.string()));

                        // setup patters...
                        std::regex translator_pattern(R"(Translator::translate\(\s*\"([^\"]*)\"\s*\))");
                        std::regex localization_key_pattern(R"(\s*\"([^\"]*)\"\s*)");

                        try
                        {
                            for(auto directoryIterator = std::filesystem::recursive_directory_iterator(m_SourceFilesPath, std::filesystem::directory_options::skip_permission_denied); 
                                directoryIterator != std::filesystem::recursive_directory_iterator(); 
                                    directoryIterator++)
                            {
                                // handle pause and cancel events
                                while (m_Paused)
                                {
                                    if(m_Canceled)
                                        return;
                                }

                                if(m_Canceled)
                                    return;

                                // get path
                                auto entry     = *directoryIterator;
                                auto path      = (*directoryIterator).path();
                                auto extention = Frenchie::Core::FileSystem::get_file_extention(*directoryIterator);

                                // skip not files and files of unsupported extention
                                if(entry.is_directory() || 
                                    m_Extentions.find(extention) == m_Extentions.end())
                                    continue;

                                FILE* file = Frenchie::Core::FileSystem::open_file(path.string(), "rb");

                                if(file == nullptr) 
                                    continue;

                                m_Status.append(fmt::format("{}\n", path.filename().string()));

                                std::string contents;

                                char buffer[1024];

                                while (fgets(buffer, sizeof(buffer), file) != NULL)
                                    contents.append(buffer);

                                fclose(file);

                                // Using std::regex_iterator (finds all matches)
                                for (auto translatorIterator = std::sregex_iterator(contents.begin(), contents.end(), translator_pattern); 
                                            translatorIterator != std::sregex_iterator();
                                                ++translatorIterator) 
                                {
                                    // handle pause and cancel events
                                    while (m_Paused)
                                    {
                                        if(m_Canceled)
                                            return;
                                    }

                                    if(m_Canceled)
                                        return;

                                    auto translatorEntry = (*translatorIterator).str();

                                    for (auto localizationKeyIterator = 
                                                std::sregex_iterator(translatorEntry.begin(), translatorEntry.end(), localization_key_pattern); 
                                                    localizationKeyIterator != std::sregex_iterator(); 
                                                        ++localizationKeyIterator) 
                                    {
                                        // handle pause and cancel events
                                        while (m_Paused)
                                        {
                                            if(m_Canceled)
                                                return;
                                        }

                                        if(m_Canceled)
                                            return;

                                        auto key = Frenchie::Core::String::remove_symbol((*localizationKeyIterator).str(), '"');

                                        m_LocalizationKeys.insert(key);

                                        m_Status.append(fmt::format("\t{}\n", key));
                                    }
                                }
                            }

                            // finish process
                            m_Finished = true;
                        }
                        catch(const std::exception& e)
                        {
                            Frenchie::Core::Logger::instance()->critical(e.what());
                            m_Failed = true;
                        }
                    }
                );

                return true;
            }

            virtual std::string iprocess_status_request_status() override
            {
                std::lock_guard<std::mutex> lock(m_Mutex);
                return m_Status;
            }

            std::set<std::string> m_LocalizationKeys;

        protected:
            std::filesystem::path m_SourceFilesPath;
            std::set<std::string> m_Extentions;
            std::string           m_Status;
        };
    
        class UpdateLocalizationKeysInTranslationFilesProcess : public Frenchie::Application::Process, public Frenchie::Application::IProcessStatus
        {
        public:
            UpdateLocalizationKeysInTranslationFilesProcess(
                const std::set<std::filesystem::path> _Paths,
                const std::set<std::string>&          _LocalizationKeys) : 
                Process(STRINGIFY(UpdateLocalizationKeysInTranslationFilesProcess)), 
                m_Paths(_Paths), 
                m_LocalizationKeys(_LocalizationKeys){}

            virtual ~UpdateLocalizationKeysInTranslationFilesProcess(){}

            // Frenchie::Application::Layer
            virtual bool awake() override
            {
                auto localizationKeysLoadProcess = 
                    Frenchie::Application::ProcessQueue::instance()->push<LoadTranslationFilesProcess>(m_Paths);

                localizationKeysLoadProcess->on_finished(
                    [this, localizationKeysLoadProcess]()
                    {
                        // update keys
                        for(auto&& translationFile : localizationKeysLoadProcess->m_TranslationFiles)
                        {
                            for(auto& localizationKey : m_LocalizationKeys)
                            {
                                if(translationFile.Translations.find(localizationKey) == 
                                    translationFile.Translations.end())
                                {
                                    TranslationUnit unit;
                                    unit.Key      = localizationKey;
                                    unit.Value    = "";
                                    unit.Selected = false;
                                    translationFile.Translations.insert(unit);
                                }
                            }
                        }

                        // save files
                        Frenchie::Application::ProcessQueue::instance()->push<SaveTranslationFilesProcess>(
                            localizationKeysLoadProcess->m_TranslationFiles);
                    }
                );

                return true;
            }

            virtual std::string iprocess_status_request_status() override
            {
                std::lock_guard<std::mutex> lock(m_Mutex);
                return m_Status;
            }

        protected:

            std::set<std::filesystem::path> m_Paths;
            std::set<std::string>           m_LocalizationKeys;
            std::string                     m_Status;
        };
    }
}

TranslationFilesUpdater::TranslationFilesUpdater() : 
    Layer(Translator::translate("Translation files editor")){}
TranslationFilesUpdater::~TranslationFilesUpdater(){}

void TranslationFilesUpdater::frame_update()
{
    // update name
    set_name(Translator::translate("Translation files updater"));

    // draw
    ImGui::Begin(fmt::format("{}###Translation files updater", get_name()).c_str(), &m_Opened);

    ImGui::SeparatorText(Translator::translate("Localization keys tool bar").c_str());
    {
        // load localization keys from source code
        ImGui::InputText("##", &m_SourceCodeFilesPath, ImGuiInputTextFlags_::ImGuiInputTextFlags_ReadOnly);
        ImGui::SameLine();
        
        if(ImGui::Button(Translator::translate("Load localization keys from source code").c_str()))
        {
            Frenchie::Application::application()->push_layer<Frenchie::Editor::FileSystem::ExplorerDialog>(
                Translator::translate("Select directory where the source code files are located..."))->on_accepted(
                [this]()
                {
                    auto dialog = 
                        Frenchie::Application::application()->find_layer<Frenchie::Editor::FileSystem::ExplorerDialog>();

                    auto process = 
                        Frenchie::Application::ProcessQueue::instance()
                            ->push<LoadLocalizationKeysFromSourceCodeProcess>(dialog->get_current_path());

                    m_SourceCodeFilesPath = Frenchie::Core::String::as_utf8(dialog->get_current_path().wstring());

                    process->on_finished(
                        [this, process]()
                        {
                            m_LocalizationKeys = process->m_LocalizationKeys;
                        }
                    );
                }
            );
        }

        // suggest to update translation files by the previously loaded keys
        if(!m_LocalizationKeys.empty())
        {
            ImGui::SameLine();

            if(ImGui::Button(Translator::translate("Update translation files").c_str()))
            {
                Frenchie::Application::application()->push_layer<Frenchie::Editor::FileSystem::ExplorerDialog>(
                    Translator::translate("Select translation files you need to update..."))->on_accepted(
                    [this]()
                    {
                        auto dialog = 
                            Frenchie::Application::application()->find_layer<Frenchie::Editor::FileSystem::ExplorerDialog>();

                        auto process = 
                            Frenchie::Application::ProcessQueue::instance()
                                ->push<UpdateLocalizationKeysInTranslationFilesProcess>(dialog->get_selected_paths(), m_LocalizationKeys);
                    }
                );
            }
        }

        // show what we have just loaded
        ImGui::SetNextItemOpen(!m_LocalizationKeys.empty());

        if(ImGui::TreeNode(Translator::translate("Localization keys").c_str()))
        {
            if(!m_LocalizationKeys.empty())
            {
                int  widgetID = 0;
                auto width    = ImGui::GetContentRegionAvail().x * 0.7f;

                for(auto&& localizationKey : m_LocalizationKeys)
                {
                    if(localizationKey.empty()) 
                        continue;

                    ImGui::SetNextItemWidth(width);
                    ImGui::TextUnformatted(localizationKey.c_str());
                    ImGui::SameLine();
                    
                    ImGui::PushID(++widgetID);

                    if(ImGui::Button(Translator::translate("Remove").c_str()))
                    {
                        Frenchie::Application::CommandsQueue::instance()->push<Frenchie::Application::CallbackCommand>(
                            [this, localizationKey]()
                            {
                                m_LocalizationKeys.erase(localizationKey);
                            }
                        );
                    }

                    ImGui::PopID();
                }
            }

            ImGui::TreePop();
        }
    }

    ImGui::End();
}

void TranslationFilesUpdater::finish(){}

bool TranslationFilesUpdater::allows_multiple_instances() const
{
    return false;
}