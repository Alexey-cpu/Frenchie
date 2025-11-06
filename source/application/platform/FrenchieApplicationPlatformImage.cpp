#include <FrenchieApplicationPlatformImage.hpp>

// Core
#include <FrenchieCoreStringUtilities.hpp>
#include <FrenchieCoreStringUnicode.hpp>
#include <FrenchieCoreMemoryCache.hpp>
#include <FrenchieCoreFileSystem.hpp>

// GLAD
#include <glad/glad.h> 

// GLFW
#include <GLFW/glfw3.h>

using namespace Frenchie::Core;
using namespace Frenchie::Core::String;
using namespace Frenchie::Core::FileSystem;

using namespace Frenchie::Application;

namespace Frenchie
{
    namespace Application
    {
        const std::set<std::string> SUPPORTED_IMAGE_FORMATS =
            std::set<std::string>({".png", ".jpg", ".tga", ".bmp"});
    }
}

Image::Image(){}

Image::Image(const std::filesystem::path& _Path)
{
    // try to load image
    if(SUPPORTED_IMAGE_FORMATS.find(utf8_to_lower(get_file_extention(_Path))) == SUPPORTED_IMAGE_FORMATS.end() ||
        !std::filesystem::exists(_Path))
    {
        return;
    }

    // load image as a raw buffer
    stbi_set_flip_vertically_on_load(true);

    std::shared_ptr<unsigned char> buffer = 
        std::shared_ptr<unsigned char>(
            stbi_load(
                convert_utf32_to_utf8(_Path.u32string()).c_str(), &Width, &Height, &Channels, 0),
                [](unsigned char* _Buffer)
                {
                    if(_Buffer != nullptr)
                        stbi_image_free(_Buffer);
                }
        );

    // #define GL_ALPHA 0x1906
    // #define GL_RGB 0x1907
    // #define GL_RGBA 0x1908

    // register image within platform specific low level grphics API
    glGenTextures(1, &Sampler2D);
    glBindTexture(GL_TEXTURE_2D, Sampler2D); 
    
    // set the texture wrapping parameters
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT); // TODO: set texture wrapping to GL_REPEAT (this MUST BE A SETTING !!!)
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
    
    // set texture filtering parameters
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

    glTexImage2D(
        GL_TEXTURE_2D,
        0,
        GL_RGBA, // this also needs to be optional
        Width,
        Height,
        0,
        GL_RGBA,
        GL_UNSIGNED_BYTE,
        buffer.get()
    );

    glGenerateMipmap(GL_TEXTURE_2D); // TODO: this also needs to be optional

    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, Sampler2D);
}

Image::~Image(){}

// unsigned char* Image::get_buffer() const
// {
//     return m_Buffer.get();
// }

// int Image::get_width() const
// {
//     return m_Width;
// }

// int Image::get_height() const
// {
//     return m_Height;
// }

// int Image::get_channels() const
// {
//     return m_Channels;
// }

// bool Image::is_valid() const
// {
//     return m_Buffer != nullptr && m_Width > 0 && m_Height > 0 && m_Channels > 0;
// }

// std::shared_ptr<Image> Frenchie::Core::Renderer::load_image(const std::filesystem::path& _Path)
// {
//     return Frenchie::Core::Memory::Cache<Image>::request(_Path.u32string(), _Path);
// }

// bool Frenchie::Core::Renderer::save_image_as_png(const std::shared_ptr<Image>& _Image, const std::filesystem::path& _Path, const std::u32string& _FileName)
// {
//     if(!std::filesystem::exists(_Path.parent_path()))
//         return false;

//     if(!_Image->is_valid())
//         return false;

//     std::filesystem::path path =
//         std::filesystem::path(_Path.u32string().append(U"/").append(_FileName).append(U".png")).make_preferred();

//     return stbi_write_png(
//         convert_utf32_to_utf8(path.u32string()).c_str(),
//         _Image->get_width(),
//         _Image->get_height(),
//         _Image->get_channels(),
//         _Image->get_buffer(),
//         _Image->get_width() * _Image->get_channels()
//     );
// }

// bool Frenchie::Core::Renderer::save_image_as_jpg(const std::shared_ptr<Image>& _Image, const std::filesystem::path& _Path, const std::u32string& _FileName, const int& _Quality)
// {
//     if(!std::filesystem::exists(_Path.parent_path()))
//         return false;

//     if(!_Image->is_valid())
//         return false;

//     std::filesystem::path path =
//         std::filesystem::path(_Path.u32string().append(U"/").append(_FileName).append(U".jpg")).make_preferred();

//     return stbi_write_jpg(
//         convert_utf32_to_utf8(path.u32string()).c_str(),
//         _Image->get_width(),
//         _Image->get_height(),
//         _Image->get_channels(),
//         _Image->get_buffer(),
//         _Quality
//     );
// }

// bool Frenchie::Core::Renderer::save_image_as_tga(const std::shared_ptr<Image>& _Image, const std::filesystem::path& _Path, const std::u32string& _FileName)
// {
//     if(!std::filesystem::exists(_Path.parent_path()))
//         return false;

//     if(!_Image->is_valid())
//         return false;

//     std::filesystem::path path =
//         std::filesystem::path(_Path.u32string().append(U"/").append(_FileName).append(U".tga")).make_preferred();

//     return stbi_write_tga(
//         convert_utf32_to_utf8(path.u32string()).c_str(),
//         _Image->get_width(),
//         _Image->get_height(),
//         _Image->get_channels(),
//         _Image->get_buffer()
//     );
// }

// bool Frenchie::Core::Renderer::save_image_as_bmp(const std::shared_ptr<Image>& _Image, const std::filesystem::path& _Path, const std::u32string& _FileName)
// {
//     if(!std::filesystem::exists(_Path.parent_path()))
//         return false;

//     if(!_Image->is_valid())
//         return false;

//     std::filesystem::path path =
//         std::filesystem::path(_Path.u32string().append(U"/").append(_FileName).append(U".bmp")).make_preferred();

//     return stbi_write_bmp(
//         convert_utf32_to_utf8(path.u32string()).c_str(),
//         _Image->get_width(),
//         _Image->get_height(),
//         _Image->get_channels(),
//         _Image->get_buffer()
//     );
// }