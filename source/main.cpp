// //---------------------------------------------------------------------------------------------------
// // Editor
// //---------------------------------------------------------------------------------------------------
#include <FrenchieApplicationEditorLauncher.hpp>

int main(int argc, char *argv[])
{
    return Frenchie::Editor::Launcher::execute();
}

// #include <stdio.h>
// #include <math.h>
// #define STB_TRUETYPE_IMPLEMENTATION  // force following include to generate implementation
// #include "stb_truetype.h"

// unsigned char* read_ttf_file(const char* filename, long* file_size) 
// {
//     FILE* file = fopen(filename, "rb");

//     if (file == NULL) 
//     {
//         perror("Error opening file");
//         return NULL;
//     }

//     // Determine file size
//     fseek(file, 0, SEEK_END);
//     *file_size = ftell(file);
//     fseek(file, 0, SEEK_SET);

//     // Allocate memory for the buffer
//     unsigned char* buffer = (unsigned char*)malloc(*file_size);

//     if (buffer == NULL) 
//     {
//         fclose(file);
//         return NULL;
//     }

//     // Read the file into the buffer
//     size_t bytes_read = fread(buffer, 1, *file_size, file);

//     if (bytes_read != *file_size) 
//     {
//         free(buffer);
//         fclose(file);
//         return NULL;
//     }

//     fclose(file);
//     return buffer;
// }

// int check_glyph_existence(stbtt_fontinfo* font_info, int codepoint) 
// {
//     return stbtt_FindGlyphIndex(font_info, codepoint);
// }

// int main(int argc, char **argv)
// {
//    stbtt_fontinfo font;

//    long ttf_buffer_size = 0;
//    unsigned char* ttf_buffer = read_ttf_file(
//     "C:/SDK/Qt_Projects/OpenGL/logs/fonts/forkawesome-webfont.ttf", 
//     &ttf_buffer_size);

//    stbtt_InitFont(&font, ttf_buffer, stbtt_GetFontOffsetForIndex(ttf_buffer,0));

//     for (unsigned int codepoint = 0; codepoint <= 0xFFFF; codepoint++)
//     {
//         if(check_glyph_existence(&font, codepoint))
//         {
//             printf("U+%04X\n", codepoint);
//         }
//     }

//    return 0;
// }