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
    namespace Core
    {
        namespace Image
        {
            class ImageInfo
            {
            public:

                ImageInfo(const std::filesystem::path& _Path);
                ImageInfo();

                ~ImageInfo();

                // getters
                unsigned char* get_buffer() const;
                int  get_width() const;
                int  get_height() const;
                int  get_channels() const;
                bool is_valid() const;

            protected:

                int            m_Width   {-1}; 
                int            m_Height  {-1};
                int            m_Channels{-1};
                unsigned char* m_Buffer  {nullptr};
            };

            std::shared_ptr<ImageInfo>
            load_image(const std::filesystem::path& _Path);
            
            bool save_image_as_png(
                const std::shared_ptr<ImageInfo>& _Image,
                const std::filesystem::path&      _Path,
                const std::u32string&             _FileName);
            
            bool save_image_as_jpg(
                const std::shared_ptr<ImageInfo>& _Image,
                const std::filesystem::path&      _Path,
                const std::u32string&             _FileName,
                const int&                        _Quality = 90);
            
            bool save_image_as_tga(
                const std::shared_ptr<ImageInfo>& _Image,
                const std::filesystem::path&      _Path,
                const std::u32string&             _FileName);
        
            bool save_image_as_bmp(
                const std::shared_ptr<ImageInfo>& _Image,
                const std::filesystem::path&      _Path,
                const std::u32string&             _FileName);
        }
    }
}