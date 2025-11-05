#pragma once

#include <FrenchieApplication.hpp>

namespace Frenchie
{
    namespace Application
    {
        namespace Configuration
        {
            class ConfigurationLoader : public Frenchie::Application::Layer
            {
            public:
                ConfigurationLoader(const std::filesystem::path& _Path = std::filesystem::current_path());
                virtual ~ConfigurationLoader();

                // getters
                std::filesystem::path get_app_data_path() const;

                // Frenchie::Application::Layer
                virtual bool awake() override;
                virtual void finish() override;
                virtual bool allows_multiple_instances() const override;

            protected:
                std::filesystem::path m_Path;
            };
        }
    }
}