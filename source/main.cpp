// //---------------------------------------------------------------------------------------------------
// // Editor
// //---------------------------------------------------------------------------------------------------
// #include <FrenchieApplicationEditorLauncher.hpp>

// int main(int argc, char *argv[])
// {
//     return Frenchie::Editor::Launcher::execute();
// }

// IMAGE BACKEND API
// #include <FrenchieCoreImage.hpp>

// int main(int argc, char *argv[])
// {
//     auto image = Frenchie::Core::Image::load_image(
//         "C:/Users/User/Desktop/vscode_icons/vscode-icons-master/png/default_file.png");

//     std::cout << "image info: \n";
//     std::cout << image->get_width() << "\n";
//     std::cout << image->get_height() << "\n";
//     std::cout << image->get_channels() << "\n";

//     Frenchie::Core::Image::save_image_as_png(image, "C:/SDK/Qt_Projects/OpenGL/logs/images/", U"image");
//     Frenchie::Core::Image::save_image_as_bmp(image, "C:/SDK/Qt_Projects/OpenGL/logs/images/", U"image");
//     Frenchie::Core::Image::save_image_as_jpg(image, "C:/SDK/Qt_Projects/OpenGL/logs/images/", U"image");
//     Frenchie::Core::Image::save_image_as_tga(image, "C:/SDK/Qt_Projects/OpenGL/logs/images/", U"image");

//     return 1;
// }

// TTF BACKEND API
// Created By: Justin Meiners (2013)
#include <cmath>
#include <stdio.h>
#include <stdlib.h>
#include <cstring>
#include <string>
#include <vector>

#include <iostream>

#include "stb_image_write.h" /* http://nothings.org/stb/stb_image_write.h */
#include "stb_truetype.h" /* http://nothings.org/stb/stb_truetype.h */

#include <FrenchieCoreTime.hpp>

void stb_free_font_info(stbtt_fontinfo* _Info)
{
    if(_Info == nullptr)
        return;

    if(_Info->data != nullptr)
        free(_Info->data);

    if(_Info->userdata != nullptr)
        free(_Info->userdata);
}

unsigned char* stb_load_font_to_buffer(const char* _FilePathUTF8, int* _Size)
{
    // load font file
    long size;
    unsigned char* fontBuffer;
    
    FILE* fontFile = fopen(_FilePathUTF8, "rb");
    fseek(fontFile, 0, SEEK_END);
    size = ftell(fontFile);
    fseek(fontFile, 0, SEEK_SET);
    
    fontBuffer = (unsigned char*)malloc(size);
    
    fread(fontBuffer, size, 1, fontFile);
    fclose(fontFile);

    *_Size = size;

    return fontBuffer;
}

stbtt_fontinfo stb_open_ttf_file(const char* _FilePathUTF8)
{
    // load font file
    long size;
    unsigned char* fontBuffer;
    
    FILE* fontFile = fopen(_FilePathUTF8, "rb");
    fseek(fontFile, 0, SEEK_END);
    size = ftell(fontFile);
    fseek(fontFile, 0, SEEK_SET);
    
    fontBuffer = (unsigned char*)malloc(size);
    
    fread(fontBuffer, size, 1, fontFile);
    fclose(fontFile);

    // prepare font
    stbtt_fontinfo info;

    if (!stbtt_InitFont(&info, fontBuffer, 0))
    {
        info.userdata = nullptr;
        info.data     = nullptr;
        free(fontBuffer);
        return stbtt_fontinfo();
    }

    return info;
}

int stb_get_text_line_width(stbtt_fontinfo* _FontInfo, const char* _Utf8Text, float _FontScale) 
{
    int x = 0;
    int previousCodepoint = 0;

    for (int codePointIndex = 0; codePointIndex < strlen(_Utf8Text); ++codePointIndex)
    {
        int codepoint = _Utf8Text[codePointIndex];

        // compute glyph horizontal metrics
        int glyphAdvanceWidth    = 0; // The distance to advance the cursor after rendering the glyph.
	    int glyphLeftSideBearing = 0; // The horizontal offset from the origin to the left edge of the glyph's bounding box
        
        int glyph_kern = // The adjustment of the horizontal space between individual glyph pairs to ensure they look balanced
            previousCodepoint != 0 ? stbtt_GetCodepointKernAdvance(_FontInfo, previousCodepoint, codepoint) : 0;

        stbtt_GetCodepointHMetrics(
            _FontInfo,
            codepoint,
            &glyphAdvanceWidth,
            &glyphLeftSideBearing
        );
        
        // add kerning
        x += (int)roundf(glyphAdvanceWidth * _FontScale) + (int)roundf(glyph_kern * _FontScale);

        previousCodepoint = codepoint;
    }

    return x;
}

void render_colored_text_example()
{
    // load .ttf
    auto fontInfo = stb_open_ttf_file(
        "C:/SDK/Qt_Projects/OpenGL/shared/appData/fonts/Alice-Regular.ttf");

    // scale font to fit a given font size in pixels
    float fontSizeInPixels   = 64.f;
    float fontPixelSizeScale = stbtt_ScaleForPixelHeight(&fontInfo, fontSizeInPixels);

    // retrieve absolute font vertical metrics
    int fontAscent  = 0; // distance from the glyph baseline to it's highest point 
    int fontDescent = 0; // distance fron the glyph baseline to it's lowest point
    int fontLineGap = 0; // recommended gap between text lines
    stbtt_GetFontVMetrics(&fontInfo, &fontAscent, &fontDescent, &fontLineGap);
    
    // compute scaled font vertical metrics
    fontAscent  = (int)roundf((float)fontAscent  * fontPixelSizeScale);
    fontDescent = (int)roundf((float)fontDescent * fontPixelSizeScale);
    fontLineGap = (int)roundf((float)fontLineGap * fontPixelSizeScale);

    // try to render text info an image
    std::string text = "the quick brown fox the quick";

    // compute text size
    int textWidth    = stb_get_text_line_width(&fontInfo, text.c_str(), fontPixelSizeScale);
    int textHeight   = fontAscent - fontDescent;
    int textChannels = 4;

    // generate bitmap for the text
    unsigned char* bitmap = (unsigned char*)calloc(textWidth * textHeight, sizeof(unsigned char));
    
    int x = 0;
    int y = 0;

    int previous_codepoint = 0; // For kerning

    for (int i = 0; i < strlen(text.c_str()); ++i)
    {
        int codepoint = text.c_str()[i];

        // compute glyph horizontal metrics
        int glyphAdvanceWidth    = 0; // The distance to advance the cursor after rendering the glyph.
	    int glyphLeftSideBearing = 0; // The horizontal offset from the origin to the left edge of the glyph's bounding box

        int glyph_kern = // The adjustment of the horizontal space between individual glyph pairs to ensure they look balanced
            previous_codepoint != 0 ? stbtt_GetCodepointKernAdvance(&fontInfo, previous_codepoint, codepoint) : 0;

        stbtt_GetCodepointHMetrics(
            &fontInfo,
            codepoint,
            &glyphAdvanceWidth,
            &glyphLeftSideBearing
        );

        // compute glyph bounding box
        int glyphXmin = 0;
        int glyphYmin = 0;
        int glyphXmax = 0;
        int glyphYmax = 0;

        stbtt_GetCodepointBitmapBox(
            &fontInfo,
            codepoint,
            fontPixelSizeScale,
            fontPixelSizeScale,
            &glyphXmin,
            &glyphYmin,
            &glyphXmax,
            &glyphYmax
        );

        int glyphWidth  = (glyphXmax - glyphXmin);
        int glyphHeight = (glyphYmax - glyphYmin);

        // render character (stride and offset is important here)
        int glyphByteOffset = x + (int)roundf(glyphLeftSideBearing * fontPixelSizeScale) + (fontAscent + glyphYmin) * textWidth;

        stbtt_MakeCodepointBitmap(
            &fontInfo,
            bitmap + glyphByteOffset,
            glyphWidth,
            glyphHeight,
            textWidth,
            fontPixelSizeScale,
            fontPixelSizeScale,
            codepoint
        );
        
        // add kerning
        x += (int)roundf(glyphAdvanceWidth * fontPixelSizeScale) + (int)roundf(glyph_kern * fontPixelSizeScale);

        previous_codepoint = codepoint;
    }

    // colorify text
    int red   = 0;
    int green = 1;
    int blue  = 2;
    int alpha = 3;

    unsigned char* image =
        (unsigned char*)calloc(textWidth * textHeight * textChannels, sizeof(unsigned char));

    for (int y = 0; y < textHeight; y++)
    {
        for (int x = 0; x < textWidth; x++)
        {
            image[textChannels * (y * textWidth + x) + red  ] = 128;
            image[textChannels * (y * textWidth + x) + green] = 0;
            image[textChannels * (y * textWidth + x) + blue ] = 0;
            image[textChannels * (y * textWidth + x) + alpha] = bitmap[y * textWidth + x];
        }
    }

    // save out a 1 channel image
    stbi_write_png("out.png", textWidth, textHeight, textChannels, image, textWidth * textChannels);
    
    free(bitmap);
    free(image);
}

// Core
#include <FrenchieCoreStringUnicode.hpp>
#include <FrenchieCoreMemoryCache.hpp>

// STL
#include <filesystem>

namespace Fonts
{
    class Font;
    class Atlas;

    // RAII font
    class Font
    {
    public:
        Font(const std::filesystem::path& _Path, const int& _SizeInPixels = 12) : m_SizeInPixels(_SizeInPixels)
        {
            // load font file
            FILE* fontFile = fopen(Frenchie::Core::String::convert_utf32_to_utf8(_Path.u32string()).c_str(), "rb");

            if(!fontFile)
                return;

            // determine font file size
            fseek(fontFile, 0, SEEK_END);
            long fontFileSize = ftell(fontFile);
            fseek(fontFile, 0, SEEK_SET);
            
            // read font file contents into a buffer
            unsigned char* fontBuffer =
                (unsigned char*)malloc(fontFileSize * sizeof(unsigned char));
            
            fread(fontBuffer, fontFileSize, 1, fontFile);
            fclose(fontFile);

            // prepare font
            m_Info = std::shared_ptr<stbtt_fontinfo>(
                (stbtt_fontinfo*)malloc(sizeof(stbtt_fontinfo)),
                [this](stbtt_fontinfo* _Info)
                {
                    if(_Info == nullptr)
                        return;
                    
                    free(_Info->data);
                    free(_Info->userdata);
                    free(_Info);
                }
            );

            m_Info->data     = nullptr;
            m_Info->userdata = nullptr;

            if (!stbtt_InitFont(m_Info.get(), fontBuffer, 0))
            {
                free(fontBuffer);
                return;
            }

            for(unsigned int codepoint = 0; codepoint <= 0xFFFF; codepoint++)
            {
                if(!stbtt_FindGlyphIndex(m_Info.get(), codepoint))
                    continue;

                m_UnicodeMin = std::min<int>(m_UnicodeMin, codepoint);
                m_UnicodeMax = std::max<int>(m_UnicodeMax, codepoint);
            }

            m_IsDirty = true;
        }

        ~Font(){}

        // getters
        float get_max_font_size() const
        {
            return m_SizeMaximum;
        }

        float get_min_font_size() const
        {
            return m_SizeMinimum;
        }

        unsigned char* get_font_buffer() const
        {
            return m_Info != nullptr ? m_Info->data : nullptr;
        }

        int get_size_in_pixels() const
        {
            return m_SizeInPixels;
        }

        float get_pixel_size_scale() const
        {
            if(m_IsDirty) on_dirty();
            return m_SizeInPixelScale;
        }

        int get_ascent() const
        {
            if(m_IsDirty) on_dirty();
            return m_Ascent;
        }

        int get_descent() const
        {
            if(m_IsDirty) on_dirty();
            return m_Descent;
        }

        int get_linegap() const
        {
            if(m_IsDirty) on_dirty();
            return m_LineGap;
        }

        int get_unciode_min() const
        {
            return m_UnicodeMin;
        }

        int get_unciode_max() const
        {
            return m_UnicodeMax;
        }

        int is_dirty() const
        {
            return m_IsDirty;
        }

        std::shared_ptr<Atlas> get_atlas() const
        {
            return m_Atlas;
        }

        const std::shared_ptr<stbtt_fontinfo> get_info() const
        {
            return m_Info;
        }

        // setters
        void set_size_in_pixels(const int& _Size)
        {
            m_SizeInPixels = _Size;
            m_IsDirty      = true;
        }

    protected:

        void on_dirty() const
        {
            // compute font pixel size scale
            m_SizeInPixelScale = stbtt_ScaleForPixelHeight(m_Info.get(), (float)m_SizeInPixels);

            // compute font vertical metrics
            stbtt_GetFontVMetrics(m_Info.get(), &m_Ascent, &m_Descent, &m_LineGap);

            m_Ascent  = (int)roundf((float)m_Ascent  * m_SizeInPixelScale);
            m_Descent = (int)roundf((float)m_Descent * m_SizeInPixelScale);
            m_LineGap = (int)roundf((float)m_LineGap * m_SizeInPixelScale);
            m_IsDirty = false;
        }

        // info
        mutable float                           m_SizeMaximum      = 512.f;   // maximum font size
        mutable float                           m_SizeMinimum      = 2.f;     // minimum font size
        mutable int                             m_SizeInPixels     = 12;      // font size in pixels
        mutable float                           m_SizeInPixelScale = 1.f;     // scale font to fit a given font size in pixels
        mutable int                             m_Ascent           = 0;       // distance from the glyph baseline to it's highest point 
        mutable int                             m_Descent          = 0;       // distance fron the glyph baseline to it's lowest point
        mutable int                             m_LineGap          = 0;       // recommended gap between text lines
        mutable int                             m_UnicodeMin       = INT_MAX; // starting Unicode symbol
        mutable int                             m_UnicodeMax       = INT_MIN; // ending Unicode symbol
        mutable bool                            m_IsDirty          = true;    // dirty flag
        mutable std::shared_ptr<Atlas>          m_Atlas            = nullptr; // STB true type font atlas
        mutable std::shared_ptr<stbtt_fontinfo> m_Info             = nullptr; // STB true type font info
    };

    class Atlas final
    {
    public:

        Atlas(const Font* _Font)
        {
            if(_Font == nullptr)
                return;

            m_Chars = std::shared_ptr<stbtt_packedchar>(
                (stbtt_packedchar*)malloc(sizeof(stbtt_packedchar) * (_Font->get_unciode_max() - _Font->get_unciode_min())),
                [](stbtt_packedchar* _Range)
                {
                    if(_Range != nullptr)
                        free(_Range);
                }
            );

            // make a most likely large enough bitmap, adjust to font type, number of sizes and glyphs and oversampling
            int atlasWidth     = 0;
            int atlasHeight    = 0;
            int atlasMaxHeight = 16;
            int atlasMaxWidth  = 32;
            
            while (true)
            {
                atlasWidth  = 0;
                atlasHeight = 0;
                
                for (int codepoint = _Font->get_unciode_min(); codepoint < _Font->get_unciode_max();)
                {
                    int width = 0;
                    int maxHeight = INT_MIN;

                    while (true)
                    {
                        // compute glyph bounding box
                        int glyphXmin = 0;
                        int glyphYmin = 0;
                        int glyphXmax = 0;
                        int glyphYmax = 0;

                        stbtt_GetCodepointBitmapBox(
                            _Font->get_info().get(),
                            codepoint,
                            _Font->get_pixel_size_scale(),
                            _Font->get_pixel_size_scale(),
                            &glyphXmin,
                            &glyphYmin,
                            &glyphXmax,
                            &glyphYmax
                        );

                        int glyphWidth  = (glyphXmax - glyphXmin);
                        int glyphHeight = (glyphYmax - glyphYmin);

                        width    += std::max<int>(1, glyphWidth);
                        maxHeight = std::max<int>(glyphHeight, maxHeight);

                        ++codepoint;

                        if(width >= atlasMaxWidth)
                        {
                            width -= std::max<int>(1, glyphWidth);
                            width = atlasMaxWidth;
                            break;
                        }
                        else
                        {
                        }
                    }

                    atlasWidth  = std::max<int>(atlasWidth, width);
                    atlasHeight += std::max<int>(maxHeight, 1);
                }

                atlasMaxWidth *= 2;
                atlasMaxWidth  = std::min<int>(atlasMaxWidth, 2048);

                if(atlasHeight <= atlasMaxHeight)
                    break;

                atlasMaxHeight *= 2;
                atlasMaxHeight  = std::min<int>(atlasMaxHeight, 2048);
            }

            // pack atlas
            m_Bitmap        = nullptr;
            m_TextureWidth  = atlasWidth;
            m_TextureHeight = atlasHeight;

            while (true)
            {
                m_Bitmap = std::shared_ptr<unsigned char>(
                    (unsigned char*)malloc(m_TextureWidth * m_TextureHeight),
                    [](unsigned char* _Bitmap)
                    {
                        if(_Bitmap != nullptr)
                        {
                            free(_Bitmap);
                            _Bitmap = nullptr;
                        }
                    }
                );

                stbtt_pack_context pc;
                stbtt_PackBegin(&pc, m_Bitmap.get(), m_TextureWidth, m_TextureHeight, 0, 1, NULL);   
                stbtt_PackSetOversampling(&pc, 1, 1);

                if(!stbtt_PackFontRange(
                    &pc,
                    _Font->get_font_buffer(),
                    0,
                    (float)_Font->get_size_in_pixels(),
                    _Font->get_unciode_min(),
                    _Font->get_unciode_max() - _Font->get_unciode_min(),
                    m_Chars.get()))
                {
                    stbtt_PackEnd(&pc);

                    if(m_TextureWidth < 4096)
                        m_TextureWidth *= 2;
                    else 
                        m_TextureHeight *= 2;
                } 
                else 
                {
                    stbtt_PackEnd(&pc);
                    break;
                }
            }
        }
        
        ~Atlas(){}

        std::shared_ptr<unsigned char>    m_Bitmap        = nullptr;
        std::shared_ptr<stbtt_packedchar> m_Chars         = nullptr;
        int                               m_TextureWidth  = 32;
        int                               m_TextureHeight = 32;
    };
}

int main(int argc, const char * argv[])
{
    std::filesystem::path path("C:/SDK/Qt_Projects/OpenGL/shared/appData/fonts/Alice-Regular.ttf");

    auto font  = Frenchie::Core::Memory::Cache<Fonts::Font>::request(path.u32string(), path, 128);

    auto then = Frenchie::Core::Time::tic();

    auto atlas = Frenchie::Core::Memory::Cache<Fonts::Atlas>::request(
        U"Alice-Regular-Atlas", font.get());

    std::cout << Frenchie::Core::Time::elapsed<std::chrono::milliseconds>(then, Frenchie::Core::Time::tic()) << "ms \n";

    stbi_write_png("atlas.png",
        atlas->m_TextureWidth,
        atlas->m_TextureHeight,
        1,
        atlas->m_Bitmap.get(),
        atlas->m_TextureWidth);

    return 0;
}