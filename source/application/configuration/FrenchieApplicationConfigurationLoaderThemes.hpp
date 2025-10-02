#pragma once

// Core
#include <FrenchieCoreISerializer.hpp>

// Application
#include <FrenchieApplicationLayer.hpp>

namespace Frenchie
{
    namespace Application
    {
        namespace Configuration
        {
            class Theme;
            class Themes;

            class Theme
            {
            public:
                Theme(const std::filesystem::path& _Path, const Themes* _Owner);
                ~Theme();

                // getters
                std::string get_name() const;
                std::filesystem::path get_path() const;
                bool is_current() const;

                // API
                void setup();
                void save();

            protected:

                const std::filesystem::path m_Path   {std::filesystem::path()};
                const Themes*               m_Owner  {nullptr};
                bool                        m_Current{false};
            };

            class Themes : 
                public Frenchie::Application::Layer, 
                public Frenchie::Core::Serialization::ISerializer
            {
            public:
                Themes();
                virtual ~Themes();

                // getters
                std::filesystem::path get_app_theme_files_path() const;
                Frenchie::Core::Reference<Theme> get_current_theme() const;
                std::vector<Frenchie::Core::Reference<Theme>> get_supported_themes() const;

                // setters
                void set_supported_themes(const std::set<std::filesystem::path>&);

                // Frenchie::Application::Layer
                virtual bool allows_multiple_instances() const override;

                // Frenchie::Core::Serialization::ISerializer
                virtual bool serialize(const Frenchie::Core::Serialization::Node& _Parent) override;
                virtual bool deserialize(const Frenchie::Core::Serialization::Node& _Parent) override;

                // API
                Frenchie::Core::Reference<Theme> create_theme(const std::filesystem::path& _Path);

                // static API
                static Frenchie::Core::Reference<Themes> instance();

            protected:

                friend class Theme;

                mutable std::filesystem::path                                   m_AppThemeFilesPath;
                mutable std::map<std::filesystem::path, std::shared_ptr<Theme>> m_Themes;
            };
        }
    }
}