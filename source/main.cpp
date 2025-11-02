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

#include <iostream>

#include "stb_image_write.h" /* http://nothings.org/stb/stb_image_write.h */
#include "stb_truetype.h" /* http://nothings.org/stb/stb_truetype.h */

void stb_free_font_info(stbtt_fontinfo* _Info)
{
    if(_Info == nullptr)
        return;

    if(_Info->data != nullptr)
        free(_Info->data);

    if(_Info->userdata != nullptr)
        free(_Info->userdata);
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

    return (int)roundf(x);
}

int main(int argc, const char * argv[])
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

    std::cout << "fontAscent  " << fontAscent << "\n";
    std::cout << "fontDescent " << fontDescent << "\n";
    std::cout << "fontLineGap " << fontLineGap << "\n";
    std::cout << "textWidth  "  << textWidth << "\n";
    std::cout << "textHeight "  << textHeight << "\n";

    int previous_codepoint = 0; // For kerning

    for (int i = 0; i < strlen(text.c_str()); ++i)
    {
        int codepoint = text.c_str()[i];

        // compute glyph horizontal metrics
        int glyphAdvanceWidth     = 0; // The distance to advance the cursor after rendering the glyph.
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

        //std::cout << "glyphByteOffset " << glyphByteOffset << "\n";

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
    
    unsigned char* image = (unsigned char*)calloc(textWidth * textHeight * textChannels, sizeof(unsigned char));

    for (int y = 0; y < textHeight; y++)
    {
        for (int x = 0; x < textWidth; x++)
        {
            image[textChannels * (y * textWidth + x) + 0] = 255;
            image[textChannels * (y * textWidth + x) + 1] = 0;
            image[textChannels * (y * textWidth + x) + 2] = 0;
            image[textChannels * (y * textWidth + x) + 3] = bitmap[y * textWidth + x];
        }
    }

    // save out a 1 channel image
    //stbi_write_png("out.png", textWidth, textHeight, 1, bitmap, textWidth);
    stbi_write_png("out.png", textWidth, textHeight, textChannels, image, textWidth * textChannels);
    
    free(bitmap);
    free(image);
    
    return 0;
}