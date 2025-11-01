#pragma once

// Application
#include <FrenchieApplication.hpp>
#include <FrenchieApplicationLayer.hpp>

// STL
#include <filesystem>
#include <map>

// IMGUI
#include <imgui.h>
#include <imgui_stdlib.h>
#include <imgui_internal.h>

namespace Frenchie
{
    namespace Editor
    {
        namespace Tools
        {
            enum FontHardCoderToolEncoding
            {
                HardCoderToolEncoding_UTF8,
                HardCoderToolEncoding_UTF32
            };

            enum FontHardCoderToolSourceFileFormat
            {
                FontHardCoderToolSourceFileFormat_C,
                FontHardCoderToolSourceFileFormat_CPP,
                FontHardCoderToolSourceFileFormat_C_SHARP
            };

            struct FontHardCoderToolCodePointInfo
            {
                unsigned int Code = 0;
                std::string  Name = std::string();
            };

            struct FontHardCoderToolFontInfo
            {
                ImFont*                                     Font  = nullptr;
                std::vector<FontHardCoderToolCodePointInfo> Codes = std::vector<FontHardCoderToolCodePointInfo>();
            };

            class FontHardCoderTool : public Frenchie::Application::Layer
            {
            public:
                FontHardCoderTool();
                virtual ~FontHardCoderTool();

                // Frenchie::Application::Layer
                virtual void frame_update() override;
                virtual void finish() override;

            protected:

                std::filesystem::path      m_TTF;
                std::filesystem::path      m_CPP;
                std::string                m_ClassName{"HardCodedFont"};
                FontHardCoderToolEncoding  m_Encoding {FontHardCoderToolEncoding::HardCoderToolEncoding_UTF8};
                bool                       m_Compress {true};
                FontHardCoderToolFontInfo  m_FontInfo;

                // service methods
                void on_ttf_file_path_search_button_pressed();
                void on_cpp_file_path_search_button_pressed();
                void on_font_hardcode_button_pressed();

                std::string encoding_to_string(const FontHardCoderToolEncoding&);
            };
        }
    }
}