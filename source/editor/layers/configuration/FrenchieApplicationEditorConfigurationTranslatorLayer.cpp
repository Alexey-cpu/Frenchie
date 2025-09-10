#include <FrenchieApplicationEditorConfigurationTranslatorLayer.hpp>

#include <FrenchieApplication.hpp>
#include <FrenchieApplicationCommandsLayer.hpp>
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
#include <regex>
#include <fstream>
#include <iostream>
#include <functional>

// IMGUI
#include <imgui.h>

using namespace Frenchie;
using namespace Frenchie::Application;
using namespace Frenchie::Application::Editor;
using namespace Frenchie::Application::Editor::FileSystem;
using namespace Frenchie::Application::Editor::Configuration;


// Translator
Translator::Translator() : 
    Layer(STRINGIFY(Translator)){}

Translator::~Translator(){}

std::map<std::string, std::string>& Translator::get_translations() const
{
    return m_Translations;
}

std::set<std::string>& Translator::get_supported_languages() const
{
    return m_SupportedLanguages;
}

std::string Translator::get_current_language() const
{
    return m_CurrentLanguage;
}

void Translator::set_translation_files_path(const std::filesystem::path& _Path)
{
    if(!std::filesystem::exists(_Path)) 
        return;

    // setup path
    m_Path = _Path;

    // clean-up translations
    for(auto translation : m_Translations) 
        translation.second = std::string();

    // load translations from .xlf file
    Frenchie::Core::Serialization::Document document;

    if(!document.read<Frenchie::Core::Serialization::XMLReader>(translation_file_path())) 
        return;

    auto translations = document.find_node("xliff").find_node("file").find_node("body");

    for (auto translation : translations)
    {
        m_Translations[translation.find_node("source").get_value()] = 
            translation.find_node("target").get_value();
    }
}

void Translator::set_language(const std::string& _Language)
{
    if(m_SupportedLanguages.find(_Language) == m_SupportedLanguages.end()) 
        return;

    // select new language
    m_CurrentLanguage = _Language;

    // load language file
    set_translation_files_path(m_Path);
}

bool Translator::allows_multiple_instances() const
{
    return false;
}

bool Translator::serialize(const Frenchie::Core::Serialization::Node& _Parent)
{
    auto main = _Parent.append_node(STRINGIFY(Translator));

    // languages
    {
        auto languages = main.append_node("Languages");

        for(auto language : m_SupportedLanguages)
            languages.append_node(language.c_str());
    }

    // save .xlf file
    {
        if(!std::filesystem::exists(m_Path)) 
            return true;

        Frenchie::Core::Serialization::Document document;

        auto xliff = document.append_node("xliff");
        xliff.append_node("version", "1.2", Frenchie::Core::Serialization::NodeType::ATTRIBUTE);
        xliff.append_node("xmlns", "urn:oasis:names:tc:xliff:document:1.2", Frenchie::Core::Serialization::NodeType::ATTRIBUTE);

        auto file = xliff.append_node("file");
        file.append_node("source-language", "frenchie", Frenchie::Core::Serialization::NodeType::ATTRIBUTE);
        file.append_node("target-language", m_CurrentLanguage.c_str(), Frenchie::Core::Serialization::NodeType::ATTRIBUTE);
        file.append_node("datatype", "plaintext", Frenchie::Core::Serialization::NodeType::ATTRIBUTE);

        auto body = file.append_node("body");

        for(auto translation : m_Translations)
        {
            auto transUnit = body.append_node("trans-unit");
            transUnit.append_node("source", translation.first.c_str());
            transUnit.append_node("target", translation.second.c_str());
        }

        document.write<Frenchie::Core::Serialization::XMLBeautifulWriter>(translation_file_path());
    }

    return true;
}

bool Translator::deserialize(const Frenchie::Core::Serialization::Node& _Parent)
{
    auto main = _Parent.find_node(STRINGIFY(Translator));

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

std::string Translator::translate(const std::string& _Key)
{
    auto localizator = application()->find<Translator>();

    if(localizator == nullptr) 
        localizator = application()->push<Translator>();

    auto translation = localizator->m_Translations[_Key];

    return translation.empty() ? _Key : translation;
}

std::filesystem::path Translator::translation_file_path()
{
    return m_Path.wstring()
                .append(L"/")
                .append(Frenchie::Core::String::as_wide(m_CurrentLanguage))
                .append(L".xlf");
}