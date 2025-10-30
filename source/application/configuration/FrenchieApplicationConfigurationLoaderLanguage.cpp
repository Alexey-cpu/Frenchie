#include <FrenchieApplicationConfigurationLoaderLanguage.hpp>

// Core
#include <FrenchieCoreThreadPool.hpp>
#include <FrenchieCoreFileSystem.hpp>
#include <FrenchieCoreStringUnicode.hpp>
#include <FrenchieCoreSerializationFormatXML.hpp>
#include <FrenchieCoreSerializationFormatJSON.hpp>
#include <FrenchieCoreSerializationFormatYAML.hpp>
#include <FrenchieApplicationConfigurationLoader.hpp>

// Application
#include <FrenchieApplication.hpp>
#include <FrenchieApplicationCommandsLayer.hpp>

// STL
#include <chrono>
#include <regex>
#include <fstream>
#include <iostream>
#include <functional>

// IMGUI
#include <imgui.h>

using namespace Frenchie::Application::Configuration;

namespace Frenchie
{
    namespace Application
    {
        namespace Configuration
        {
            class PushTranslatorIntoAppQueue : 
                public Frenchie::Application::Command::Registry<PushTranslatorIntoAppQueue, const CommandPayloads&>
            {
            public:

                PushTranslatorIntoAppQueue(const CommandPayloads& _Sender) : 
                    Frenchie::Application::Command::Registry<PushTranslatorIntoAppQueue, const CommandPayloads&>(_Sender){}
                virtual ~PushTranslatorIntoAppQueue(){}

                // Frenchie::Application::Command
                virtual void execute() override
                {
                    Frenchie::Application::Configuration::Translator::instance();
                }

                // Command::TRegistryType
                static std::string factory_id()
                {
                    return fmt::format("{}::{}", STRINGIFY(Frenchie::Application::Configuration), STRINGIFY(Translator));
                }
            };

            const bool pushTranslatorIntoAppQueueStatus = PushTranslatorIntoAppQueue::registerFactory();
        }
    }
}

Language::Language(const std::filesystem::path& _Path, const Translator* _Translator) : 
    m_Path(_Path), 
    m_Owner(_Translator){}

Language::~Language(){}

// getters
std::string Language::get_name() const
{
    return get_path().filename().stem().string();
}

std::filesystem::path Language::get_path() const
{
    return Frenchie::Core::String::convert_utf32_to_utf8(m_Path.u32string());
}

TranslationFile& Language::get_translation_file() const
{
    return m_TranslationFile;
}

bool Language::is_current() const
{
    return m_Current;
}

void Language::setup()
{
    if(m_Owner == nullptr) 
        return;

    if(!m_Owner->m_ThreadsQueue.instance()->empty()) 
    {
        // TODO: add log here...
        return;
    }

    // deselect all
    for(auto&& supportedLanguage : m_Owner->m_SupportedLanguages)
        supportedLanguage->m_Current = false;

    // load translation file
    auto loadTranslationFile = m_Owner->m_ThreadsQueue.instance()->push(
        [this](const Frenchie::Application::Thread* _Thread)
        {
            auto progress = _Thread->find_component<Frenchie::Application::ThreadProgressComponent>();
            auto status   = _Thread->find_component<Frenchie::Application::ThreadStatusComponent>();

            if(!std::filesystem::exists(m_Path)       || 
                std::filesystem::is_directory(m_Path) || 
                Frenchie::Core::FileSystem::get_file_extention(m_Path) != ".xlf") 
            {
                return;
            }

            // load XLIFF
            if(status != nullptr)
                status->push_message(fmt::format("Trying to load: {}\n", m_Path.string()));

            // load translations from .xlf file
            Frenchie::Core::Serialization::Document document;

            if(!document.read<Frenchie::Core::Serialization::XMLReader>(m_Path)) 
            {
                if(status != nullptr)
                    status->push_message("Could not load...\n");

                return;
            }

            TranslationFile translationFile;
            auto& translations = translationFile.Translations;

            auto body = document.find_node("xliff").find_node("file").find_node("body");

            size_t current = 0;
            size_t total   = body.size();

            for (auto item : body)
            {
                translations.insert(
                    {
                        item.find_node("source").get_value(),
                        item.find_node("target").get_value(),
                        false    
                    }
                );

                if(progress != nullptr) 
                    progress->set_progress((float)(++current) / (float)(total));
            }

            // update progress
            if(status != nullptr)
                status->push_message("Success...\n");

            // update progress
            if(status != nullptr)
                status->push_message("Completed...\n");

            // setup translation file
            m_TranslationFile = translationFile;
        }
    );

    loadTranslationFile->on_finished(
        [this](const Frenchie::Application::Thread*){m_Current = true;});

    loadTranslationFile->attach_component<Frenchie::Application::ThreadProgressComponent>();
    loadTranslationFile->attach_component<Frenchie::Application::ThreadStatusComponent>();
}

void Language::save()
{
    if(!m_Owner->m_ThreadsQueue.instance()->empty()) 
    {
        // TODO: add log here...
        return;
    }

    // save translation file
    auto saveTranslationFile = m_Owner->m_ThreadsQueue.instance()->push(
        [this](const Frenchie::Application::Thread* _Thread)
        {
            // retrieve components
            auto progress = _Thread->find_component<Frenchie::Application::ThreadProgressComponent>();
            auto status   = _Thread->find_component<Frenchie::Application::ThreadStatusComponent>();

            // write file
            Frenchie::Core::Serialization::Document document;

            auto& translations = m_TranslationFile.Translations;
            auto& path         = m_TranslationFile.Path;

            // xliff
            auto xliff = document.append_node("xliff");
            xliff.append_node("version", "1.2", Frenchie::Core::Serialization::ATTRIBUTE);
            xliff.append_node("xmlns", "urn:oasis:names:tc:xliff:document:1.2", Frenchie::Core::Serialization::ATTRIBUTE);

            // file
            auto file = xliff.append_node("file");
            file.append_node("source-language", "english", Frenchie::Core::Serialization::ATTRIBUTE);
            file.append_node("target-language", get_name().c_str(), Frenchie::Core::Serialization::ATTRIBUTE);
            file.append_node("datatype", "plaintext", Frenchie::Core::Serialization::ATTRIBUTE);

            // body
            auto body = file.append_node("body");

            size_t current = 0;
            size_t total   = translations.size();

            if(status != nullptr)
            {
                status->push_message(
                    fmt::format(
                        "start saving .xlf file to {}\n",
                        Frenchie::Core::String::convert_utf32_to_utf8(m_Path.u32string())
                    )
                );
            }

            for(auto&& translation : translations)
            {
                auto unit = body.append_node("trans-unit");
                unit.append_node("source", translation.Key.c_str());
                unit.append_node("target", translation.Value.c_str());

                if(status != nullptr)
                {
                    status->push_message(
                        fmt::format(
                            "saving {} --> {}\n",
                            translation.Key,
                            translation.Value
                        )
                    );
                }

                if(progress != nullptr) 
                    progress->set_progress((float)(++current)/(float)(total));
            }
    
            if(status != nullptr)
                status->push_message("Finished...\n");

            if(progress != nullptr) 
                progress->set_progress((float)(++current)/(float)(total));

            // save file
            document.write<Frenchie::Core::Serialization::XMLBeautifulWriter>(m_Path);
        }
    );

    saveTranslationFile->attach_component<Frenchie::Application::ThreadProgressComponent>();
    saveTranslationFile->attach_component<Frenchie::Application::ThreadStatusComponent>();
}

// Translator
Translator::Translator() : 
    Layer(STRINGIFY(Translator)){}

Translator::~Translator(){}

std::filesystem::path Translator::get_app_translation_files_path() const
{
    if(std::filesystem::exists(m_AppTranslationFilesPath))
        return m_AppTranslationFilesPath;

    auto configurationLoader = 
        Frenchie::Application::application()
            ->find_layer<Frenchie::Application::Configuration::ConfigurationLoader>();

    if(configurationLoader == nullptr) 
        return m_AppTranslationFilesPath;
        
    m_AppTranslationFilesPath = configurationLoader->get_app_data_path().u32string().append(U"/translations");

    if(!std::filesystem::exists(m_AppTranslationFilesPath)) 
    {
        try
        {
            std::filesystem::create_directory(m_AppTranslationFilesPath);
        }
        catch(...)
        {
            // TODO: put a log here...
        }
    }

    return m_AppTranslationFilesPath;
}

std::vector<Frenchie::Core::Reference<Language>> Translator::get_supported_languages() const
{
    std::vector<Frenchie::Core::Reference<Language>> supportedLanguages;

    for(auto&& supportedLanguage : m_SupportedLanguages)
        supportedLanguages.push_back(Frenchie::Core::Reference<Language>(supportedLanguage));

    return supportedLanguages;
}

Frenchie::Core::Reference<Language> Translator::get_current_language() const
{
    for(auto&& supportedLanguage : m_SupportedLanguages) 
    {
        if(supportedLanguage->is_current()) 
            return Frenchie::Core::Reference<Language>(supportedLanguage);
    }

    return Frenchie::Core::Reference<Language>();
}

void Translator::set_supported_languages(const std::set<std::filesystem::path>& _Paths)
{
    if(_Paths.empty()) 
        return;

    m_SupportedLanguages.clear();

    for(auto&& path : _Paths)
    {
        if(std::filesystem::exists(path) && 
            Frenchie::Core::FileSystem::get_file_extention(path) == ".xlf") 
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
                    std::filesystem::path(supportedLanguage.get_value()).make_preferred(), 
                    this
                )
            );
        }
    }
    else // try to to load translation files from default path
    {
        std::filesystem::path defaultTranslationFilesPath = get_app_translation_files_path();

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
            catch(...){}
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
    auto currentLanguage = Translator::instance()->get_current_language();

    if(currentLanguage == nullptr) 
        return _Key;

    const auto& translations = currentLanguage->get_translation_file().Translations;
    const auto  iterator     = translations.find(_Key);

    return iterator == translations.end() || iterator->Value.empty() ? _Key : iterator->Value;
}

Frenchie::Core::Reference<Language> Translator::create_new_translation_file(const std::filesystem::path& _Path)
{
    auto path  = std::filesystem::path(_Path.parent_path().u32string().append(U"/").append(_Path.filename().stem().u32string()).append(U".xlf")).make_preferred();
    auto theme = std::make_shared<Language>(path, this);
    m_SupportedLanguages.push_back(theme);

    return Frenchie::Core::Reference<Language>(theme);
}

Frenchie::Core::Reference<Translator> Translator::instance()
{
    auto layer = Frenchie::Application::application()->find_layer<Translator>();

    if(layer == nullptr) 
        layer = Frenchie::Application::application()->push_layer<Translator>();

    return layer;
}