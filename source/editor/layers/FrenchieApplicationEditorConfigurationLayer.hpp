#pragma once

// Core
#include <FrenchieCoreISerializer.hpp>

// Application
#include <FrenchieApplicationLayer.hpp>

namespace Frenchie
{
    namespace Application
    {
        namespace Editor
        {
            // // kits
            // class Package
            // {
            // public:

            //     Package(const std::string& _Name) : m_Name(_Name){}

            //     ~Package(){}

            // protected:

            //     // nested types
            //     class PackageFinder
            //     {
            //     public:
            //         PackageFinder(const Package* _Package, std::set<std::filesystem::path>& _Paths, size_t _SearchDepth) : 
            //             m_Package(_Package)
            //         {
            //             Frenchie::Core::ThreadPool::instance()->enqueue(
            //                 [this, _Paths, _SearchDepth]()
            //                 {
            //                     for(auto&& path : _Paths)
            //                     {
            //                         try
            //                         {
            //                             for(auto it = std::filesystem::recursive_directory_iterator(path, std::filesystem::directory_options::skip_permission_denied); 
            //                                 it != std::filesystem::recursive_directory_iterator(); ++it)
            //                             {
            //                                 // stop on cancel
            //                                 if(m_Canceled) 
            //                                     return;

            //                                 if(it.depth() > _SearchDepth)
            //                                 {
            //                                     const_cast<std::filesystem::recursive_directory_iterator&>(it)
            //                                         .disable_recursion_pending();
            //                                     continue;
            //                                 }

            //                                 if(!it->is_directory() && 
            //                                     Frenchie::Core::String::to_lower(it->path().filename().stem().string()) == m_Package->m_Name) 
            //                                 {
            //                                     m_Package->m_Paths.insert(
            //                                         {
            //                                             it->path(), 
            //                                             false
            //                                         }
            //                                     );
            //                                 }
            //                             }
            //                         }
            //                         catch(const std::exception& e)
            //                         {
            //                             Frenchie::Core::Logger::instance()->critical(e.what());
            //                         }
            //                     }
            //                 }
            //             );
            //         }

            //         ~PackageFinder()
            //         {
            //             m_Canceled = true;
            //         }

            //     protected:
            //         const Package* m_Package = nullptr;

            //         bool m_Finished = false;
            //         bool m_Canceled = false;
            //         bool m_Failed   = false;
            //     };

            //     // info
            //     mutable std::map<std::filesystem::path, bool> m_Paths;
            //     mutable std::string                           m_Name;
            // };
            
            // class Kits
            // {
            // public:
            //     std::map<std::filesystem::path, bool> m_CMakes;
            //     std::map<std::filesystem::path, bool> m_CPacks;
            //     std::map<std::filesystem::path, bool> m_CCompilers;
            //     std::map<std::filesystem::path, bool> m_CXXCompilers;
            //     std::map<std::filesystem::path, bool> m_Generators;
            //     std::map<std::filesystem::path, bool> m_Debuggers;
            // };

            class Config : 
                public Layer, 
                public Frenchie::Core::Serialization::ISerializer
            {
            public:
                Config();
                virtual ~Config();

                // API
                void scan_fonts(const std::filesystem::path& _Paths);

                void load_fonts(
                    const std::set<std::filesystem::path>& _Paths, 
                    const std::string&                     _Font = std::string());

                // Frenchie::Application::Layer
                virtual bool awake() override;
                virtual void frame_start() override;
                virtual void frame_update() override;
                virtual bool allows_multiple_instances() const override;

                // Frenchie::Core::Serialization::ISerializer
                virtual bool serialize(const Frenchie::Core::Serialization::Node& _Parent) override;
                virtual bool deserialize(const Frenchie::Core::Serialization::Node& _Parent) override;

            protected:

                // fonts
                std::set<std::filesystem::path> m_Fonts = 
                    std::set<std::filesystem::path>();
            };
        }
    }
}