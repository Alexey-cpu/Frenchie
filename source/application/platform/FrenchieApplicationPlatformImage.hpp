#pragma once

// STL
#include <filesystem>
#include <iostream>
#include <memory>
#include <set>

// STB
#include <stb_image.h>
#include <stb_image_write.h>

namespace Frenchie
{
    namespace Application
    {
        // Renderer RAII image
        struct Image final
        {
        public:

            Image();
            Image(const std::filesystem::path& _Path);

            ~Image();

            int          Width    {0};
            int          Height   {0};
            int          Channels {0};
            unsigned int Sampler2D{0};
        };

        // std::shared_ptr<Image>
        // load_image(const std::filesystem::path& _Path);
        
        // bool save_image_as_png(
        //     const std::shared_ptr<Image>& _Image,
        //     const std::filesystem::path&  _Path,
        //     const std::u32string&         _FileName);
        
        // bool save_image_as_jpg(
        //     const std::shared_ptr<Image>& _Image,
        //     const std::filesystem::path&  _Path,
        //     const std::u32string&         _FileName,
        //     const int&                    _Quality = 90);
        
        // bool save_image_as_tga(
        //     const std::shared_ptr<Image>& _Image,
        //     const std::filesystem::path&  _Path,
        //     const std::u32string&         _FileName);
    
        // bool save_image_as_bmp(
        //     const std::shared_ptr<Image>& _Image,
        //     const std::filesystem::path&  _Path,
        //     const std::u32string&         _FileName);
    }
}