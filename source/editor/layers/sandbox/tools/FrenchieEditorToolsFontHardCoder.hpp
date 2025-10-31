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
            std::map<unsigned int, std::string> m_CodePoints{std::map<unsigned int, std::string>()};

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