// //---------------------------------------------------------------------------------------------------
// // Editor
// //---------------------------------------------------------------------------------------------------
#include <FrenchieApplicationEditorLauncher.hpp>
#include <FrenchieCoreRenderer2DLayer.hpp>
#include <FrenchieCoreTime.hpp>

#include <FrenchieCoreContainersObjectList.hpp>

class StringNode : public Frenchie::Core::Containers::ObjectTreeRoot<StringNode>
{
public:
    StringNode(const std::string& _Value = std::string()) : Value(_Value){}
    virtual ~StringNode(){}

    std::string Value{std::string()};
};

// int main(int argc, char *argv[])
// {
//     return 0;
// }

// int main(int argc, char *argv[])
// {
//     // setup application logger
//     Frenchie::Core::Logger::instance()->set_level(spdlog::level::level_enum::trace);

//     try
//     {
//         Frenchie::Core::Logger::instance()->register_sink<spdlog::sinks::stdout_color_sink_mt>();
//     }
//     catch(const std::exception& e)
//     {
//         Frenchie::Core::Logger::instance()->info(e.what());
//     }


//     Frenchie::Application::application()->push_layer<Frenchie::Application::Renderer2D>();

//     return Frenchie::Application::application()->execute();
// }

int main(int argc, char *argv[])
{
    // StringNode nodes;

    // nodes.insert_after(nodes.begin(), "A");
    // nodes.insert_after(nodes.begin(), "B");
    // nodes.insert_after(nodes.begin(), "C");

    // for(auto&& node : nodes) std::cout << node->Value << "\n";

    Frenchie::Core::Serialization::Document document;

    auto then = Frenchie::Core::Time::tic();

    document.read<Frenchie::Core::Serialization::XMLReader>(
        std::filesystem::path(U"C:/SDK/Qt_Projects/OpenGL/logs/1_Летний минимум_2027_ГОСТ_п.5.3_Г.pwrct")
    );

    std::cout << "elapsed " << Frenchie::Core::Time::elapsed<std::chrono::milliseconds>(then, Frenchie::Core::Time::tic()) << " ms \n";

    return 0;//Frenchie::Editor::Launcher::execute();
}

// #include "miniz.h"
// #include <cstdio>
// #include <string>
// #include <vector>

// int main() {
//     const char* zip_filename = "my_archive.zip";
//     const char* file1_name = "file1.txt";
//     const char* file2_name = "file2.bin";

//     // Data for the files
//     std::string file1_content = "This is the content of the first file.";
//     std::vector<unsigned char> file2_content = {0x01, 0x02, 0x03, 0x04, 0x05};

//     mz_zip_archive zip_archive;
//     memset(&zip_archive, 0, sizeof(zip_archive));

//     // Initialize the ZIP archive writer
//     if (!mz_zip_writer_init_file(&zip_archive, zip_filename, 0)) {
//         printf("Error initializing ZIP archive writer: %s\n", mz_zip_get_error_string(mz_zip_get_last_error(&zip_archive)));
//         return 1;
//     }

//     // Add the first file to the archive
//     if (!mz_zip_writer_add_mem_from_buf(&zip_archive, file1_name, file1_content.data(), file1_content.length(), NULL, 0, MZ_DEFAULT_COMPRESSION)) {
//         printf("Error adding file1.txt to archive: %s\n", mz_zip_get_error_string(mz_zip_get_last_error(&zip_archive)));
//         mz_zip_writer_end(&zip_archive);
//         return 1;
//     }

//     // Add the second file to the archive
//     if (!mz_zip_writer_add_mem_from_buf(&zip_archive, file2_name, file2_content.data(), file2_content.size(), NULL, 0, MZ_DEFAULT_COMPRESSION)) {
//         printf("Error adding file2.bin to archive: %s\n", mz_zip_get_error_string(mz_zip_get_last_error(&zip_archive)));
//         mz_zip_writer_end(&zip_archive);
//         return 1;
//     }

//     // Finalize the archive (write central directory and close file)
//     if (!mz_zip_writer_finalize_archive(&zip_archive)) {
//         printf("Error finalizing ZIP archive: %s\n", mz_zip_get_error_string(mz_zip_get_last_error(&zip_archive)));
//         mz_zip_writer_end(&zip_archive);
//         return 1;
//     }

//     // End the writer (free resources)
//     if (!mz_zip_writer_end(&zip_archive)) {
//         printf("Error ending ZIP archive writer: %s\n", mz_zip_get_error_string(mz_zip_get_last_error(&zip_archive)));
//         return 1;
//     }

//     printf("Archive '%s' created successfully with '%s' and '%s'.\n", zip_filename, file1_name, file2_name);

//     return 0;
// }

// int main(int argc, char *argv[])
// {

//     enum FolderState
//     {
//         OPENED,
//         CLOSED,
//         COUNT
//     };

//     std::map<std::string, std::array<std::string, FolderState::COUNT>> folder_names_filter
//     {
//         {"default", {"default_folder.png", "default_root_folder_opened.png"}},
//         {".git",    {"folder_type_git.png", "folder_type_git_opened.png"}},
//         {".vscode", {"folder_type_vscode.png", "folder_type_vscode_opened.png"}},
//         {"cmake", {"folder_type_cmake.png", "folder_type_cmake_opened.png"}},
//     };

//     std::map<std::string, std::string> file_names_finter = 
//     {
//         {"cmakelists.txt", "file_type_cmake.png"},
//         {"cmakecache.txt", "file_type_cmake.png"},
//         {"CMakePresets.json", "file_type_cmake.png"}
//     };

//     std::map<std::string, std::string> file_extention_finter =
//     {
//         {"default", "default_file.png"},

//         // C++ headers
//         {".hpp", "file_type_cppheader.png"},
//         {".h", "file_type_cppheader.png"},
//         {".hxx", "file_type_cppheader.png"},
//         {".h++", "file_type_cppheader.png"},
//         {".hp", "file_type_cppheader.png"},
//         {".tcc", "file_type_cppheader.png"},
//         {".inl", "file_type_cppheader.png"},

//         // cmake files
//         {".cmake", "file_type_cmake.png"},

//         // C++ sources
//         {".cc", "file_type_cppheader.png"},
//         {".cpp", "file_type_cppheader.png"},
//         {".cxx", "file_type_cppheader.png"},
//         {".c++", "file_type_cppheader.png"},
//         {".cp", "file_type_cppheader.png"},
//         {".mii", "file_type_cppheader.png"},
//         {".ii", "file_type_cppheader.png"},

//         // images
//         {"png", "file_type_image.png"},
//         {"jpeg", "file_type_image.png"},
//         {"jpg", "file_type_image.png"},
//         {"gif", "file_type_image.png"},
//         {"ico", "file_type_image.png"},
//         {"tif", "file_type_image.png"},
//         {"tiff", "file_type_image.png"},
//         {"ami", "file_type_image.png"},
//         {"apx", "file_type_image.png"},
//         {"avif", "file_type_image.png"},
//         {"bmp", "file_type_image.png"},
//         {"bpg", "file_type_image.png"},
//         {"brk", "file_type_image.png"},
//         {"cur", "file_type_image.png"},
//         {"dds", "file_type_image.png"},
//         {"exr", "file_type_image.png"},
//         {"fpx", "file_type_image.png"},
//         {"gbr", "file_type_image.png"},
//         {"img", "file_type_image.png"},
//         {"jbig2", "file_type_image.png"},
//         {"jb2", "file_type_image.png"},
//         {"jng", "file_type_image.png"},
//         {"jxr", "file_type_image.png"},
//         {"pgf", "file_type_image.png"},
//         {"pic", "file_type_image.png"},
//         {"raw", "file_type_image.png"},
//         {"webp", "file_type_image.png"},
//         {"eps", "file_type_image.png"},
//         {"afphoto", "file_type_image.png"},
//         {"ase", "file_type_image.png"},
//         {"aseprite", "file_type_image.png"},
//         {"clip", "file_type_image.png"},
//         {"cpt", "file_type_image.png"},
//         {"heif", "file_type_image.png"},
//         {"heic", "file_type_image.png"},
//         {"kra", "file_type_image.png"},
//         {"mdp", "file_type_image.png"},
//         {"ora", "file_type_image.png"},
//         {"pdn", "file_type_image.png"},
//         {"reb", "file_type_image.png"},
//         {"sai", "file_type_image.png"},
//         {"tga", "file_type_image.png"},
//         {"xcf", "file_type_image.png"},
//         {"jfif", "file_type_image.png"},
//         {"ppm", "file_type_image.png"},
//         {"pbm", "file_type_image.png"},
//         {"pgm", "file_type_image.png"},
//         {"pnm", "file_type_image.png"},
//         {"icns", "file_type_image.png"},
//         {"3fr", "file_type_image.png"},
//         {"ari", "file_type_image.png"},
//         {"arw", "file_type_image.png"},
//         {"bay", "file_type_image.png"},
//         {"braw", "file_type_image.png"},
//         {"crw", "file_type_image.png"},
//         {"cr2", "file_type_image.png"},
//         {"cr3", "file_type_image.png"},
//         {"cap", "file_type_image.png"},
//         {"data", "file_type_image.png"},
//         {"dcs", "file_type_image.png"},
//         {"dcr", "file_type_image.png"},
//         {"dng", "file_type_image.png"},
//         {"drf", "file_type_image.png"},
//         {"eip", "file_type_image.png"},
//         {"erf", "file_type_image.png"},
//         {"fff", "file_type_image.png"},
//         {"gpr", "file_type_image.png"},
//         {"iiq", "file_type_image.png"},
//         {"k25", "file_type_image.png"},
//         {"kdc", "file_type_image.png"},
//         {"mdc", "file_type_image.png"},
//         {"mef", "file_type_image.png"},
//         {"mos", "file_type_image.png"},
//         {"mrw", "file_type_image.png"},
//         {"nef", "file_type_image.png"},
//         {"nrw", "file_type_image.png"},
//         {"obm", "file_type_image.png"},
//         {"orf", "file_type_image.png"},
//         {"pef", "file_type_image.png"},
//         {"ptx", "file_type_image.png"},
//         {"pxn", "file_type_image.png"},
//         {"r3d", "file_type_image.png"},
//         {"raf", "file_type_image.png"},
//         {"rwl", "file_type_image.png"},
//         {"rw2", "file_type_image.png"},
//         {"rwz", "file_type_image.png"},
//         {"sr2", "file_type_image.png"},
//         {"srf", "file_type_image.png"},
//         {"srw", "file_type_image.png"},
//         {"x3f", "file_type_image.png"},
//         {"ktx", "file_type_image.png"},
//         {"ktx2", "file_type_image.png"},
//     };    

//     return 0;
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

// // TTF BACKEND API
// // Created By: Justin Meiners (2013)
// #include <cmath>
// #include <stdio.h>
// #include <stdlib.h>
// #include <cstring>
// #include <string>
// #include <vector>

// #include <iostream>

// #include "stb_image_write.h"
// #include "stb_truetype.h"

// #include <FrenchieCoreTime.hpp>

// void stb_free_font_info(stbtt_fontinfo* _Info)
// {
//     if(_Info == nullptr)
//         return;

//     if(_Info->data != nullptr)
//         free(_Info->data);

//     if(_Info->userdata != nullptr)
//         free(_Info->userdata);
// }

// unsigned char* stb_load_font_to_buffer(const char* _FilePathUTF8, int* _Size)
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

//     *_Size = size;

//     return fontBuffer;
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

// int stb_get_text_line_width(stbtt_fontinfo* _FontInfo, const char* _Utf8Text, float _FontScale) 
// {
//     int x = 0;
//     int previousCodepoint = 0;

//     for (int codePointIndex = 0; codePointIndex < strlen(_Utf8Text); ++codePointIndex)
//     {
//         int codepoint = _Utf8Text[codePointIndex];

//         // compute glyph horizontal metrics
//         int glyphAdvanceWidth    = 0; // The distance to advance the cursor after rendering the glyph.
// 	    int glyphLeftSideBearing = 0; // The horizontal offset from the origin to the left edge of the glyph's bounding box
        
//         int glyph_kern = // The adjustment of the horizontal space between individual glyph pairs to ensure they look balanced
//             previousCodepoint != 0 ? stbtt_GetCodepointKernAdvance(_FontInfo, previousCodepoint, codepoint) : 0;

//         stbtt_GetCodepointHMetrics(
//             _FontInfo,
//             codepoint,
//             &glyphAdvanceWidth,
//             &glyphLeftSideBearing
//         );
        
//         // add kerning
//         x += (int)roundf(glyphAdvanceWidth * _FontScale) + (int)roundf(glyph_kern * _FontScale);

//         previousCodepoint = codepoint;
//     }

//     return x;
// }

// void render_colored_text_example()
// {
//     // load .ttf
//     auto fontInfo = stb_open_ttf_file(
//         "C:/SDK/Qt_Projects/OpenGL/shared/appData/fonts/Alice-Regular.ttf");

//     // scale font to fit a given font size in pixels
//     float fontSizeInPixels   = 64.f;
//     float fontPixelSizeScale = stbtt_ScaleForPixelHeight(&fontInfo, fontSizeInPixels);

//     // retrieve absolute font vertical metrics
//     int fontAscent  = 0; // distance from the glyph baseline to it's highest point 
//     int fontDescent = 0; // distance fron the glyph baseline to it's lowest point
//     int fontLineGap = 0; // recommended gap between text lines
//     stbtt_GetFontVMetrics(&fontInfo, &fontAscent, &fontDescent, &fontLineGap);
    
//     // compute scaled font vertical metrics
//     fontAscent  = (int)roundf((float)fontAscent  * fontPixelSizeScale);
//     fontDescent = (int)roundf((float)fontDescent * fontPixelSizeScale);
//     fontLineGap = (int)roundf((float)fontLineGap * fontPixelSizeScale);

//     // try to render text info an image
//     std::string text = "the quick brown fox the quick";

//     // compute text size
//     int textWidth    = stb_get_text_line_width(&fontInfo, text.c_str(), fontPixelSizeScale);
//     int textHeight   = fontAscent - fontDescent;
//     int textChannels = 4;

//     // generate bitmap for the text
//     unsigned char* bitmap = (unsigned char*)calloc(textWidth * textHeight, sizeof(unsigned char));
    
//     int x = 0;
//     int y = 0;

//     int previous_codepoint = 0; // For kerning

//     for (int i = 0; i < strlen(text.c_str()); ++i)
//     {
//         int codepoint = text.c_str()[i];

//         // compute glyph horizontal metrics
//         int glyphAdvanceWidth    = 0; // The distance to advance the cursor after rendering the glyph.
// 	    int glyphLeftSideBearing = 0; // The horizontal offset from the origin to the left edge of the glyph's bounding box

//         int glyph_kern = // The adjustment of the horizontal space between individual glyph pairs to ensure they look balanced
//             previous_codepoint != 0 ? stbtt_GetCodepointKernAdvance(&fontInfo, previous_codepoint, codepoint) : 0;

//         stbtt_GetCodepointHMetrics(
//             &fontInfo,
//             codepoint,
//             &glyphAdvanceWidth,
//             &glyphLeftSideBearing
//         );

//         // compute glyph bounding box
//         int glyphXmin = 0;
//         int glyphYmin = 0;
//         int glyphXmax = 0;
//         int glyphYmax = 0;

//         stbtt_GetCodepointBitmapBox(
//             &fontInfo,
//             codepoint,
//             fontPixelSizeScale,
//             fontPixelSizeScale,
//             &glyphXmin,
//             &glyphYmin,
//             &glyphXmax,
//             &glyphYmax
//         );

//         int glyphWidth  = (glyphXmax - glyphXmin);
//         int glyphHeight = (glyphYmax - glyphYmin);

//         // render character (stride and offset is important here)
//         int glyphByteOffset = x + (int)roundf(glyphLeftSideBearing * fontPixelSizeScale) + (fontAscent + glyphYmin) * textWidth;

//         stbtt_MakeCodepointBitmap(
//             &fontInfo,
//             bitmap + glyphByteOffset,
//             glyphWidth,
//             glyphHeight,
//             textWidth,
//             fontPixelSizeScale,
//             fontPixelSizeScale,
//             codepoint
//         );
        
//         // add kerning
//         x += (int)roundf(glyphAdvanceWidth * fontPixelSizeScale) + (int)roundf(glyph_kern * fontPixelSizeScale);

//         previous_codepoint = codepoint;
//     }

//     // colorify text
//     int red   = 0;
//     int green = 1;
//     int blue  = 2;
//     int alpha = 3;

//     unsigned char* image =
//         (unsigned char*)calloc(textWidth * textHeight * textChannels, sizeof(unsigned char));

//     for (int y = 0; y < textHeight; y++)
//     {
//         for (int x = 0; x < textWidth; x++)
//         {
//             image[textChannels * (y * textWidth + x) + red  ] = 128;
//             image[textChannels * (y * textWidth + x) + green] = 0;
//             image[textChannels * (y * textWidth + x) + blue ] = 0;
//             image[textChannels * (y * textWidth + x) + alpha] = bitmap[y * textWidth + x];
//         }
//     }

//     // save out a 1 channel image
//     stbi_write_png("out.png", textWidth, textHeight, textChannels, image, textWidth * textChannels);
    
//     free(bitmap);
//     free(image);
// }

// // Core
// #include <FrenchieCoreStringUnicode.hpp>
// #include <FrenchieCoreMemoryCache.hpp>

// // STL
// #include <filesystem>

// int main(int argc, const char * argv[])
// {
//     // std::filesystem::path path("C:/SDK/Qt_Projects/OpenGL/shared/appData/fonts/Alice-Regular.ttf");

//     // auto font  = Frenchie::Core::Memory::Cache<Fonts::Font>::request(path.u32string(), path, 128);

//     // auto then = Frenchie::Core::Time::tic();

//     // auto atlas = Frenchie::Core::Memory::Cache<Fonts::FontAtlas>::request(
//     //     U"Alice-Regular-Atlas", font.get());

//     // std::cout << Frenchie::Core::Time::elapsed<std::chrono::milliseconds>(then, Frenchie::Core::Time::tic()) << "ms \n";

//     // stbi_write_png("atlas.png",
//     //     atlas->m_TextureWidth,
//     //     atlas->m_TextureHeight,
//     //     1,
//     //     atlas->m_Bitmap.get(),
//     //     atlas->m_TextureWidth);

//     return 0;
// }