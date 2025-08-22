#pragma once

#include <FrenchieApplication.hpp>

namespace Frenchie
{
    namespace Application
    {
        namespace Editor
        {
            class MainWindow
            {
            public:
                MainWindow();
                ~MainWindow();
                
                // API
                int execute();

            protected:
                std::filesystem::path m_AppExeDirectory;
                std::filesystem::path m_AppLogDirectory;
                std::filesystem::path m_AppStateDirectory;
            };
        }
    }
}