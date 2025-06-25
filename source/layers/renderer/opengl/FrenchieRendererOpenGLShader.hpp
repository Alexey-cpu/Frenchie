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

            class VertexShader : public Shader
            {
            public:
                VertexShader(const std::filesystem::path& _Path);
                virtual ~VertexShader();
            };

            class FragmentShader : public Shader
            {
            public:
                FragmentShader(const std::filesystem::path& _Path);
                virtual ~FragmentShader();
            };

            class ShaderProgram
            {
            public:

                ShaderProgram(const std::vector<std::shared_ptr<Shader>>& _Shaders);
                virtual ~ShaderProgram();

                void begin();
                void end();

                const unsigned int& get_id() const;
                template<typename T> void set_uniform(const std::string& _Name, const T& _Value);

            protected:
                unsigned int m_ID;
            };
        }
    }
}