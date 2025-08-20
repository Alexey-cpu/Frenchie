#pragma once

#include <FrenchieApplication.hpp>

namespace Frenchie
{
    namespace Application
    {
        namespace Editor
        {
            class Editor
            {
            public:
                Editor();
                virtual ~Editor();
                int execute();

            protected:
                std::filesystem::path m_AppExeDirectory;
                std::filesystem::path m_AppLogDirectory;
                std::filesystem::path m_AppStateDirectory;
            };
        }
    }
}