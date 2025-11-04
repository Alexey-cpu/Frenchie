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
        namespace Renderer
        {
            class Image
            {
            public:

                Image(const std::filesystem::path& _Path);
                Image();

                ~Image();

                // getters
                unsigned char* get_buffer() const;
                int            get_width() const;
                int            get_height() const;
                int            get_channels() const;
                bool           is_valid() const;

            protected:

                int                            m_Width   {-1}; 
                int                            m_Height  {-1};
                int                            m_Channels{-1};
                std::shared_ptr<unsigned char> m_Buffer  {nullptr};
            };

            std::shared_ptr<Image>
            load_image(const std::filesystem::path& _Path);
            
            bool save_image_as_png(
                const std::shared_ptr<Image>& _Image,
                const std::filesystem::path&  _Path,
                const std::u32string&         _FileName);
            
            bool save_image_as_jpg(
                const std::shared_ptr<Image>& _Image,
                const std::filesystem::path&  _Path,
                const std::u32string&         _FileName,
                const int&                    _Quality = 90);
            
            bool save_image_as_tga(
                const std::shared_ptr<Image>& _Image,
                const std::filesystem::path&  _Path,
                const std::u32string&         _FileName);
        
            bool save_image_as_bmp(
                const std::shared_ptr<Image>& _Image,
                const std::filesystem::path&  _Path,
                const std::u32string&         _FileName);
        }
    }
}