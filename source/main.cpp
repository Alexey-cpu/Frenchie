// //---------------------------------------------------------------------------------------------------
// // Editor
// //---------------------------------------------------------------------------------------------------
#include <FrenchieApplicationEditorLauncher.hpp>

namespace LowLevelRendererAPI
{
    // Utils
    template<typename Head>
    inline Head __max__(const Head& _A, const Head& _B)
    {
        return _A > _B ? _A : _B;
    }

    template<typename Head, typename ... Args>
    Head __max__(const Head& _A, const Head& _B, Args... _Args)
    {
        return __max__(__max__(_A, _B), _Args...);
    }

    template<typename Head>
    inline  Head __min__(const Head& _A, const Head&)
    {
        return _A < _B ? _A : _B;
    }

    template<typename Head, typename ... Args>
    Head __min__(const Head& _A, const Head& _B, Args... _Args)
    {
        return __min__(__min__( _A, _B ), _Args...);
    }

    template<typename __type>
    inline  __type __abs__(const __type& _A)
    {
        return (_A < 0 ) ? -_A : _A;
    }

    // Allocator
    struct RendererAllocationInfo final
    {
        uintptr_t Chunk  = 0;
        uintptr_t Amount = 0;
    };

    struct RendererMemoryChunk final
    {
        // info
        mutable uintptr_t            ElementSize   = 0;
        mutable uintptr_t            ElementsCount = 0;
        mutable uintptr_t            Free          = 0;
        mutable uintptr_t            Head          = 0;
        mutable uintptr_t            Size          = 0;
        mutable char*                Memory        = nullptr;
        mutable RendererMemoryChunk* Next          = nullptr;
        mutable RendererMemoryChunk* Prev          = nullptr;

        RendererMemoryChunk(uintptr_t _ChunkElementSize, uintptr_t _ChunkSize)
        {
            ElementSize   = __max__<uintptr_t>(_ChunkElementSize, 1);
            ElementsCount = __max__<uintptr_t>(_ChunkSize, 1);
            Free          = (sizeof(RendererAllocationInfo) + ElementSize) * ElementsCount;
            Head          = 0;
            Size          = Free;
            Memory        = reinterpret_cast<char*>(malloc(Size));
        }

        ~RendererMemoryChunk()
        {
            ElementSize   = 0;
            ElementsCount = 0;
            Free          = 0;
            Head          = 0;
            Size          = 0;
            free(Memory);
            Memory = nullptr;
            Next   = nullptr;
            Prev   = nullptr;
        }

        void* request(uintptr_t _Size)
        {
            uintptr_t amount = sizeof(RendererAllocationInfo) + __max__<uintptr_t>(_Size, 1) * ElementSize;

            if(Head + amount > Size) 
                return nullptr; // out-of memory
            
            char* buffer         = Memory + Head + sizeof(RendererAllocationInfo);
            RendererAllocationInfo* info = reinterpret_cast<RendererAllocationInfo*>(buffer - sizeof(RendererAllocationInfo));
            info->Chunk          = reinterpret_cast<uintptr_t>(this);
            info->Amount         = amount;
            Head                += amount;
            Free                -= amount;

            return buffer;
        }

        static RendererAllocationInfo* release(void* _Pointer)
        {
            if(_Pointer == nullptr)
                return nullptr;

            RendererAllocationInfo* info  = reinterpret_cast<RendererAllocationInfo*>(reinterpret_cast<char*>(_Pointer) - sizeof(RendererAllocationInfo));
            RendererMemoryChunk*    chunk = reinterpret_cast<RendererMemoryChunk*>(info->Chunk);
            chunk->Free += info->Amount;

            if(chunk->Free >= chunk->Size)
            {
                chunk->Head = 0;
                chunk->Free = chunk->Size;
            }

            return info;
        }

        bool is_free() const
        {
            return Free >= Size;
        }
    };

    template<typename Type>
    class RendererMemoryChunkAllocator final
    {
    public:
        RendererMemoryChunkAllocator(uintptr_t _ChunkSize) : 
            m_ChunkSize(__max__<uintptr_t>(_ChunkSize, 16)), 
            m_Head(new RendererMemoryChunk(sizeof(Type), m_ChunkSize)), 
            m_Tail(m_Head){}

        ~RendererMemoryChunkAllocator()
        {
            release();
        }

        Type* allocate(uintptr_t _Size) const
        {
            // create chunks list
            if(m_Head == nullptr && m_Tail == nullptr)
            {
                m_Head = new RendererMemoryChunk(sizeof(Type), m_ChunkSize);
                m_Tail= m_Head;
            }

            // allocate buffer
            auto buffer = m_Head->request(_Size);

            if(buffer != nullptr) 
                return reinterpret_cast<Type*>(buffer);

            m_ChunkSize = __max__<uintptr_t>(m_ChunkSize, _Size);

            RendererMemoryChunk* chunk  = new RendererMemoryChunk(sizeof(Type), m_ChunkSize);
            chunk->Next = nullptr;
            chunk->Prev = m_Head;

            m_Head->Next = chunk;
            m_Head = chunk;

            return reinterpret_cast<Type*>(m_Head->request(_Size));
        }

        void deallocate(Type* _Pointer) const
        {
            if(m_Head == nullptr && m_Tail == nullptr) 
                return;

            // clear pointer and retrieve allocation info
            auto info  = RendererMemoryChunk::release(_Pointer);
            auto chunk = info != nullptr ? reinterpret_cast<RendererMemoryChunk*>(info->Chunk) : nullptr;

            // check that this is the first chunk
            if(chunk == nullptr || 
                (chunk->Prev == nullptr && chunk->Next == nullptr) || !chunk->is_free()) 
            {
                return;
            }

            // update chunk links
            if(chunk->Prev != nullptr)
            {
                chunk->Prev->Next = chunk->Next;
            }
            else
            {
                // update tail
                m_Tail = chunk->Next;
            }
            
            if(chunk->Next != nullptr)
            {
                chunk->Next->Prev = chunk->Prev;
            }
            else 
            {
                // update head
                m_Head = chunk->Prev;
            }

            // remove chunk
            delete chunk;
        }

        void release()
        {
            auto next = m_Tail;

            while (next)
            {
                auto current = next;
                next = next->Next;
                delete current;
            }

            // clean up tail
            m_Tail = nullptr;
            m_Head = nullptr;
        }

        uintptr_t get_total_memory_size() const
        {
            uintptr_t freeMemory = 0;

            auto next = m_Tail;

            while (next)
            {
                freeMemory  += next->Size;
                next = next->Next;
            }

            return freeMemory;
        }

        uintptr_t get_free_memory_amount() const
        {
            uintptr_t freeMemory = 0;

            auto next = m_Tail;

            while (next)
            {
                freeMemory  += next->Free;
                next = next->Next;
            }

            return freeMemory;
        }

        uintptr_t get_busy_memory_amount() const
        {
            uintptr_t freeMemory = 0;

            auto next = m_Tail;

            while (next)
            {
                freeMemory += next->Head;
                next = next->Next;
            }

            return freeMemory;
        }

    private:
        
        mutable  uintptr_t            m_ChunkSize{1024};
        mutable  RendererMemoryChunk* m_Head{nullptr};
        mutable  RendererMemoryChunk* m_Tail{nullptr};
    };

    // structs
    struct RendererBuffer
    {
        uintptr_t Ptr{0};
    };
    
    struct RendererShader
    {
        uintptr_t Ptr{0};
    };

    struct RendererTexture
    {
        unsigned int Width   {0};
        unsigned int Height  {0};
        unsigned int Channels{0};
        uintptr_t    Ptr     {0};
    };

    struct RendererState
    {
        RendererMemoryChunkAllocator<RendererBuffer>  VBOs;
        RendererMemoryChunkAllocator<RendererBuffer>  VBAs;
        RendererMemoryChunkAllocator<RendererBuffer>  EBOs;
        RendererMemoryChunkAllocator<RendererShader>  Shaders;
        RendererMemoryChunkAllocator<RendererTexture> Textures;
    };
}

int main(int argc, char *argv[])
{
    return Frenchie::Editor::Launcher::execute();
}


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