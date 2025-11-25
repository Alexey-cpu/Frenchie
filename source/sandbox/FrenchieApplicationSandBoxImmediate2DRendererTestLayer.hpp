#pragma once

#include <FrenchieApplicationImmediate2DRenderer.hpp>

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
                    (stbtt_packedchar*)malloc(sizeof(stbtt_packedchar) * m_UnicodeMax),
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
                
                for (int codepoint = m_UnicodeMin; codepoint < m_UnicodeMax;)
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
            font.AtlasTexture = RenderingQueueTexture();

            return font;
        }

        class Immedidate2DRendererTestLayer : public Layer
        {
        public:
            Immedidate2DRendererTestLayer(){}
            virtual ~Immedidate2DRendererTestLayer(){}

            virtual bool awake() override
            {
                if(m_Renderer == nullptr)
                    m_Renderer = Frenchie::Application::application()->push_layer<Immediate2DRenderer>();

                load_font(128);

                return m_Renderer != nullptr && m_Renderer->awake();
            }

            virtual void frame_start() override
            {
                // add graphical primitives
                if(Frenchie::Application::application()
                    ->is_mouse_button_down(Frenchie::Application::ApplicationMouseButton::ApplicationMouseButton_Left))
                {
                    auto x = Frenchie::Application::application()->get_cursor_position().x;
                    auto y = Frenchie::Application::application()->get_cursor_position().y;

                    Transform = gs_matrix_translate(
                        gs_mat4f(1),
                        gs_vec3f(
                            x, 
                            -y, 
                            -100.f
                        )
                    );
                }
            }

            virtual void frame_update() override
            {
                if(m_Renderer == nullptr)
                    return;

                for(int i = 0; i < 1e3; ++i)
                {
                    for(int j = 0; j < 12; ++j)
                    {
                        float lineWidth = 8.f;
                        float R = 128.f;

                        // m_Renderer->push_triangle_filled(
                        //     gs_vec2f(i * R * 2, j * R * 2),
                        //     gs_vec2f(i * R * 2 + R, j * R * 2 + R),
                        //     gs_vec2f(i * R * 2 + R, j * R * 2 - R),
                        //     +0.f,
                        //     gs_vec4f(255.f, 0.f, 0.f, 255.f),
                        //     Transform
                        // );

                        // m_Renderer->push_triangle(
                        //     gs_vec2f(i * R * 2, j * R * 2),
                        //     gs_vec2f(i * R * 2 + R, j * R * 2 + R),
                        //     gs_vec2f(i * R * 2 + R, j * R * 2 - R),
                        //     +0.f,
                        //     lineWidth,
                        //     gs_vec4f(255.f, 0.f, 0.f, 255.f),
                        //     Transform
                        // );

                        // m_Renderer->push_rectangle(
                        //     gs_vec2f(i * R * 2, j * R * 2),
                        //     gs_vec2f(i * R * 2 + R, j * R * 2 + R),
                        //     +0.f,
                        //     lineWidth,
                        //     gs_vec4f(255.f, 0.f, 0.f, 255.f),
                        //     Transform);

                        m_Renderer->push_rectangle_rounded(
                            gs_vec2f(i * R * 2, j * R * 2),
                            gs_vec2f(i * R * 2 + R, j * R * 2 + R),
                            32,
                            +0.f,
                            lineWidth,
                            gs_vec4f(255.f, 0.f, 0.f, 255.f),
                            Transform);

                        // m_Renderer->push_rectangle_filled(
                        //     gs_vec2f(i * R * 2, j * R * 2),
                        //     gs_vec2f(i * R * 2 + R, j * R * 2 + R),
                        //     +0.f,
                        //     gs_vec4f(255.f, 0.f, 0.f, 255.f),
                        //     Transform);

                        // m_Renderer->push_arc(
                        //     gs_vec2f(i * R * 2, j * R * 2),
                        //     +R,
                        //     +R * 0.5f,
                        //     0.f,
                        //     360.f,
                        //     0.f,
                        //     lineWidth,
                        //     gs_vec4f(255.f, 0.f, 0.f, 255.f),
                        //     Transform
                        // );
                    }
                }
            }

        protected:

            gs_mat4f Transform = gs_mat4f(1.f);

            std::shared_ptr<Immediate2DRenderer> m_Renderer;

        };
    }
}