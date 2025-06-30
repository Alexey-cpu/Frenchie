#pragma once

#include <FrenchieCoreObject.hpp>

// FMT
#include <fmt/format.h>

// STL
#include <filesystem>
#include <set>

namespace Frenchie
{
    namespace Renderer
    {
        class Shader
        {
        public:

            Shader(const std::filesystem::path& _Vertex, const std::filesystem::path& _Fragment);
            virtual ~Shader();

            bool instantiate();
            void use();
            void unuse();

            const unsigned int& get_id() const;
            template<typename T> void set_uniform(const std::string& _Name, const T& _Value);

        protected:
            const std::filesystem::path m_VertexShaderPath   = std::filesystem::path();
            const std::filesystem::path m_FragmentShaderPath = std::filesystem::path();

        private:
            unsigned int m_ID = 0;
        };
    }
}