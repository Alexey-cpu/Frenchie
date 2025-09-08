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

Localizator::Localizator() : 
    Layer(STRINGIFY(Frenchie::Application::Editor::Configuration::Localizator)){}

Localizator::~Localizator(){}

std::map<std::string, std::string>& Localizator::get_translations() const
{
    return m_Translations;
}

std::set<std::string>& Localizator::get_supported_languages() const
{
    return m_SupportedLanguages;
}

std::string Localizator::get_current_language() const
{
    return m_CurrentLanguage;
}

void Localizator::set_translation_files_path(const std::filesystem::path& _Path)
{
    if(!std::filesystem::exists(_Path)) 
        return;

    // setup path
    m_Path = _Path;

    // clean-up translations
    for(auto translation : m_Translations) 
        translation.second = std::string();

    // load translations
    Frenchie::Core::Serialization::Document document;

    if(document.read<Frenchie::Core::Serialization::XMLReader>(translation_file_path()))
    {
        auto translations = 
            document.find_node("Translations");

        if(!translations.is_valid()) 
            return;

        for (auto translation : translations)
        {
            m_Translations[translation.get_name()] = 
                translation.get_value();
        }
    }
}

void Localizator::set_language(const std::string& _Language)
{
    if(m_SupportedLanguages.find(_Language) == m_SupportedLanguages.end()) 
        return;

    // select new language
    m_CurrentLanguage = _Language;

    // load language file
    set_translation_files_path(m_Path);
}

bool Localizator::allows_multiple_instances() const
{
    return false;
}

bool Localizator::serialize(const Frenchie::Core::Serialization::Node& _Parent)
{
    auto main = _Parent.append_node(STRINGIFY(Localizator));

    // languages
    {
        auto languages = main.append_node("Languages");

        for(auto language : m_SupportedLanguages)
            languages.append_node(language.c_str());
    }

    // save serialization file
    {
        if(std::filesystem::exists(m_Path)) 
        {
            main.append_node("Path", Frenchie::Core::String::as_utf8(m_Path.wstring()).c_str());

            Frenchie::Core::Serialization::Document document;
            document.append_node("Language", m_CurrentLanguage.c_str());
            auto translations = document.append_node("Translations");

            for(auto translation : m_Translations)
                translations.append_node(translation.first.c_str(), translation.second.c_str());

            document.write<Frenchie::Core::Serialization::XMLBeautifulWriter>(translation_file_path());
        }
    }

    return true;
}

bool Localizator::deserialize(const Frenchie::Core::Serialization::Node& _Parent)
{
    auto main = _Parent.find_node(STRINGIFY(Localizator));

    if(!main.is_valid()) 
        return false;

    // read a list of supported languages
    {
        auto node = main.find_node("Languages");

        if(node.is_valid())
        {
            m_SupportedLanguages.clear();

            for(auto&& language : node)
                m_SupportedLanguages.insert(language.get_name());
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

std::filesystem::path Localizator::translation_file_path()
{
    return m_Path.wstring()
                .append(L"/")
                .append(Frenchie::Core::String::as_wide(m_CurrentLanguage))
                .append(L".xml");
}