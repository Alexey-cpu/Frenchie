#include <FrenchieApplicationRenderingBackend.hpp>

using namespace Frenchie::Application;

// STB
#include <stb_image.h>
#include <stb_truetype.h>
#include <stb_image_write.h>

// STL
#include <memory>

namespace Frenchie
{
    namespace Application
    {
        // These are STB library helpers borrowed from imgui
        static unsigned int stb_decompress_length(const unsigned char *input)
        {
            return (input[8] << 24) + (input[9] << 16) + (input[10] << 8) + input[11];
        }

        static unsigned char *stb__barrier_out_e, *stb__barrier_out_b;
        static const unsigned char *stb__barrier_in_b;
        static unsigned char *stb__dout;
        static void stb__match(const unsigned char *data, unsigned int length)
        {
            // INVERSE of memmove... write each byte before copying the next...
            GS_ASSERT(stb__dout + length <= stb__barrier_out_e);
            if (stb__dout + length > stb__barrier_out_e) { stb__dout += length; return; }
            if (data < stb__barrier_out_b) { stb__dout = stb__barrier_out_e+1; return; }
            while (length--) *stb__dout++ = *data++;
        }

        static void stb__lit(const unsigned char *data, unsigned int length)
        {
            GS_ASSERT(stb__dout + length <= stb__barrier_out_e);
            if (stb__dout + length > stb__barrier_out_e) { stb__dout += length; return; }
            if (data < stb__barrier_in_b) { stb__dout = stb__barrier_out_e+1; return; }
            memcpy(stb__dout, data, length);
            stb__dout += length;
        }

        #define stb__in2(x)   ((i[x] << 8) + i[(x)+1])
        #define stb__in3(x)   ((i[x] << 16) + stb__in2((x)+1))
        #define stb__in4(x)   ((i[x] << 24) + stb__in3((x)+1))

        static const unsigned char *stb_decompress_token(const unsigned char *i)
        {
            if (*i >= 0x20) { // use fewer if's for cases that expand small
                if (*i >= 0x80)       stb__match(stb__dout-i[1]-1, i[0] - 0x80 + 1), i += 2;
                else if (*i >= 0x40)  stb__match(stb__dout-(stb__in2(0) - 0x4000 + 1), i[2]+1), i += 3;
                else /* *i >= 0x20 */ stb__lit(i+1, i[0] - 0x20 + 1), i += 1 + (i[0] - 0x20 + 1);
            } else { // more ifs for cases that expand large, since overhead is amortized
                if (*i >= 0x18)       stb__match(stb__dout-(stb__in3(0) - 0x180000 + 1), i[3]+1), i += 4;
                else if (*i >= 0x10)  stb__match(stb__dout-(stb__in3(0) - 0x100000 + 1), stb__in2(3)+1), i += 5;
                else if (*i >= 0x08)  stb__lit(i+2, stb__in2(0) - 0x0800 + 1), i += 2 + (stb__in2(0) - 0x0800 + 1);
                else if (*i == 0x07)  stb__lit(i+3, stb__in2(1) + 1), i += 3 + (stb__in2(1) + 1);
                else if (*i == 0x06)  stb__match(stb__dout-(stb__in3(1)+1), i[4]+1), i += 5;
                else if (*i == 0x04)  stb__match(stb__dout-(stb__in3(1)+1), stb__in2(4)+1), i += 6;
            }
            return i;
        }

        static unsigned int stb_adler32(unsigned int adler32, unsigned char *buffer, unsigned int buflen)
        {
            const unsigned long ADLER_MOD = 65521;
            unsigned long s1 = adler32 & 0xffff, s2 = adler32 >> 16;
            unsigned long blocklen = buflen % 5552;

            unsigned long i;
            while (buflen) {
                for (i=0; i + 7 < blocklen; i += 8) {
                    s1 += buffer[0], s2 += s1;
                    s1 += buffer[1], s2 += s1;
                    s1 += buffer[2], s2 += s1;
                    s1 += buffer[3], s2 += s1;
                    s1 += buffer[4], s2 += s1;
                    s1 += buffer[5], s2 += s1;
                    s1 += buffer[6], s2 += s1;
                    s1 += buffer[7], s2 += s1;

                    buffer += 8;
                }

                for (; i < blocklen; ++i)
                    s1 += *buffer++, s2 += s1;

                s1 %= ADLER_MOD, s2 %= ADLER_MOD;
                buflen -= blocklen;
                blocklen = 5552;
            }
            return (unsigned int)(s2 << 16) + (unsigned int)s1;
        }

        static unsigned int stb_decompress(unsigned char *output, const unsigned char *i, unsigned int /*length*/)
        {
            if (stb__in4(0) != 0x57bC0000) return 0;
            if (stb__in4(4) != 0)          return 0; // error! stream is > 4GB
            const unsigned int olen = stb_decompress_length(i);
            stb__barrier_in_b = i;
            stb__barrier_out_e = output + olen;
            stb__barrier_out_b = output;
            i += 16;

            stb__dout = output;
            for (;;) {
                const unsigned char *old_i = i;
                i = stb_decompress_token(i);
                if (i == old_i) {
                    if (*i == 0x05 && i[1] == 0xfa) {
                        GS_ASSERT(stb__dout == output + olen);
                        if (stb__dout != output + olen) return 0;
                        if (stb_adler32(1, output, olen) != (unsigned int) stb__in4(2))
                            return 0;
                        return olen;
                    } else {
                        GS_ASSERT(0); /* NOTREACHED */
                        return 0;
                    }
                }
                GS_ASSERT(stb__dout <= output + olen);
                if (stb__dout > output + olen)
                    return 0;
            }
        }
    }
}

ApplicationRenderingBackendFont                              ApplicationRenderingBackend::m_DefaultFont;
ApplicationRenderingBackendTexture                           ApplicationRenderingBackend::m_DefaultTexture;
std::shared_ptr<ApplicationRenderingBackendGraphicsApiState> ApplicationRenderingBackend::m_GraphicsApiState = nullptr;

ApplicationRenderingBackendFont ApplicationRenderingBackend::get_default_font()
{
    return m_DefaultFont;
}

ApplicationRenderingBackendTexture ApplicationRenderingBackend::get_default_texture()
{
    return m_DefaultTexture;
}

ApplicationRenderingBackendTexture ApplicationRenderingBackend::construct_texture(
    const char*                                        _FilePath,
    const ApplicationRenderingBackendTextureFormat&    _Format,
    const ApplicationRenderingBackendTextureWrapMode&  _Wrap,
    const ApplicationRenderingBackendTextureMinFilter& _MinFilter, 
    const ApplicationRenderingBackendTextureMaxFilter& _MaxFilter)
{
    // auxiliary lambdas
    auto formatToRequestdChannels = [](ApplicationRenderingBackendTextureFormat _Format)->int
    {
        switch (_Format)
        {
        case ApplicationRenderingBackendTextureFormat_::ApplicationRenderingBackendTextureFormat_RGBA:
            return 4;
        
        case ApplicationRenderingBackendTextureFormat_::ApplicationRenderingBackendTextureFormat_RGB:
            return 3;

        case ApplicationRenderingBackendTextureFormat_::ApplicationRenderingBackendTextureFormat_ALPHA:
            return 1;
        }

        return 0; // extract everything by default
    };

    int width   {0};
    int height  {0};
    int channels{0};

    stbi_uc* buffer = stbi_load(_FilePath, &width, &height, &channels, formatToRequestdChannels(_Format));

    if(buffer == nullptr)
        return ApplicationRenderingBackendTexture();

    // construct image
    auto image = ApplicationRenderingBackend::construct_texture(buffer, width, height, _Format, _Wrap, _MinFilter, _MaxFilter);

    // clear raw image buffer
    stbi_image_free(buffer);

    return image;
}

ApplicationRenderingBackendFont ApplicationRenderingBackend::construct_font(const unsigned char* _Memory, const int& _SizeInPixels)
{
    auto load_font_from_memory = [](const unsigned char* fontBuffer)->stbtt_fontinfo*
    {
        // prepare font
        stbtt_fontinfo* info = new stbtt_fontinfo();
        info->userdata = nullptr;
        info->data     = nullptr;

        if (!stbtt_InitFont(info, fontBuffer, stbtt_GetFontOffsetForIndex((unsigned char*)fontBuffer, 0)))
        {
            free(info);
            return nullptr;
        }

        return info;
    };

    // load font file
    std::shared_ptr<stbtt_fontinfo> fontInfo =
        std::shared_ptr<stbtt_fontinfo>(load_font_from_memory(_Memory),
        [](stbtt_fontinfo* _Data)
        {
            if(_Data == nullptr) return;

            if(_Data->data != nullptr)
                free(_Data->data);

            if(_Data->userdata != nullptr)
                free(_Data->userdata);

            delete _Data;
        });
    
    // retrieve available font file characters
    int unicodeMin  = INT_MAX;
    int unicodeMax  = INT_MIN;

    for(unsigned int codepoint = 0; codepoint <= 0xFFFF; codepoint++)
    {
        if(!stbtt_FindGlyphIndex(fontInfo.get(), codepoint))
            continue;

        unicodeMin = std::min<int>(unicodeMin, codepoint);
        unicodeMax = std::max<int>(unicodeMax, codepoint);
    }

    int glyphsCount = unicodeMax - unicodeMin + 1;

    // compute font geometry metrics
    float sizeInPixelsScale = stbtt_ScaleForPixelHeight(fontInfo.get(), (float)_SizeInPixels); // scale font to fit a given font size in pixels
    int   ascent  = 0;                                                                         // distance from the glyph baseline to it's highest point 
    int   descent = 0;                                                                         // distance fron the glyph baseline to it's lowest point
    int   lineGap = 0;                                                                         // recommended gap between text lines

    stbtt_GetFontVMetrics(fontInfo.get(), &ascent, &descent, &lineGap);
    ascent  = (int)roundf((float)ascent  * sizeInPixelsScale);
    descent = (int)roundf((float)descent * sizeInPixelsScale);
    lineGap = (int)roundf((float)lineGap * sizeInPixelsScale);

    // make a most likely large enough bitmap, adjust to font type, number of sizes and glyphs and oversampling
    int atlasWidth  = 32;
    int atlasHeight = 32;

    // pack atlas
    std::shared_ptr<stbtt_packedchar> packedCharacters = 
        std::shared_ptr<stbtt_packedchar>(
            new stbtt_packedchar[glyphsCount],
            [](stbtt_packedchar* _Range)
            {
                if(_Range != nullptr)
                    delete [] _Range;
            }
        );

    std::shared_ptr<unsigned char> atlasBitMap  = nullptr;

    while (true)
    {
        atlasBitMap = std::shared_ptr<unsigned char>(
            (unsigned char*)malloc(atlasWidth * atlasHeight),
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
        stbtt_PackBegin(&pc, atlasBitMap.get(), atlasWidth, atlasHeight, 0, 1, NULL);   
        stbtt_PackSetOversampling(&pc, 1, 1);

        if(!stbtt_PackFontRange(
            &pc,
            fontInfo->data,
            0,
            (float)_SizeInPixels,
            unicodeMin,
            glyphsCount,
            packedCharacters.get()))
        {
            stbtt_PackEnd(&pc);

            if(atlasWidth < 4096)
                atlasWidth *= 2;
            else 
                atlasHeight *= 2;
        } 
        else 
        {
            stbtt_PackEnd(&pc);
            break;
        }
    }

    // generate atlas glyphs quads
    std::shared_ptr<stbtt_aligned_quad> packedCharactersQuads = 
        std::shared_ptr<stbtt_aligned_quad>(
            new stbtt_aligned_quad[glyphsCount],
            [](stbtt_aligned_quad* _Range)
            {
                if(_Range != nullptr)
                    delete [] _Range;
            }
        );

    for (int i = unicodeMin; i <= unicodeMax; i++)
    {
        int idx = i - unicodeMin;

        float unusedX, unusedY;

        stbtt_GetPackedQuad(
            packedCharacters.get(),            // Array of stbtt_packedchar
            atlasWidth,                        // Width of the font atlas texture
            atlasHeight,                       // Height of the font atlas texture
            idx,                               // Index of the glyph
            &unusedX, &unusedY,                // current position of the glyph in screen pixel coordinates, (not required as we have a different corrdinate system)
            &packedCharactersQuads.get()[idx], // stbtt_alligned_quad struct. (this struct mainly consists of the texture coordinates)
            0                                  // Allign X and Y position to a integer (doesn't matter because we are not using 'unusedX' and 'unusedY')
        );
    }

    // retrieve atlas glyphs
    std::shared_ptr<ApplicationRenderingBackendGlyph> glyphs = std::shared_ptr<ApplicationRenderingBackendGlyph>(
        new ApplicationRenderingBackendGlyph[glyphsCount],
        [](ApplicationRenderingBackendGlyph* _Data)
        {
            if(_Data != nullptr)
                delete [] _Data;
        }
    );

    for (int i = unicodeMin; i <= unicodeMax; i++)
    {
        int idx = i - unicodeMin;

        glyphs.get()[idx].Box = gs_2dboxf(
            gs_vec2f(packedCharacters.get()[idx].x0, packedCharacters.get()[idx].y0),
            gs_vec2f(packedCharacters.get()[idx].x1, packedCharacters.get()[idx].y1));
        
        gs_2dboxf BoxUV = gs_2dboxf(
            gs_vec2f(packedCharactersQuads.get()[idx].s0, packedCharactersQuads.get()[idx].t0),
            gs_vec2f(packedCharactersQuads.get()[idx].s1, packedCharactersQuads.get()[idx].t1));

        glyphs.get()[idx].MinUV   = BoxUV.Min;
        glyphs.get()[idx].MaxUV   = BoxUV.Max;
        glyphs.get()[idx].Bearing = gs_vec2f(packedCharacters.get()[idx].xoff, packedCharacters.get()[idx].yoff);
        glyphs.get()[idx].Advance = packedCharacters.get()[idx].xadvance;
    }
    
    // generate font colorified bitmap
    if(atlasBitMap == nullptr)
    {
        return ApplicationRenderingBackendFont(
            _SizeInPixels,
            (float)ascent,
            (float)descent,
            (float)lineGap,
            unicodeMin,
            unicodeMax);
    }

    const int channels = 4;

    std::shared_ptr<unsigned char> colorifiedAtlasBitMap = 
        std::shared_ptr<unsigned char>(
            (unsigned char*)malloc(sizeof(unsigned char) * atlasWidth * atlasHeight * channels),
            [](unsigned char* _Bitmap)
            {
                if(_Bitmap != nullptr)
                    free(_Bitmap);
            }
        );

    for (int y = 0; y < atlasHeight; y++)
    {
        for (int x = 0; x < atlasWidth; x++)
        {
            for (int c = 0; c < channels; c++)
            {
                colorifiedAtlasBitMap.get()[channels * (y * atlasWidth + x) + c] =
                    atlasBitMap.get()[(y * atlasWidth + x)];
            }
        }
    }

    return ApplicationRenderingBackendFont(
        _SizeInPixels,
        (float)ascent,
        (float)descent,
        (float)lineGap,
        unicodeMin,
        unicodeMax,
        glyphs,
        ApplicationRenderingBackend::construct_texture(
            colorifiedAtlasBitMap.get(),
            atlasWidth,
            atlasHeight,
            ApplicationRenderingBackendTextureFormat_::ApplicationRenderingBackendTextureFormat_RGBA)
        );
}

ApplicationRenderingBackendFont ApplicationRenderingBackend::construct_font(const void* _CompressedTTF, const unsigned int& _CompressedTTFSize, const int& _SizeInPixels)
{
    const unsigned int buf_decompressed_size = stb_decompress_length((const unsigned char*)_CompressedTTF);
    unsigned char* buf_decompressed_data = (unsigned char*)malloc(buf_decompressed_size);
    stb_decompress(buf_decompressed_data, (const unsigned char*)_CompressedTTF, (unsigned int)_CompressedTTFSize);

    return construct_font(buf_decompressed_data, _SizeInPixels);
}

ApplicationRenderingBackendFont ApplicationRenderingBackend::construct_font(const char* _FilePath, const int& _SizeInPixels)
{
    auto stb_open_ttf_file = [](const char* _FilePathUTF8)->unsigned char*
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

        return fontBuffer;
    };

    std::shared_ptr<unsigned char> fontBuffer = std::shared_ptr<unsigned char>(
        stb_open_ttf_file(_FilePath),
        [](unsigned char* _Data)
        {
            if(_Data != nullptr)
                free(_Data);
        }
    );

    return construct_font(fontBuffer.get(), _SizeInPixels);
}

void ApplicationRenderingBackend::destroy_font(const ApplicationRenderingBackendFont& _Font)
{
    ApplicationRenderingBackend::destroy_texture(_Font.AtlasTexture);
}