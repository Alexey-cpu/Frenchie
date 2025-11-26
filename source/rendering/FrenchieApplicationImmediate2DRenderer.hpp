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
        struct Font
        {
            std::shared_ptr<stbtt_packedchar>   Chars       {nullptr};
            std::shared_ptr<stbtt_aligned_quad> Quads       {nullptr};
            int                                 UnicodeMin  {0};
            int                                 UnicodeMax  {0};
            RenderingQueueTexture               AtlasTexture{RenderingQueueTexture()};
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

            Font m_Font;

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
                stbtt_fontinfo* info = (stbtt_fontinfo*)malloc(sizeof(stbtt_fontinfo));

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

            Font load_font(const int& m_SizeInPixels)
            {
                // load font file
                std::shared_ptr<stbtt_fontinfo> m_Info =
                    std::shared_ptr<stbtt_fontinfo>(
                        stb_open_ttf_file("C:/SDK/Qt_Projects/OpenGL/shared/appData/fonts/Alice-Regular.ttf"),
                        [](stbtt_fontinfo* _Info)
                        {
                            if(_Info != nullptr)
                                free(_Info);
                        }
                    );
                
                // retrieve available font file characters
                int m_UnicodeMin = INT_MAX;
                int m_UnicodeMax = INT_MIN;

                for(unsigned int codepoint = 0; codepoint <= 0xFFFF; codepoint++)
                {
                    if(!stbtt_FindGlyphIndex(m_Info.get(), codepoint))
                        continue;

                    m_UnicodeMin = std::min<int>(m_UnicodeMin, codepoint);
                    m_UnicodeMax = std::max<int>(m_UnicodeMax, codepoint);
                }

                // compute font geometry metrics
                float m_SizeInPixelScale = 1.f; // scale font to fit a given font size in pixels
                int   m_Ascent           = 0;   // distance from the glyph baseline to it's highest point 
                int   m_Descent          = 0;   // distance fron the glyph baseline to it's lowest point
                int   m_LineGap          = 0;   // recommended gap between text lines
        
                m_SizeInPixelScale = stbtt_ScaleForPixelHeight(m_Info.get(), (float)m_SizeInPixels);

                stbtt_GetFontVMetrics(m_Info.get(), &m_Ascent, &m_Descent, &m_LineGap);
                m_Ascent  = (int)roundf((float)m_Ascent  * m_SizeInPixelScale);
                m_Descent = (int)roundf((float)m_Descent * m_SizeInPixelScale);
                m_LineGap = (int)roundf((float)m_LineGap * m_SizeInPixelScale);

                // pack font atlas
                std::shared_ptr<stbtt_packedchar> m_Chars = 
                    std::shared_ptr<stbtt_packedchar>(
                        (stbtt_packedchar*)malloc(sizeof(stbtt_packedchar) * (m_UnicodeMax + 1)),
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
                    
                    for (int codepoint = m_UnicodeMin; codepoint <= m_UnicodeMax;)
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
                                m_Info.get(),
                                codepoint,
                                m_SizeInPixelScale,
                                m_SizeInPixelScale,
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
                std::shared_ptr<unsigned char> m_Bitmap        = nullptr;
                int                            m_TextureWidth  = atlasMaxWidth;
                int                            m_TextureHeight = atlasHeight;

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
                        m_Info->data,
                        0,
                        (float)m_SizeInPixels,
                        m_UnicodeMin,
                        m_UnicodeMax - m_UnicodeMin,
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

                // write packed png...
                stbi_write_png(
                    "C:/SDK/Qt_Projects/OpenGL/logs/atlas.png",
                    m_TextureWidth,
                    m_TextureHeight,
                    1,
                    m_Bitmap.get(),
                    m_TextureWidth);

                // generate atlas glyphs quads
                std::shared_ptr<stbtt_aligned_quad> m_Quads = 
                    std::shared_ptr<stbtt_aligned_quad>(
                        (stbtt_aligned_quad*)malloc(sizeof(stbtt_aligned_quad) * m_UnicodeMax),
                        [](stbtt_aligned_quad* _Range)
                        {
                            if(_Range != nullptr)
                                free(_Range);
                        }
                    );

                for (int i = 0; i < m_UnicodeMax; i++)
                {
                    float unusedX, unusedY;

                    stbtt_GetPackedQuad(
                        m_Chars.get(),      // Array of stbtt_packedchar
                        m_TextureWidth,     // Width of the font atlas texture
                        m_TextureHeight,    // Height of the font atlas texture
                        i,                  // Index of the glyph
                        &unusedX, &unusedY, // current position of the glyph in screen pixel coordinates, (not required as we have a different corrdinate system)
                        &m_Quads.get()[i],  // stbtt_alligned_quad struct. (this struct mainly consists of the texture coordinates)
                        0                   // Allign X and Y position to a integer (doesn't matter because we are not using 'unusedX' and 'unusedY')
                    );
                }

                // generate output font
                Font font;
                font.Chars        = m_Chars;
                font.Quads        = m_Quads;
                font.UnicodeMin   = m_UnicodeMin;
                font.UnicodeMax   = m_UnicodeMax;
                
                if(m_Bitmap != nullptr)
                {
                    font.AtlasTexture = Frenchie::Application::application_rendering_queue()->construct_texture(
                        m_Bitmap.get(),
                        m_TextureWidth,
                        m_TextureHeight,
                        RenderingQueueTextureFormat_::RenderingQueueTextureFormat_ALPHA
                    );
                }

                return font;
            }

            void push_text(const Font& _Font, const gs_mat4f& _Transform)
            {
                // let's draw a single character
                char ch = 'A';

                stbtt_packedchar*   packedChar  = &_Font.Chars.get()[ch - _Font.UnicodeMin];
                stbtt_aligned_quad* alignedQuad = &_Font.Quads.get()[ch - _Font.UnicodeMin];

                build_rectangle_filled_mesh(
                    gs_vec2f(0.f, 0.f),
                    gs_vec2f(100.f, -100.f),
                    gs_vec2f(alignedQuad->s0, alignedQuad->t0),
                    gs_vec2f(alignedQuad->s1, alignedQuad->t1),
                    0.f,
                    m_Vertexes,
                    m_Indexes
                );

                push_rendering_command(_Font.AtlasTexture, gs_vec4f(255.f, 255.f, 255.f, 255.f), _Transform);

// static void build_rectangle_filled_mesh(
//     const gs_vec2f&                    _Min,
//     const gs_vec2f&                    _Max,
//     const gs_vec2f&                    _MinUV,
//     const gs_vec2f&                    _MaxUV,
//     const float&                       _Depth,
//     std::vector<RenderingQueueVertex>& _Vertexes,
//     std::vector<int>&                  _Indexes)

                // printf("Atlas width = %d height = %d \n", _Font.AtlasTexture.Width, _Font.AtlasTexture.Height);
                // printf("stbtt_packedchar  : x0 = %d y0 = %d x1 = %d y1 = %d \n", packedChar->x0, packedChar->y0, packedChar->x1, packedChar->x0);
                printf("stbtt_aligned_quad: s0 = %f t0 = %f s1 = %f t1 = %f \n", alignedQuad->s0, alignedQuad->t0, alignedQuad->s1, alignedQuad->t1);
                // printf("stbtt_aligned_quad: x0 = %f y0 = %f x1 = %f y1 = %f \n", alignedQuad->x0, alignedQuad->y0, alignedQuad->x1, alignedQuad->x0);
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