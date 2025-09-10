#include <FrenchieApplicationEditorConfigurationFontsLayer.hpp>

#include <FrenchieApplication.hpp>
#include <FrenchieApplicationCommandsLayer.hpp>
#include <FrenchieApplicationEditorFileSystemExplorerLayer.hpp>
#include <FrenchieApplicationEditorConfigurationTranslatorLayer.hpp>

// Frenchie::Core
#include <FrenchieCoreHelpers.hpp>
#include <FrenchieCoreThreadPool.hpp>

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
using namespace Frenchie::Editor;
using namespace Frenchie::Editor::FileSystem;
using namespace Frenchie::Editor::Configuration;

namespace Frenchie
{
    namespace Editor
    {
        class FontsLoader : public Dialog
        {
        public:

            FontsLoader(
                const std::set<std::filesystem::path>& _Paths,
                const std::string                      _Font,
                const std::function<void()>&           _OnFinished,
                const std::string&                     _Name = STRINGIFY(Frenchie::Application::Editor::AsyncLoaderView)) : 
                Dialog(_Name, 512.f, 128.f), 
                m_Paths(_Paths), 
                m_Font(_Font), 
                m_OnFinished(_OnFinished){}
            
            virtual ~FontsLoader(){}

            // Dialog
            virtual bool awake() override
            {
                Frenchie::Core::ThreadPool::instance()->enqueue(
                    [this]()
                    {
                        std::set<std::filesystem::path> fonts;
                        auto total   = m_Paths.size();
                        auto current = 0;

                        for(auto&& path : m_Paths)
                        {
                            if(m_Canceled) 
                                return;

                            while (m_Paused)
                            {
                                if(m_Canceled) 
                                    return;
                            }

                            if(!std::filesystem::exists(path) || 
                                fonts.find(path) != fonts.end())
                            {
                                m_Progress = (float)(++current) / (float)total;
                                continue;
                            }

                            // retrive ImGui IO
                            auto& io = ImGui::GetIO();

                            // load font
                            try
                            {
                                io.Fonts->AddFontFromFileTTF(
                                    Frenchie::Core::String::as_utf8(path).c_str(),
                                    ImGui::GetStyle().FontSizeBase,
                                    nullptr,
                                    io.Fonts->GetGlyphRangesCyrillic());
                            }
                            catch(const std::exception& e)
                            {
                                Frenchie::Core::Logger::instance()->critical(e.what());
                            }

                            m_Progress = (float)(++current) / (float)total;

                            // add to cache
                            fonts.insert(path);
                        }

                        // finish
                        m_Finished = true;
                    }
                );

                return true;
            }

            virtual void frame_update() override
            {
                if(m_Canceled || m_Failed) 
                {
                    close();
                    return;
                }

                Dialog::frame_update();
            }

            virtual void draw_content() override
            {
                if(m_Finished)
                {
                    close(); // close dialog when finished
                    return;
                }

                // calculate progress percantage
                int percantage = (int)(m_Progress * 100.f);

                // show progress
                ImGui::SetNextItemWidth(ImGui::GetContentRegionAvail().x - ImGui::CalcTextSize("100%").x);
                ImGui::ProgressBar(m_Progress, ImVec2(0.0f, 0.0f), "");
                ImGui::SameLine(0.0f, ImGui::GetStyle().ItemInnerSpacing.x);
                ImGui::TextUnformatted(fmt::format("{} %", percantage).c_str());
            }

            virtual void draw_buttons() override
            {
                if(ImGui::Button("Cancel"))
                {
                    m_Finished = false;
                    m_Canceled = true;
                    close();
                }
            }

            virtual void finish() override
            {
                if(!m_Finished) 
                    return;

                // apply new font
                for (ImFont* font : ImGui::GetIO().Fonts->Fonts)
                {
                    font->Scale = 1.f;

                    if (std::string(font->GetDebugName()) == m_Font)
                    {
                        ImGui::GetIO().FontDefault = font;
                        break;
                    }
                }

                // build fonts
                ImGui::GetIO().Fonts->Build();

                // reload app
                application()->reload();

                //
                if(m_OnFinished != nullptr)
                    m_OnFinished();
            }

        protected:

            // info
            float                           m_Progress   = 0.f;
            std::string                     m_Font       = std::string();
            std::set<std::filesystem::path> m_Paths      = std::set<std::filesystem::path>();
            std::function<void()>           m_OnFinished = nullptr;

            // task
            bool m_Paused   = false;
            bool m_Finished = false;
            bool m_Canceled = false;
            bool m_Failed   = false;
        };
    }
}

Fonts::Fonts() : 
    Layer(STRINGIFY(Fonts)){}

Fonts::~Fonts(){}

bool Fonts::allows_multiple_instances() const 
{
    return false;
}

bool Fonts::serialize(const Frenchie::Core::Serialization::Node& _Parent)
{
    auto main  = _Parent.append_node(STRINGIFY(Fonts));
    main.append_node("Size").set_value_as<float>(ImGui::GetStyle().FontSizeBase);
    main.append_node("Font", ImGui::GetFont()->GetDebugName());
    auto entries = main.append_node("Fonts", ImGui::GetFont()->GetDebugName());

    for(auto&& font : m_Path)
    {
        entries.append_node(
            Frenchie::Core::String::as_utf8(font.filename().stem().wstring()).c_str(), 
            Frenchie::Core::String::as_utf8(font.wstring()).c_str());
    }

    return true;
}

bool Fonts::deserialize(const Frenchie::Core::Serialization::Node& _Parent)
{
    auto fonts = _Parent.find_node("Fonts");

    if(fonts.is_valid())
    {
        // size
        {
            auto node = fonts.find_node("Size");
            if(node.is_valid()) 
                ImGui::GetStyle().FontSizeBase = node.get_value_as<float>();
        }

        // load fonts
        {
            std::set<std::filesystem::path> fontsToLoad;

            auto childFonts = fonts.find_node("Fonts");

            for(auto&& font : childFonts)
            {
                std::filesystem::path path(font.get_value());

                if(std::filesystem::exists(path)) 
                    fontsToLoad.insert(path);
            }

            load_fonts(fontsToLoad, std::string(fonts.find_node("Font").get_value()));
        }
    }

    return true;
}

void Fonts::scan_fonts(const std::filesystem::path& _Path)
{
    // load fonts
    application()->push_layer<PathScannerDialog>(
        _Path,
        [](const std::filesystem::path& _Entry)->bool
        {
            return !std::filesystem::is_directory(_Entry) && 
                Frenchie::Core::FileSystem::get_file_extention(_Entry) == ".ttf";
        },
        [this](std::map<std::filesystem::path, bool>& _Paths)
        {
            std::set<std::filesystem::path> paths;

            for(auto&& entry : _Paths)
            {
                if(entry.second) 
                    paths.insert(entry.first);
            }

            load_fonts(paths, std::string());
        }
    ); 
}

void Fonts::load_fonts(
    const std::set<std::filesystem::path>& _Fonts, 
    const std::string&                     _Font)
{
    if(_Fonts.empty()) 
        return;

    // load fonts
    application()->push_command<CallbackCommand>(
            [this, _Font, _Fonts]()
            {
                // remove old fonts besides the current font
                // as we need to render something
                auto fonts = ImGui::GetIO().Fonts->Fonts;

                for(ImFont* font : fonts)
                {
                    if(font != ImGui::GetIO().FontDefault)
                        ImGui::GetIO().Fonts->RemoveFont(font);
                }

                // load new fonts
                application()->push_layer<FontsLoader>(
                    _Fonts, 
                    _Font,
                    [this, _Fonts]()
                    {
                        m_Path = _Fonts;
                    },
                    Translator::translate("FRENCHIE_APPLICATION_EDITOR_CONFIGURATION_FONTS_LOADING").c_str());
            }
        );
}