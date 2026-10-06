#pragma once

// Application
#include <FrenchieApplication.hpp>
#include <FrenchieApplicationLayer.hpp>
#include <FrenchieApplicationRenderingBackend.hpp>

// Core
#include <FrenchieCoreStringUtilities.hpp>

// STL
#include <map>
#include <iostream>
#include <filesystem>

namespace Frenchie
{
    namespace Application
    {
        // Asset
        class Asset
        {
        public:
            Asset(const std::filesystem::path&);
            virtual ~Asset();
            const std::filesystem::path m_Path;
        };

        // ImageAsset
        class ImageAsset : public Asset
        {
        public:
            ImageAsset(const std::filesystem::path&);
            virtual ~ImageAsset();
            const ApplicationRenderingBackendTexture m_Asset{ApplicationRenderingBackendTexture()};
        };

        // FontAsset
        class FontAsset : public Asset
        {
        public:
            FontAsset(const std::filesystem::path& _Path, const int& _Size);
            virtual ~FontAsset();
            const ApplicationRenderingBackendFont m_Asset{ApplicationRenderingBackendFont()};
        };

        // Assets
        class Assets final
        {
        private:

            // nested types
            class Importer final : public Layer
            {
            public:
                Importer();
                virtual ~Importer();

                virtual void frame_start() override;
                virtual void finish() override;
                virtual bool allows_multiple_instances() const override;

                template<typename Type, typename ... Args>
                std::shared_ptr<Type> request(const std::filesystem::path& _Path, const Args& ... _Args)
                {
                    auto iterator = m_Assets.find(_Path);

                    if(iterator != m_Assets.end() && !iterator->second.expired())
                        return std::dynamic_pointer_cast<Type>(iterator->second.lock());

                    std::shared_ptr<Type> asset = std::make_shared<Type>(_Path, _Args ...);
                    m_Assets[_Path] = asset;
                    return asset;
                }

            private:
                std::map<std::filesystem::path, std::weak_ptr<Asset>> m_Assets{std::map<std::filesystem::path, std::weak_ptr<Asset>>()};
            };

        public:

            Assets() = delete;
            Assets(const Assets&) = delete;
            Assets& operator=(const Assets&) = delete;

            template<typename Type, typename ... Args>
            static std::shared_ptr<Type> request(const std::filesystem::path& _Path, const Args& ... _Args)
            {
                return Frenchie::Application::App::push_layer<Importer>()->request<Type>(_Path, _Args ...);
            }
        };
    }
}