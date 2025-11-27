#pragma once

#include <FrenchieApplication.hpp>

#include <chrono>
#include <iostream>

// STB
#include "stb_image_write.h"
#include "stb_truetype.h"

namespace Frenchie
{
    namespace Application
    {
        struct RenderingQueueGlyph
        {
            RenderingQueueGlyph(
                const gs_rectf& _Box     = gs_rectf(gs_vec2f(0.f), gs_vec2f(0.f)),
                const gs_vec2f& _MinUV   = gs_vec2f(0.f),
                const gs_vec2f& _MaxUV   = gs_vec2f(0.f),
                const gs_vec2f& _Bearing = gs_vec2f(0.f),
                const float&    _Advance = 0.f) :
                    Box(_Box),
                    MinUV(_MinUV),
                    MaxUV(_MaxUV),
                    Bearing(_Bearing),
                    Advance(_Advance){}

            gs_rectf Box    {gs_rectf(gs_vec2f(0.f), gs_vec2f(0.f))};
            gs_vec2f MinUV  {gs_vec2f(0.f)};
            gs_vec2f MaxUV  {gs_vec2f(0.f)};
            gs_vec2f Bearing{gs_vec2f(0.f)};
            float    Advance{0.f};
        };

        struct RenderingQueueFont
        {
            RenderingQueueFont(
                const int&                                  _SizeInPixels      = 0,
                const float&                                _SizeInPixelsScale = 0,
                const int&                                  _UnicodeMin        = 0,
                const int&                                  _UnicodeMax        = 0,
                const std::shared_ptr<RenderingQueueGlyph>& _Glyphs            = nullptr,
                const RenderingQueueTexture&                _AtlasTexture      = RenderingQueueTexture()) :
                    SizeInPixels(_SizeInPixels),
                    SizeInPixelsScale(_SizeInPixelsScale),
                    UnicodeMin(_UnicodeMin),
                    UnicodeMax(_UnicodeMax),
                    Glyphs(_Glyphs),
                    AtlasTexture(_AtlasTexture){}

            int                    SizeInPixels     {0};
            float                  SizeInPixelsScale{0.f};
            int                    UnicodeMin       {0};
            int                    UnicodeMax       {0};
            std::shared_ptr<RenderingQueueGlyph> Glyphs           {nullptr};
            RenderingQueueTexture  AtlasTexture     {RenderingQueueTexture()};

            bool contains_glyph(const unsigned int& _UTF8Codepoint) const
            {
                return _UTF8Codepoint >= UnicodeMin &&
                       _UTF8Codepoint <= UnicodeMax;
            }

            RenderingQueueGlyph retrieve_glyph(const unsigned int& _UTF8Codepoint) const
            {
                return Glyphs.get()[_UTF8Codepoint - UnicodeMin];
            }
        };

        class Immediate2DRenderer : public Layer
        {
        public:
            Immediate2DRenderer();
            virtual ~Immediate2DRenderer();

            virtual bool awake() override;
            virtual void frame_start() override;
            virtual void frame_render() override;
            virtual void finish() override;

            // command API
            void push_rendering_command(const RenderingQueueTexture& _Texture, const gs_vec4f& _Color, const gs_mat4f& _Transform);

            // rendering API
            void push_triangle_filled(
                const gs_vec2f& _P1,
                const gs_vec2f& _P2,
                const gs_vec2f& _P3,
                const float&    _Depth,
                const gs_vec4f& _Color,
                const gs_mat4f& _Transform,
                const RenderingQueueTexture& _Texture  = RenderingQueueTexture());

            void push_rectangle_filled(
                const gs_vec2f&              _Min,
                const gs_vec2f&              _Max,
                const float&                 _Depth,
                const gs_vec4f&              _Color,
                const gs_mat4f&              _Transform,
                const RenderingQueueTexture& _Texture  = RenderingQueueTexture());

            void push_line(
                const gs_vec2f& _P1,
                const gs_vec2f& _P2,
                const float&    _Depth,
                const float&    _LineWidth,
                const gs_vec4f& _Color,
                const gs_mat4f& _Transform);

            void push_arc(
                const gs_vec2f& _Center,
                const float&    _MinorRadius,
                const float&    _MajorRadius,
                const float&    _SourceAngle,
                const float&    _TargetAngle,
                const float&    _Depth,
                const float&    _LineWidth,
                const gs_vec4f& _Color,
                const gs_mat4f& _Transform);

            void push_triangle(
                const gs_vec2f& _P1,
                const gs_vec2f& _P2,
                const gs_vec2f& _P3,
                const float&    _Depth,
                const float&    _LineWidth,
                const gs_vec4f& _Color,
                const gs_mat4f& _Transform);

            void push_rectangle(
                const gs_vec2f& _Min,
                const gs_vec2f& _Max,
                const float&    _Depth,
                const float&    _LineWidth,
                const gs_vec4f& _Color,
                const gs_mat4f& _Transform);

            void push_rectangle_rounded(
                const gs_vec2f& _Min,
                const gs_vec2f& _Max,
                const float&    _Radius,
                const float&    _Depth,
                const float&    _LineWidth,
                const gs_vec4f& _Color,
                const gs_mat4f& _Transform);

            // static API
            static void build_triangle_filled_mesh(
                const gs_vec2f&                    _P1,
                const gs_vec2f&                    _P2,
                const gs_vec2f&                    _P3,
                const float&                       _Depth,
                const RenderingQueueTexture&       _Texture,
                std::vector<RenderingQueueVertex>& _Vertexes,
                std::vector<int>&                  _Indexes);

            static void build_rectangle_filled_mesh(
                const gs_vec2f&                    _Min,
                const gs_vec2f&                    _Max,
                const float&                       _Depth,
                const RenderingQueueTexture&       _Texture,
                std::vector<RenderingQueueVertex>& _Vertexes,
                std::vector<int>&                  _Indexes);

            static void build_rectangle_filled_mesh(
                const gs_vec2f&                    _Min,
                const gs_vec2f&                    _Max,
                const gs_vec2f&                    _MinUV,
                const gs_vec2f&                    _MaxUV,
                const float&                       _Depth,
                std::vector<RenderingQueueVertex>& _Vertexes,
                std::vector<int>&                  _Indexes);

            static void build_line_mesh(
                const gs_vec2f&                    _P1,
                const gs_vec2f&                    _P2,
                const float&                       _Depth,
                const float&                       _LineWidth,
                const RenderingQueueTexture&       _Texture,
                std::vector<RenderingQueueVertex>& _Vertexes,
                std::vector<int>&                  _Indexes);

            static void build_arc_mesh(
                const gs_vec2f&                    _Center,
                const float&                       _MinorRadius,
                const float&                       _MajorRadius,
                const float&                       _SourceAngle,
                const float&                       _TargetAngle,
                const float&                       _Depth,
                const float&                       _LineWidth,
                const RenderingQueueTexture&       _Texture,
                std::vector<RenderingQueueVertex>& _Vertexes,
                std::vector<int>&                  _Indexes);

            // this is a plipeline
            std::vector<RenderingQueueVertex>  m_Vertexes        {std::vector<RenderingQueueVertex>()};
            std::vector<int>                   m_Indexes         {std::vector<int>()};
            gs_rectf                           m_Viewport        {-gs_huge<float>(), -gs_huge<float>(), +gs_huge<float>(), +gs_huge<float>()}; 
            RenderingQueueShader               m_DefaultShader   {RenderingQueueShader()};
            RenderingQueueTexture              m_DefaultTexture  {RenderingQueueTexture()};

            RenderingQueueFont m_Font;

            void stb_free_font_info(stbtt_fontinfo* _Info)
            {
                if(_Info == nullptr)
                    return;

                if(_Info->data != nullptr)
                    free(_Info->data);

                if(_Info->userdata != nullptr)
                    free(_Info->userdata);
            }

            stbtt_fontinfo* stb_open_ttf_file(const char* _FilePathUTF8)
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
                stbtt_fontinfo* info = new stbtt_fontinfo();

                if (!stbtt_InitFont(info, fontBuffer, 0))
                {
                    info->userdata = nullptr;
                    info->data     = nullptr;
                    free(fontBuffer);
                    free(info);
                    return nullptr;
                }

                return info;
            }

            RenderingQueueFont load_font(const int& _SizeInPixels)
            {
                // load font file
                std::shared_ptr<stbtt_fontinfo> fontInfo =
                    std::shared_ptr<stbtt_fontinfo>(stb_open_ttf_file("C:/SDK/Qt_Projects/OpenGL/shared/appData/fonts/Alice-Regular.ttf"),
                    [](stbtt_fontinfo* _Data)
                    {
                        if(_Data != nullptr)
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
                int atlasWidth     = 32;
                int atlasHeight    = 32;
                int atlasMaxHeight = 16;
                int atlasMaxWidth  = 32;
                int atlasMaxSize   = 4096;
                
                while (true)
                {
                    atlasWidth  = 0;
                    atlasHeight = 0;
                    
                    for (int codepoint = unicodeMin; codepoint <= unicodeMax;)
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
                                fontInfo.get(),
                                codepoint,
                                sizeInPixelsScale,
                                sizeInPixelsScale,
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

                    if(atlasHeight >= atlasMaxSize && atlasMaxWidth >= atlasMaxSize)
                        atlasMaxSize *= 2;

                    atlasMaxWidth *= 2;
                    atlasMaxWidth  = std::min<int>(atlasMaxWidth, atlasMaxSize);

                    if(atlasHeight <= atlasMaxHeight)
                        break;

                    atlasMaxHeight *= 2;
                    atlasMaxHeight  = std::min<int>(atlasMaxHeight, atlasMaxSize);
                }

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
                        packedCharacters.get(),          // Array of stbtt_packedchar
                        atlasWidth,                      // Width of the font atlas texture
                        atlasHeight,                     // Height of the font atlas texture
                        idx,                               // Index of the glyph
                        &unusedX, &unusedY,              // current position of the glyph in screen pixel coordinates, (not required as we have a different corrdinate system)
                        &packedCharactersQuads.get()[idx], // stbtt_alligned_quad struct. (this struct mainly consists of the texture coordinates)
                        0                                // Allign X and Y position to a integer (doesn't matter because we are not using 'unusedX' and 'unusedY')
                    );
                }

                // retrieve atlas glyphs
                std::shared_ptr<RenderingQueueGlyph> glyphs = std::shared_ptr<RenderingQueueGlyph>(
                    new Glyph[glyphsCount],
                    [](Glyph* _Data)
                    {
                        if(_Data != nullptr)
                            delete [] _Data;
                    }
                );

                for (int i = unicodeMin; i <= unicodeMax; i++)
                {
                    int idx = i - unicodeMin;

                    glyphs.get()[idx].Box = gs_rectf(
                        gs_vec2f(packedCharacters.get()[idx].x0, packedCharacters.get()[idx].y0),
                        gs_vec2f(packedCharacters.get()[idx].x1, packedCharacters.get()[idx].y1));
                    
                    glyphs.get()[idx].MinUV   = gs_vec2f(packedCharactersQuads.get()[idx].s0, packedCharactersQuads.get()[idx].t0);
                    glyphs.get()[idx].MaxUV   = gs_vec2f(packedCharactersQuads.get()[idx].s1, packedCharactersQuads.get()[idx].t1);
                    glyphs.get()[idx].Bearing = gs_vec2f(packedCharacters.get()[idx].xoff, packedCharacters.get()[idx].yoff);
                    glyphs.get()[idx].Advance = packedCharacters.get()[idx].xadvance;
                }
                
                // generate font colorified bitmap
                if(atlasBitMap == nullptr)
                    return RenderingQueueFont(_SizeInPixels, sizeInPixelsScale, unicodeMin, unicodeMax);

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
                            colorifiedAtlasBitMap.get()[channels * (y * atlasWidth + x) + c  ] =
                                atlasBitMap.get()[(y * atlasWidth + x)];
                        }
                    }
                }

                // // write packed png...
                // stbi_write_png(
                //     "C:/SDK/Qt_Projects/OpenGL/logs/atlas.png",
                //     atlasWidth,
                //     atlasHeight,
                //     1,
                //     atlasBitMap.get(),
                //     atlasWidth);

                return RenderingQueueFont(
                    _SizeInPixels,
                    sizeInPixelsScale,
                    unicodeMin,
                    unicodeMax,
                    glyphs,
                    Frenchie::Application::application_rendering_queue()->construct_texture(
                        colorifiedAtlasBitMap.get(),
                        atlasWidth,
                        atlasHeight,
                        RenderingQueueTextureFormat_::RenderingQueueTextureFormat_RGBA)
                    );
            }

            // text rendering command
            void push_text(
                const gs_vec2f&    _Position,
                const float&       _Size,
                const RenderingQueueFont&        _Font,
                const std::string& _Text,
                const gs_mat4f&    _Transform)
            {
                float scale     = _Size / (float)_Font.SizeInPixels;
                float positionX = _Position.x;
                float positionY = _Position.y;

                for(int i = 0; i < strlen(_Text.c_str()); ++i)
                {
                    // fallbacks
                    if(!_Font.contains_glyph(_Text.c_str()[i]))
                    {
                        // next line
                        if(_Text.c_str()[i] == '\n')
                        {
                            positionY -= _Size;
                            positionX =  _Position.x;
                        }
                        // carriage return
                        else if(_Text.c_str()[i] == '\r')
                            positionX =  _Position.x;
                        // tab
                        else if(_Text.c_str()[i] == '\t')
                            positionX += _Size;
                        else
                        {
                            // TODO: do someting here...
                            // May be use fallback font ???
                        }

                        continue;
                    }

                    RenderingQueueGlyph glyph                  = _Font.retrieve_glyph(_Text.c_str()[i]);
                    float glyphWidth             = glyph.Box.get_size().x * scale;
                    float glyphHeight            = glyph.Box.get_size().y * scale;
                    float glyphHorizontalBearing = glyph.Bearing.x * scale;
                    float glyphVerticalBearing   = glyph.Bearing.y * scale;
                    float glyphAdvance           = glyph.Advance * scale;

                    build_rectangle_filled_mesh(
                        gs_vec2f(positionX + glyphHorizontalBearing, positionY - glyphVerticalBearing),
                        gs_vec2f(positionX + glyphHorizontalBearing + glyphWidth, positionY - glyphHeight - glyphVerticalBearing),
                        glyph.MinUV,
                        glyph.MaxUV,
                        0.f,
                        m_Vertexes,
                        m_Indexes
                    );

                    // move cursor
                    positionX += glyphAdvance;
                }

                push_rendering_command(_Font.AtlasTexture, gs_vec4f(255.f, 255.f, 255.f, 255.f), _Transform);
            }
        };
    }
}

// template<typename Type> struct Tree;

// template<typename Type>
// struct Node
// {
//     int               Parent{-1};
//     int               Index {-1};
//     Type              Data  {Type()};
//     const Tree<Type>* Tree  {nullptr};
// };

// template<typename Type>
// struct Tree
// {
//     std::vector<Node<Type>> Nodes  {std::vector<Node<Type>>()};
//     std::vector<int>        Indexes{std::vector<int>()};
//     std::vector<int>        Entries{std::vector<int>()};
//     bool                    Dirty  {true};

//     template<typename ... Args>
//     Node<Type> construct_node(const Node<Type>& _Parent, Args ... _Args)
//     {
//         // make dirty
//         Dirty = true;

//         // create node
//         Node<Type> node;
//         node.Parent = _Parent.Index;
//         node.Index  = (int)Nodes.size();
//         node.Data   = Type(_Args ...);
//         node.Tree   = this;
//         Nodes.push_back(node);
//         return node;
//     }

//     void sort()
//     {
//         std::vector<Node<Type>> nodes(Nodes.size());
//         std::vector<int> workspace(Nodes.size()+1);

//         Indexes.resize(Nodes.size() + 1);
//         Entries.resize(Nodes.size());

//         for(int i = 0; i < Entries.size(); i++)
//         {
//             Entries[i] = 0;
//             Indexes[i] = 0;
//         }

//         // count items
//         for (int i = 0; i < Nodes.size(); i++)
//         {
//             if(Nodes[i].Parent < 0) continue;
//             ++Entries[Nodes[i].Parent];
//         }

//         // cumulative sum
//         int sum = 0;
//         for (int i = 0; i < Nodes.size(); i++)
//         {
//             Indexes  [i] = sum;
//             workspace[i] = sum;
//             sum += Entries[i];
//         }
//         Indexes[Nodes.size()] = sum;

//         for(int i = 0; i < Nodes.size(); i++ )
//         {
//             if(Nodes[i].Parent < 0) continue;
//             nodes[workspace[Nodes[i].Parent]++] = Nodes[i];
//         }

//         Nodes = nodes;
//     }

//     void clear()
//     {
//         Nodes.clear();
//     }
// };