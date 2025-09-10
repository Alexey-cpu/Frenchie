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
    auto localizator = application()->find_layer<Translator>();

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
    }
    ImGui::EndChild();
}

bool Languages::allows_multiple_instances() const
{
    return false;
}