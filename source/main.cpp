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

struct FontGeometry
{
    float PixelSizeScale = 1.f; // scale font to fit a given font size in pixels
    int   fontAscent     = 0;   // distance from the glyph baseline to it's highest point 
    int   fontDescent    = 0;   // distance fron the glyph baseline to it's lowest point
    int   fontLineGap    = 0;   // recommended gap between text lines
    int   UnicodeMin     = INT_MAX;
    int   UnicodeMax     = INT_MIN;

};

struct Font
{
    stbtt_fontinfo* info;
};


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

// #define NUM_SIZES  16
// #define NUM_GLYPHS glyphsCount

int main(int argc, const char * argv[])
{
    // load .ttf font to a buffer
    int ttf_buffer_size = 0;
    unsigned char* ttf_buffer = stb_load_font_to_buffer("C:/SDK/Qt_Projects/OpenGL/shared/appData/fonts/Alice-Regular.ttf", &ttf_buffer_size);

    // prepare font
    stbtt_fontinfo fontInfo;
    if (!stbtt_InitFont(&fontInfo, ttf_buffer, 0))
    {
        fontInfo.userdata = nullptr;
        fontInfo.data     = nullptr;
        free(ttf_buffer);
        return -1;
    }

    // estimate appoximate atlas size
    int MaxFontSize  = 72;
    int MinFontSize  = 12;
    int glyphsCount  = 0;
    int fontsCount   = 0;
    int firstUnicode = -1;

    glyphsCount = 0;

    // count font sizes
    for (int fontSize = MaxFontSize, fontNumber = 0; fontSize >= MinFontSize; fontSize -= 4, fontsCount++);

    // count available glyphs
    for (unsigned int codepoint = 0; codepoint <= 0xFFFF; codepoint++)
    {
        if(!stbtt_FindGlyphIndex(&fontInfo, codepoint))
            continue;

        if(firstUnicode < 0)
            firstUnicode = codepoint;

        glyphsCount++;
    }

    // std::cout << "atlasWidth  " << atlasWidth << "\n";
    // std::cout << "atlasHeight " << atlasHeight << "\n";
    std::cout << "fontsCount  " << fontsCount << "\n";
    std::cout << "glyphsCount " << glyphsCount << "\n";

    // setup glyph info stuff, check stb_truetype.h for definition of structs
    stbtt_pack_range* ranges = (stbtt_pack_range*)malloc(fontsCount*sizeof(stbtt_pack_range));

    for (int fontSize = MaxFontSize, fontNumber = 0; fontSize >= MinFontSize; fontSize -= 4, fontNumber++)
    {
        stbtt_pack_range range;
        range.font_size                        = (float)fontSize;
        range.first_unicode_codepoint_in_range = firstUnicode;
        range.array_of_unicode_codepoints      = NULL;
        range.num_chars                        = glyphsCount;
        range.chardata_for_range               = (stbtt_packedchar*)malloc(glyphsCount*sizeof(stbtt_packedchar));
        ranges[fontNumber]                     = range;
    }

    // make a most likely large enough bitmap, adjust to font type, number of sizes and glyphs and oversampling
    int textureSize  = 128;

    unsigned char *bitmap = NULL;
    
    while (true)
    {
        bitmap = (unsigned char*)malloc(textureSize * textureSize);
        stbtt_pack_context pc;
        stbtt_PackBegin(&pc, bitmap, textureSize, textureSize, 0, 1, NULL);   
        stbtt_PackSetOversampling(&pc, 1, 1); // say, choose 3x1 oversampling for subpixel positioning
        std::cout << "texture size " << textureSize << "\n";

        if(!stbtt_PackFontRanges(&pc, ttf_buffer, 0, ranges, fontsCount))
        {
			free(bitmap);
			stbtt_PackEnd(&pc);
			textureSize *= 2;
		} 
        else 
        {
			stbtt_PackEnd(&pc);
			break;
		}
    }

    stbi_write_png("atlas.png", textureSize, textureSize, 1, bitmap, textureSize);

    // cleanup
    free(ttf_buffer);
    free(bitmap);
    free(ranges);

    return 0;
}