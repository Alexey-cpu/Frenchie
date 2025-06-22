#pragma once

#include <FrenchieLogger.hpp>

// FMT
#include <fmt/format.h>

namespace Frenchie
{
    namespace Renderer
    {
        template<class IShader>
        class Shader
        {
            public:

                Shader(const unsigned int& _ID, const std::filesystem::path& _Path) :  m_IShader(IShader(_ID, _Path)){}
                ~Shader(){}

                const unsigned int& get_id() const
                {
                    return m_IShader.get_id();
                }

            private:
                IShader m_IShader;
        };
    }
}