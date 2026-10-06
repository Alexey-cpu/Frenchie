#include <FrenchieAssetsImporterLayer.hpp>

using namespace Frenchie::Application;

// Asset
Asset::Asset(const std::filesystem::path& _Path) : m_Path(_Path){}
Asset::~Asset(){}

// ImageAsset
ImageAsset::ImageAsset(const std::filesystem::path& _Path) : Asset(_Path), m_Asset(Frenchie::Application::ApplicationRenderingBackend::construct_texture(_Path.string().c_str())){}
ImageAsset::~ImageAsset(){Frenchie::Application::ApplicationRenderingBackend::destroy_texture(m_Asset);}

// FontAsset
FontAsset::FontAsset(const std::filesystem::path& _Path, const int& _Size) : Asset(_Path), m_Asset(Frenchie::Application::ApplicationRenderingBackend::construct_font(_Path.string().c_str(), _Size)){}
FontAsset::~FontAsset(){Frenchie::Application::ApplicationRenderingBackend::destroy_font(m_Asset);}

// Assets::Importer
Assets::Importer::Importer() : Layer(STRINGIFY(Assets)){}
Assets::Importer::~Importer(){}

void Assets::Importer::frame_start()
{
    if(!Frenchie::Application::App::wants_clear_cache()) return;

    decltype(m_Assets) assets;

    for(auto& asset : m_Assets)
    {
        if(!asset.second.expired())
            assets.insert({asset.first, asset.second});
    }

    m_Assets = std::move(assets);
}

void Assets::Importer::finish()
{
    m_Assets.clear();
}

bool Assets::Importer::allows_multiple_instances() const
{
    return false;
}