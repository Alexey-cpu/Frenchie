#include <FrenchieApplicationEditorConfigurationLocalizatorLayer.hpp>

#include <FrenchieApplication.hpp>
#include <FrenchieApplicationEditorCommandsLayer.hpp>
#include <FrenchieApplicationEditorFileSystemExplorerLayer.hpp>

// Frenchie::Core
#include <FrenchieCoreHelpers.hpp>
#include <FrenchieCoreThreadPool.hpp>

#include <FrenchieCoreSerializationFormatXML.hpp>
#include <FrenchieCoreSerializationFormatJSON.hpp>
#include <FrenchieCoreSerializationFormatYAML.hpp>

// SPDLOG
#include "spdlog/sinks/basic_file_sink.h"

// STL
#include <chrono>
#include <iostream>
#include <fstream>
#include <functional>

// IMGUI
#include <imgui.h>

using namespace Frenchie;
using namespace Frenchie::Application;
using namespace Frenchie::Application::Editor;
using namespace Frenchie::Application::Editor::FileSystem;
using namespace Frenchie::Application::Editor::Configuration;

Localizator::Localizator(){}

Localizator::~Localizator(){}

bool Localizator::allows_multiple_instances() const
{
    return false;
}

bool Localizator::serialize(const Frenchie::Core::Serialization::Node& _Parent)
{
    auto main = _Parent.append_node(STRINGIFY(Localizator));

    // path
    {
        if(std::filesystem::exists(m_Path)) 
            main.append_node("Path", Frenchie::Core::String::as_utf8(m_Path.wstring()).c_str());
    }

    // languages
    {
        auto languages = main.append_node("Languages");

        for(auto language : m_Languages)
            languages.append_node(language.first.c_str()).set_value_as<bool>(language.second);
    }

    // save serialization file
    {
        for(auto language : m_Languages)
        {
            if(!language.second) 
                continue;
                
            Frenchie::Core::Serialization::Document document;
            document.append_node("Language", language.first.c_str());
            auto translations = document.append_node("Translations");

            for(auto translation : m_Translations)
                translations.append_node(translation.first.c_str(), translation.second.c_str());

            document.write<Frenchie::Core::Serialization::XMLBeautifulWriter>(
                m_Path.wstring()
                    .append(L"/")
                    .append(Frenchie::Core::String::as_wide(language.first))
                    .append(L".xml")
            );

            break;
        }
    }

    return true;
}

bool Localizator::deserialize(const Frenchie::Core::Serialization::Node& _Parent)
{
    auto main = _Parent.find_node(STRINGIFY(Localizator));

    if(!main.is_valid()) 
        return false;

    // Path
    {
        auto node = main.find_node("Path");

        if(node.is_valid() && 
            std::filesystem::exists(std::filesystem::path(node.get_value())))
        {
            // setuo path
            m_Path = std::filesystem::path(node.get_value());
        }
    }

    // languages
    {
        auto node = main.find_node("Languages");

        if(node.is_valid())
        {
            m_Languages.clear();

            for(auto&& language : node)
            {
                m_Languages[language.get_name()] = 
                    language.get_value_as<bool>();
            }
        }
    }

    // load localizations file
    {
        for(auto&& language : m_Languages)
        {
            if(!language.second) 
                continue;

            auto lang = language.first;

            // load lang file
        }
    }

    return true;
}

std::string Localizator::translation(const std::string& _Key)
{
    auto localizator = 
        Frenchie::Application::Application::instance()->find_or_push<Localizator>();

    if(localizator == nullptr) 
        return _Key;

    auto translation = localizator->m_Translations[_Key];

    return translation.empty() ? _Key : translation;
}

void Localizator::setup_translation_files_location(const std::filesystem::path& _Path)
{

}

void Localizator::setup_language(const std::string& _Language)
{
    auto localizator = 
        Frenchie::Application::Application::instance()->find_or_push<Localizator>();

    if(localizator == nullptr || 
        localizator->m_Languages.find(_Language) == localizator->m_Languages.end()) 
        return;

    // deselect all languages
    for(auto language : localizator->m_Languages)
        language.second = false;

    // select new language
    localizator->m_Languages[_Language] = true;

    // load language file
}