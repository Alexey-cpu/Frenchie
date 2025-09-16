#include <FrenchieEditorConfigurationTranslatorLayer.hpp>

// Application
#include <FrenchieApplication.hpp>
#include <FrenchieApplicationCommandsLayer.hpp>

// Core
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

namespace Frenchie
{
    namespace Editor
    {
        namespace Configuration
        {
            class PushTranslatorIntoAppQueue : 
                public Frenchie::Application::Command::Registry<PushTranslatorIntoAppQueue, void*>
            {
            public:

                PushTranslatorIntoAppQueue(void* _Sender) : 
                    Frenchie::Application::Command::Registry<PushTranslatorIntoAppQueue, void*>(_Sender){}
                virtual ~PushTranslatorIntoAppQueue(){}

                // Frenchie::Application::Command
                virtual void execute() override
                {
                    Frenchie::Editor::Configuration::Translator::instance();
                }

                // Command::TRegistryType
                static std::string factory_id()
                {
                    return fmt::format("{}::{}", STRINGIFY(Frenchie::Editor::Configuration), STRINGIFY(Translator));
                }
            };
        }
    }
}

using namespace Frenchie::Editor;
using namespace Frenchie::Editor::Configuration;

// LoadTranslationFiles
LoadTranslationFilesProcess::LoadTranslationFilesProcess(const std::set<std::filesystem::path>& _Path) : 
    Process(STRINGIFY(LoadTranslationFilesProcess))
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
                std::set<TranslationUnit, TranslationUnit::TransparentComparator>(),
            }
        );
    }
}

LoadTranslationFilesProcess::LoadTranslationFilesProcess(const std::filesystem::path& _Path) : 
    Process(STRINGIFY(LoadTranslationFilesProcess))
{
    if(std::filesystem::exists(_Path)         && 
        !std::filesystem::is_directory(_Path) && 
        Frenchie::Core::FileSystem::get_file_extention(_Path) == ".xlf") 
    {
        m_TranslationFiles.push_back(
            {
                _Path, 
                std::set<TranslationUnit, TranslationUnit::TransparentComparator>(),
            }
        );
    }
}

LoadTranslationFilesProcess::~LoadTranslationFilesProcess(){}

void LoadTranslationFilesProcess::execute() 
{
    // finish on empty
    if(m_TranslationFiles.empty())
    {
        m_Finished = true;
        return;
    }

    // run process
    size_t total    = m_TranslationFiles.size();
    size_t progress = 0;

    for(auto&& translationFile : m_TranslationFiles)
    {
        while(paused())
        {
            if(canceled()) 
                return;
        }

        if(canceled()) 
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
                translations.insert(
                    {
                        item.find_node("source").get_value(),
                        item.find_node("target").get_value(),
                        false    
                    }
                );
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
SaveTranslationFilesProcess::SaveTranslationFilesProcess(const std::vector<TranslationFile>& _Translations) :
    Process(STRINGIFY(SaveTranslationFilesProcess)), m_TranslationFiles(_Translations){}

SaveTranslationFilesProcess::~SaveTranslationFilesProcess(){}

void SaveTranslationFilesProcess::execute()
{
    if(m_TranslationFiles.empty())
    {
        m_Finished = true;
        return;
    }

    size_t total    = m_TranslationFiles.size();
    size_t progress = 0;

    for(auto&& translationFile : m_TranslationFiles)
    {
        while(paused())
        {
            if(canceled()) 
                return;
        }

        if(canceled()) 
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
            transUnit.append_node("source", translation.Key.c_str());
            transUnit.append_node("target", translation.Value.c_str());
        }

        m_Progress = (float)(++progress) / (float)(total);
        m_Status   = m_Status.append(fmt::format("finished...")).append("\n");
        document.write<Frenchie::Core::Serialization::XMLBeautifulWriter>(path);

        Frenchie::Core::Logger::instance()->warn("Saving {}", path.string());
    }

    m_Finished = true;
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

Language::Language(const std::filesystem::path& _Path, const Translator* _Translator) : 
    m_Path(_Path), 
    m_Translator(_Translator){}

Language::~Language(){}

// getters
std::string Language::get_name() const
{
    return get_path().filename().stem().string();
}

std::filesystem::path Language::get_path() const
{
    return Frenchie::Core::String::as_utf8(m_Path.wstring());
}

bool Language::is_current() const
{
    return m_Current;
}

void Language::setup()
{
    if(m_Translator == nullptr) 
        return;

    // deselect all
    for(auto&& supportedLanguage : m_Translator->m_SupportedLanguages)
        supportedLanguage->m_Current = false;

    // select self
    m_Current = true;

    // load translation file
    m_Translator->m_Process = Frenchie::Application::ProcessQueue::instance()->push<LoadTranslationFilesProcess>(m_Path);
    m_Translator->m_Process->on_finished(
        [this]()
        {
            if(m_Translator->m_Process->m_TranslationFiles.empty()) 
                return;

            m_Translator->m_TranslationFile = 
                m_Translator->m_Process->m_TranslationFiles.front();

            m_Translator->m_Process = nullptr;
        }
    );
}

// Translator
Translator::Translator() : 
    Layer(STRINGIFY(Translator)){}

Translator::~Translator(){}

std::vector<std::unique_ptr<Language>>& Translator::get_supported_languages() const
{
    return m_SupportedLanguages;
}

void Translator::set_supported_languages(const std::set<std::filesystem::path>& _Paths)
{
    if(_Paths.empty()) 
        return;

    m_SupportedLanguages.clear();

    for(auto&& path : _Paths)
    {
        if(Frenchie::Core::FileSystem::get_file_extention(path) == ".xlf") 
            m_SupportedLanguages.push_back(std::make_unique<Language>(path, this));
    }

    if(!m_SupportedLanguages.empty())
        m_SupportedLanguages.front()->setup();
}

bool Translator::allows_multiple_instances() const
{
    return false;
}

bool Translator::serialize(const Frenchie::Core::Serialization::Node& _Parent)
{
    auto main = _Parent.append_node(STRINGIFY(Translator));
    
    // supported languages
    if(m_SupportedLanguages.empty()) 
        return true;

    auto languages       = main.append_node("Languages");
    auto currentLanguage = main.append_node("CurrentLanguage");

    for(auto&& language : m_SupportedLanguages)
    {
        languages.append_node(
            language->get_name().c_str(), 
            language->get_path().string().c_str());

        if(language->is_current())
            currentLanguage.set_value(language->get_name().c_str());
    }

    return true;
}

bool Translator::deserialize(const Frenchie::Core::Serialization::Node& _Parent)
{
    auto main = _Parent.find_node(STRINGIFY(Translator));

    if(!main.is_valid())
        return false;

    // retrieve supported languages
    auto supportedLanguages = main.find_node("Languages");

    if(supportedLanguages.is_valid())
    {
        for(auto&& supportedLanguage : supportedLanguages)
        {
            if(!supportedLanguage.is_valid() || 
                std::string(supportedLanguage.get_name()).empty() || 
                !std::filesystem::exists(std::string(supportedLanguage.get_value()))) 
                continue;

            m_SupportedLanguages.push_back(
                std::make_unique<Language>(
                    std::filesystem::path(supportedLanguage.get_value()), 
                    this
                )
            );
        }
    }
    else // try to to load translation files from default path
    {
        std::filesystem::path defaultTranslationFilesPath = 
            std::filesystem::path(
                Frenchie::Core::FileSystem::get_exe_absolute_directory().wstring().append(L"/appData/translations/")).make_preferred();

        if(std::filesystem::exists(defaultTranslationFilesPath))
        {
            try
            {
                for(auto directory : std::filesystem::directory_iterator(defaultTranslationFilesPath, 
                                     std::filesystem::directory_options::skip_permission_denied))
                {
                    if(!directory.is_directory() &&
                        Frenchie::Core::FileSystem::get_file_extention(directory.path()) == ".xlf") 
                    {
                        m_SupportedLanguages.push_back(std::make_unique<Language>(directory.path(), this));
                    }
                }
            }
            catch(const std::exception& e)
            {
                Frenchie::Core::Logger::instance()->critical(e.what());
            }
        }
    }

    // select current language
    auto currentLanguage = main.find_node("CurrentLanguage");

    if(currentLanguage.is_valid() &&
        !std::string(currentLanguage.get_name()).empty() && 
        !std::string(currentLanguage.get_value()).empty()) 
    {
        for (auto&& supportedLanguage : m_SupportedLanguages)
        {
            if(supportedLanguage->get_name() == 
                std::string(currentLanguage.get_value()))
            {
                supportedLanguage->setup();
                break;
            }
        }
    }
    else // try to setup default language
    {
        if(!m_SupportedLanguages.empty())
            m_SupportedLanguages.front()->setup();
    }

    return true;
}

std::string Translator::translate(const std::string& _Key)
{
    const auto& translations = Translator::instance()->m_TranslationFile.Translations;
    const auto  iterator     = translations.find(_Key);

    return iterator == translations.end() || iterator->Value.empty() ? _Key : iterator->Value;
}

Frenchie::Core::Reference<Translator> Translator::instance()
{
    auto layer = Frenchie::Application::application()->find_layer<Translator>();

    if(layer == nullptr) 
        layer = Frenchie::Application::application()->push_layer<Translator>();

    return layer;
}