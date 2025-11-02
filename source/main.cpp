// //---------------------------------------------------------------------------------------------------
// // Editor
// //---------------------------------------------------------------------------------------------------
// #include <FrenchieApplicationEditorLauncher.hpp>

// int main(int argc, char *argv[])
// {
//     return Frenchie::Editor::Launcher::execute();
// }

#include <FrenchieCoreImage.hpp>

int main(int argc, char *argv[])
{
    auto image = Frenchie::Core::Image::load_image(
        "C:/Users/User/Desktop/vscode_icons/vscode-icons-master/png/default_file.png");

    std::cout << "image info: \n";
    std::cout << image->get_width() << "\n";
    std::cout << image->get_height() << "\n";
    std::cout << image->get_channels() << "\n";

    Frenchie::Core::Image::save_image_as_png(image, "C:/SDK/Qt_Projects/OpenGL/logs/images/", U"image");
    Frenchie::Core::Image::save_image_as_bmp(image, "C:/SDK/Qt_Projects/OpenGL/logs/images/", U"image");
    Frenchie::Core::Image::save_image_as_jpg(image, "C:/SDK/Qt_Projects/OpenGL/logs/images/", U"image");
    Frenchie::Core::Image::save_image_as_tga(image, "C:/SDK/Qt_Projects/OpenGL/logs/images/", U"image");

    return 1;
}

// /* Created By: Justin Meiners (2013) */
// #include <cmath>
// #include <stdio.h>
// #include <stdlib.h>
// #include <cstring>
// #include <string>

// #include <iostream>

// #include "stb_image_write.h" /* http://nothings.org/stb/stb_image_write.h */
// #include "stb_truetype.h" /* http://nothings.org/stb/stb_truetype.h */

// void stb_free_font_info(stbtt_fontinfo* _Info)
// {
//     if(_Info == nullptr)
//         return;

//     if(_Info->data != nullptr)
//         free(_Info->data);

//     if(_Info->userdata != nullptr)
//         free(_Info->userdata);
// }

// stbtt_fontinfo stb_open_ttf_file(const char* _FilePathUTF8)
// {
//     // load font file
//     long size;
//     unsigned char* fontBuffer;
    
//     FILE* fontFile = fopen(_FilePathUTF8, "rb");
//     fseek(fontFile, 0, SEEK_END);
//     size = ftell(fontFile);
//     fseek(fontFile, 0, SEEK_SET);
    
//     fontBuffer = (unsigned char*)malloc(size);
    
//     fread(fontBuffer, size, 1, fontFile);
//     fclose(fontFile);

//     // prepare font
//     stbtt_fontinfo info;

//     if (!stbtt_InitFont(&info, fontBuffer, 0))
//     {
//         info.userdata = nullptr;
//         info.data     = nullptr;
//         free(fontBuffer);
//         return stbtt_fontinfo();
//     }

//     return info;
// }

// float stb_get_text_line_width(stbtt_fontinfo* _FontInfo, const char* _UTF_8, const int& _Length, float _FontScale) 
// {
//     float total_width  = 0.0f;
//     int prev_codepoint = 0; // For kerning

//     for (int i = 0; i < _Length; ++i) 
//     {
//         int codepoint = _UTF_8[i]; // Assuming ASCII or easily convertible codepoints

//         // Get horizontal metrics for the current character
//         int advance_width, left_side_bearing;
//         stbtt_GetCodepointHMetrics(_FontInfo, codepoint, &advance_width, &left_side_bearing);

//         // Add kerning if applicable
//         if (prev_codepoint != 0) 
//         {
//             total_width += stbtt_GetCodepointKernAdvance(_FontInfo, prev_codepoint, codepoint);
//         }

//         // Add the scaled advance width
//         total_width += advance_width;

//         prev_codepoint = codepoint;
//     }

//     return total_width *= _FontScale; // Apply the overall scale
// }

// int main(int argc, const char * argv[])
// {
//     auto info = stb_open_ttf_file("C:/SDK/Qt_Projects/OpenGL/shared/appData/fonts/Alice-Regular.ttf");

//     std::string str = "the quick brown fox the quick\nbrown fox the quick brown fox";

//     const char* word = str.c_str();
    
//     float pixels = 64.f;
//     float scale  = stbtt_ScaleForPixelHeight(&info, pixels);

//     int ascent, descent, lineGap;
//     stbtt_GetFontVMetrics(&info, &ascent, &descent, &lineGap);
    
//     ascent  = roundf(ascent * scale);
//     descent = roundf(descent * scale);
//     lineGap = roundf(lineGap * scale);

//     int b_w = stb_get_text_line_width(&info, word, strlen(word), scale);
//     int b_h = (ascent - descent + lineGap) + pixels;

//     // create a bitmap for the phrase
//     unsigned char* bitmap = (unsigned char*)calloc(b_w * b_h, sizeof(unsigned char));
    
//     int x = 0;
    
//     int i;
//     for (i = 0; i < strlen(word); ++i)
//     {
//         /* how wide is this character */
//         int advance_width;
// 	    int left_side_bearing;
//         stbtt_GetCodepointHMetrics(&info, word[i], &advance_width, &left_side_bearing);
//         // (Note that each Codepoint call has an alternative Glyph version which caches the work required to lookup the character word[i].)

//         // get bounding box for character (may be offset to account for chars that dip above or below the line)
//         int c_x1, c_y1, c_x2, c_y2;
//         stbtt_GetCodepointBitmapBox(&info, word[i], scale, scale, &c_x1, &c_y1, &c_x2, &c_y2);
        
//         // compute y (different characters have different heights)
//         int y = (ascent - descent + lineGap) + c_y1;
        
//         // render character (stride and offset is important here)
//         int byteOffset = x + roundf(left_side_bearing * scale) + (y * b_w);
//         stbtt_MakeCodepointBitmap(&info, bitmap + byteOffset, c_x2 - c_x1, c_y2 - c_y1, b_w, scale, scale, word[i]);

//         // advance x
//         x += roundf(advance_width * scale);
        
//         // add kerning
//         int kern;
//         kern = stbtt_GetCodepointKernAdvance(&info, word[i], word[i + 1]);
//         x += roundf(kern * scale);
//     }
    
//     // save out a 1 channel image
//     stbi_write_png("out.png", b_w, b_h, 1, bitmap, b_w);
    
//     free(bitmap);
    
//     return 0;
// }