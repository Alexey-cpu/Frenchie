#pragma once

// FMT
#include <fmt/format.h>

// STL
#include <filesystem>
#include <set>

namespace Frenchie
{
    namespace Renderer
    {
        namespace OpenGL
        {
            class ShaderLoader
            {
            public:

                ShaderLoader(const std::filesystem::path& _Path);
                virtual ~ShaderLoader();

                const unsigned int& get_id() const;

            private:
                unsigned int m_ID;

                std::set<std::string> m_Extentions = 
                {
                    ".vert", 
                    ".tesc", 
                    ".tese", 
                    ".geom", 
                    ".frag", 
                    ".comp"
                };
            };

            class Shader
            {
            public:

                Shader(const std::vector<std::shared_ptr<ShaderLoader>>& _Shaders);
                virtual ~Shader();

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