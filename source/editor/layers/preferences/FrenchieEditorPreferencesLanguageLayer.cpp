#include <FrenchieEditorPreferencesLanguageLayer.hpp>

// Editor
#include <FrenchieEditorFileSystemExplorerLayer.hpp>
#include <FrenchieEditorConfigurationTranslatorLayer.hpp>

// IMGUI
#include <imgui_stdlib.h>

// STL
#include <chrono>
#include <regex>
#include <fstream>
#include <iostream>
#include <functional>

using namespace Frenchie::Editor;
using namespace Frenchie::Editor::FileSystem;
using namespace Frenchie::Editor::Preferences;
using namespace Frenchie::Editor::Configuration;

Languages::Languages() : 
    Frenchie::Application::Layer::Registry<Languages>(
        Translator::translate(STRINGIFY(Languages))){}
Languages::~Languages(){}

std::string Languages::factory_id()
{
    return STRINGIFY(Frenchie::Application::Editor::Preferences::Languagess);
}

void Languages::frame_update()
{
    ImGui::BeginChild(get_name().c_str());
    {
        auto& supportedLanguages = Translator::instance()->get_supported_languages();

        if(supportedLanguages.empty())
        {
            ImGui::TextUnformatted("No translation files found. Press 'Load' to load some.");
            
            ImGui::SameLine();
            
            if(ImGui::Button("Load"))
            {
                auto dialog = Frenchie::Application::application()->push_layer<FileSystem::PathScannerDialog>(
                    [](const std::filesystem::path& _Entry)->bool
                    {
                        return !std::filesystem::is_directory(_Entry) && 
                            Frenchie::Core::FileSystem::get_file_extention(_Entry) == ".xlf";
                    }
                );

                dialog->on_finished(
                    [this](std::map<std::filesystem::path, bool>& _Paths)
                    {
                        std::set<std::filesystem::path> paths;

                        for(auto&& entry : _Paths)
                        {
                            if(entry.second)
                                paths.insert(entry.first);
                        }

                        Translator::instance()->set_supported_languages(paths);
                    }
                );
            }
        }
        else
        {
            int id = 0;
            for(auto&& supportedLanguage : supportedLanguages)
            {
                ImGui::PushID(++id);
                bool selected = supportedLanguage->is_current();
                if(ImGui::Checkbox(supportedLanguage->get_name().c_str(), &selected))
                    supportedLanguage->setup();
                ImGui::PopID();
            }
        }
    }
    ImGui::EndChild();
}

bool Languages::allows_multiple_instances() const
{
    return false;
}