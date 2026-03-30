#pragma once

// Core
#include <FrenchieCoreStringUnicode.hpp>
#include <FrenchieCoreMemoryCache.hpp>

// STL
#include <filesystem>

// STB
#include "stb_image_write.h"
#include "stb_truetype.h"

namespace Frenchie
{
    namespace Core
    {
        namespace Renderer
        {
            class Font;
            class FontAtlas;

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

                std::shared_ptr<FontAtlas> get_atlas() const
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
                mutable std::shared_ptr<FontAtlas>      m_Atlas            = nullptr; // STB true type font atlas
                mutable std::shared_ptr<stbtt_fontinfo> m_Info             = nullptr; // STB true type font info
            };

            class FontAtlas final
            {
            public:

                FontAtlas(const Font* _Font)
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
                
                ~FontAtlas(){}

                std::shared_ptr<unsigned char>    m_Bitmap        = nullptr;
                std::shared_ptr<stbtt_packedchar> m_Chars         = nullptr;
                int                               m_TextureWidth  = 32;
                int                               m_TextureHeight = 32;
            };
        }
    }
}