#pragma once

// FMT
#include <fmt/format.h>

// STL
#include <filesystem>

namespace Frenchie
{
    namespace Renderer
    {
        namespace OpenGL
        {
            class Shader
            {
                public:

                    Shader(const unsigned int& _ID, const std::filesystem::path& _Path);
                    virtual ~Shader();

                    const unsigned int& get_id() const;

                private:
                    unsigned int m_ID;
            };

            class GLVertexShader : public Shader
            {
            public:
                GLVertexShader(const std::filesystem::path& _Path);
                ~GLVertexShader();
            };

            class GLFragmentShader : public Shader
            {
            public:
                GLFragmentShader(const std::filesystem::path& _Path);
                ~GLFragmentShader();
            };
        }
    }
}