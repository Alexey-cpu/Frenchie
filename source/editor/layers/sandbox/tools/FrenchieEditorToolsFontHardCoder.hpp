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
        enum SourceEncoding
        {
            SourceEncoding_U8,
            SourceEncoding_U32
        };

        struct CodePointInfo
        {
            unsigned int Codepoint;
            std::string  Meta;
        };

        struct FontInfo
        {
            ImFont*                    Font;
            std::vector<CodePointInfo> Codepoints;
        };

        class FontHardCoderTool : public Frenchie::Application::Layer
        {
        public:
            FontHardCoderTool();
            virtual ~FontHardCoderTool();

            // Frenchie::Application::Layer
            virtual void frame_update() override;

        protected:

            std::filesystem::path               m_TTF;
            std::filesystem::path               m_CPP;
            std::string                         m_ClassName {"HardCodedFont"};
            SourceEncoding                      m_Encoding  {SourceEncoding::SourceEncoding_U8};
            bool                                m_Compress  {false};
            FontInfo                            m_FontInfo;

            // service methods
            std::string encoding_to_string(const SourceEncoding& _Encoding)
            {
                if(_Encoding == SourceEncoding::SourceEncoding_U8)
                    return "UTF-8";

                if(_Encoding == SourceEncoding::SourceEncoding_U32)
                    return "UTF-32";

                return "UNKNOWN";
            }
        };
    }
}