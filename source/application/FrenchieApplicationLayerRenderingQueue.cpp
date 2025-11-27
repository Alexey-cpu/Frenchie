#include <FrenchieApplicationLayerRenderingQueue.hpp>

using namespace Frenchie::Application;

// Application
#include <FrenchieApplication.hpp>

// GLAD
#include <glad/glad.h>

// STB
#include <stb_image.h>
#include <stb_truetype.h>
#include <stb_image_write.h>

RenderingQueue::RenderingQueue(){}
RenderingQueue::~RenderingQueue(){}

void RenderingQueue::set_projection_matrix(const gs_mat4f& _Matrix)
{
    m_ProjectionMatrix = _Matrix;
}

void RenderingQueue::set_cameraview_matrix(const gs_mat4f& _Matrix)
{
    m_CameraViewMatrix = _Matrix;
}


bool RenderingQueue::awake()
{
    return true;
}

void RenderingQueue::frame_start()
{
}

void RenderingQueue::frame_update()
{
}

void RenderingQueue::frame_render()
{
    for (int i = 0; i < (int)m_Commands.size(); ++i)
    {
        auto mesh      = m_Commands[i].Mesh;
        auto shader    = m_Commands[i].Shader;
        auto color     = m_Commands[i].Texture.Color;
        auto texture   = m_Commands[i].Texture;
        auto transform = m_Commands[i].Transform;

        begin_use_shader(shader);

        set_shader_uniform(shader, "u_ModelMatrix", transform);
        set_shader_uniform(shader, "u_CameraViewMatrix", m_CameraViewMatrix);
        set_shader_uniform(shader, "u_ProjectionMatrix", m_ProjectionMatrix);
        set_shader_uniform(shader, "u_Texture", 0);

        begin_use_texture(texture);
        begin_use_mesh(mesh);
        
        end_use_shader();
        end_use_texture();
        end_use_mesh();

        destroy_mesh(mesh);
    }

    // clear commands queue
    m_Commands.clear();
}

void RenderingQueue::frame_finish()
{
}

void RenderingQueue::finish()
{
}

void RenderingQueue::quit()
{
}

bool RenderingQueue::allows_multiple_instances() const
{
    return false;
}

RenderingQueueFont RenderingQueue::construct_font(unsigned char* _Memory, const int& _SizeInPixels)
{
    auto load_font_from_memory = [](unsigned char* fontBuffer)->stbtt_fontinfo*
    {
        // prepare font
        stbtt_fontinfo* info = new stbtt_fontinfo();
        info->userdata = nullptr;
        info->data     = nullptr;

        if (!stbtt_InitFont(info, fontBuffer, 0))
        {
            free(fontBuffer);
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
        new RenderingQueueGlyph[glyphsCount],
        [](RenderingQueueGlyph* _Data)
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
        
        gs_rectf BoxUV = gs_rectf(
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
        return RenderingQueueFont(
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
                colorifiedAtlasBitMap.get()[channels * (y * atlasWidth + x) + c  ] =
                    atlasBitMap.get()[(y * atlasWidth + x)];
            }
        }
    }

    return RenderingQueueFont(
        _SizeInPixels,
        (float)ascent,
        (float)descent,
        (float)lineGap,
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

RenderingQueueFont RenderingQueue::construct_font(const char* _FilePath, const int& _SizeInPixels)
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

    return construct_font(stb_open_ttf_file(_FilePath), _SizeInPixels);
}

void RenderingQueue::destroy_font(const RenderingQueueFont& _Font)
{
    destroy_texture(_Font.AtlasTexture);
}

RenderingQueueTexture RenderingQueue::construct_texture(
    const unsigned char*                  _RawBuffer,
    const int&                            _Width,
    const int&                            _Height,
    const RenderingQueueTextureFormat&    _Format,
    const RenderingQueueTextureWrapMode&  _Wrap,
    const RenderingQueueTextureMinFilter& _MinFilter,
    const RenderingQueueTextureMaxFilter& _MaxFilter)
{
    if(_RawBuffer == nullptr)
        return RenderingQueueTexture();

    // register image within platform specific low level grphics API
    //glPixelStorei(GL_UNPACK_ALIGNMENT, 1);
    
    unsigned int sampler;
    glGenTextures(1, &sampler);
    glBindTexture(GL_TEXTURE_2D, sampler); 
    
    // set format
    switch (_Format)
    {
    case RenderingQueueTextureFormat_::RenderingQueueTextureFormat_ALPHA:
        //glTexImage2D(GL_TEXTURE_2D, 0, GL_ALPHA, _Width, _Height, 0, GL_ALPHA, GL_UNSIGNED_BYTE, _RawBuffer);

        glTexImage2D(GL_TEXTURE_2D, 0, GL_R8, _Width, _Height, 0, GL_RED, GL_UNSIGNED_BYTE, _RawBuffer);
        break;
    
    case RenderingQueueTextureFormat_::RenderingQueueTextureFormat_RGB:
        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, _Width, _Height, 0, GL_RGB, GL_UNSIGNED_BYTE, _RawBuffer);
        break;

    case RenderingQueueTextureFormat_::RenderingQueueTextureFormat_RGBA:
        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, _Width, _Height, 0, GL_RGBA, GL_UNSIGNED_BYTE, _RawBuffer);
        break;

    default:
        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, _Width, _Height, 0, GL_RGBA, GL_UNSIGNED_BYTE, _RawBuffer);
        break;
    }

    // set the texture wrapping parameters
    switch (_Wrap)
    {
    case RenderingQueueTextureWrapMode_::RenderingQueueTextureWrapMode_Repeat:
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
        break;
    
    case RenderingQueueTextureWrapMode_::RenderingQueueTextureWrapMode_Mirrored:
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_MIRRORED_REPEAT);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_MIRRORED_REPEAT);
        break;

    case RenderingQueueTextureWrapMode_::RenderingQueueTextureWrapMode_ClampToEdge:
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
        break;

    case RenderingQueueTextureWrapMode_::RenderingQueueTextureWrapMode_ClampToBorder:
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_BORDER);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_BORDER);
        break;
    default:
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
        break;
    }
    
    // set minifying filter
    switch (_MinFilter)
    {
    case RenderingQueueTextureMinFilter_::RenderingQueueTextureMinFilter_Linear:
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
        break;
    
    case RenderingQueueTextureMinFilter_::RenderingQueueTextureMinFilter_Nearest:
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
        break;

    case RenderingQueueTextureMinFilter_::RenderingQueueTextureMinFilter_LinearMipMapLinear:
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
        break;

    case RenderingQueueTextureMinFilter_::RenderingQueueTextureMinFilter_LinearMipMapNearest:
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_NEAREST);
        break;

    case RenderingQueueTextureMinFilter_::RenderingQueueTextureMinFilter_Nearest_MipMapLinear:
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST_MIPMAP_LINEAR);
        break;

    case RenderingQueueTextureMinFilter_::RenderingQueueTextureMinFilter_NearestMipMapNearest:
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST_MIPMAP_NEAREST);
        break;

    default:
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
        break;
    }

    // set magnifying filter
    switch (_MaxFilter)
    {
    case RenderingQueueTextureMaxFilter_::RenderingQueueTextureMaxFilter_Linear:
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
        break;
    
    case RenderingQueueTextureMaxFilter_::RenderingQueueTextureMaxFilter_Nearest:
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
        break;

    default:
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
        break;
    }

    // always generate mipmaps as texture can be displayed at size lower than it realy is
    glGenerateMipmap(GL_TEXTURE_2D);

    // bind texture
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, 0);

    return RenderingQueueTexture(sampler, _Width, _Height, {255.f, 255.f, 255.f, 255.f}, _Format, _Wrap, _MinFilter, _MaxFilter);
}

RenderingQueueTexture RenderingQueue::construct_texture(
    const char*                           _FilePath,
    const RenderingQueueTextureFormat&     _Format,
    const RenderingQueueTextureWrapMode&  _Wrap,
    const RenderingQueueTextureMinFilter& _MinFilter, 
    const RenderingQueueTextureMaxFilter& _MaxFilter)
{
    // auxiliary lambdas
    auto formatToRequestdChannels = [](RenderingQueueTextureFormat _Format)->int
    {
        if(_Format & RenderingQueueTextureFormat_::RenderingQueueTextureFormat_RGBA)  return 4;
        if(_Format & RenderingQueueTextureFormat_::RenderingQueueTextureFormat_RGB)   return 3;
        if(_Format & RenderingQueueTextureFormat_::RenderingQueueTextureFormat_ALPHA) return 1;

        return 0; // extract everything by default
    };

    int width   {0};
    int height  {0};
    int channels{0};

    stbi_uc* buffer = stbi_load(_FilePath, &width, &height, &channels, formatToRequestdChannels(_Format));

    if(buffer == nullptr)
        return RenderingQueueTexture();

    // construct image
    auto image = construct_texture(buffer, width, height, _Format, _Wrap, _MinFilter, _MaxFilter);

    // clear raw image buffer
    stbi_image_free(buffer);

    return image;
}

void RenderingQueue::begin_use_texture(const RenderingQueueTexture& _Texture)
{
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, _Texture.Ptr);
}

void RenderingQueue::end_use_texture()
{
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, 0);
}

void RenderingQueue::destroy_texture(const RenderingQueueTexture& _Texture)
{
    glDeleteTextures(1, &_Texture.Ptr);
}

RenderingQueueShader RenderingQueue::construct_shader(const std::vector<std::pair<std::string, RenderingQueueShaderType>>& _ShaderInfos) const
{
    unsigned int shaderProgram = glCreateProgram();

    for(auto&& shaderInfo : _ShaderInfos)
    {
        auto         shaderSourceCode = shaderInfo.first;
        auto         shaderType       = shaderInfo.second;
        unsigned int shader           = 0;

        if(shaderType == RenderingQueueShaderType_::RenderingQueueShaderType_Vertex)
            shader = glCreateShader(GL_VERTEX_SHADER);
        else if(shaderType == RenderingQueueShaderType_::RenderingQueueShaderType_Fragment)
            shader = glCreateShader(GL_FRAGMENT_SHADER);

        int status = 1;
        const char* shaderSourceCodePtr = shaderSourceCode.c_str();
        glShaderSource(shader, 1, &shaderSourceCodePtr, nullptr);
        glCompileShader(shader);
        glGetShaderiv(shader, GL_COMPILE_STATUS, &status);

        if(status == GL_TRUE)
        {
            glAttachShader(shaderProgram, shader);
            glDeleteShader(shader);
        }
        else
        {
            const int logBufferSize = 4096;
            char logBuffer[logBufferSize];
            glGetShaderInfoLog(shader, logBufferSize, nullptr, logBuffer);

            printf("error: %s\n", logBuffer);
        }
    }

    int status = true;
    glLinkProgram(shaderProgram);
    glGetProgramiv(shaderProgram, GL_LINK_STATUS, &status);

    if(status == GL_FALSE)
    {
        const int logBufferSize = 4096;
        char logBuffer[logBufferSize];
        glGetProgramInfoLog(shaderProgram, logBufferSize, nullptr, logBuffer);
        
        printf("error: %s\n", logBuffer);

        return RenderingQueueShader();
    }

    return RenderingQueueShader(shaderProgram);
}

// gs_renderer_shader RenderingQueue::construct_shader(const std::vector<std::string>& _ShaderPaths) const
// {
//     // auxiliary lambdas
//     auto read_file = [](const char* _Path)->std::string
//     {
//         FILE *file_ptr;
//         char *buffer;
//         long file_size;

//         // Open the file in binary read mode
//         file_ptr = fopen(_Path, "rb");

//         if (file_ptr == NULL)
//             return std::string();

//         // Get the file size
//         fseek(file_ptr, 0, SEEK_END);
//         file_size = ftell(file_ptr);
//         rewind(file_ptr); // Go back to the beginning of the file

//         // Allocate memory for the buffer (+1 for null terminator)
//         buffer = (char*)malloc(file_size + 1);
//         if (buffer == NULL) 
//         {
//             fclose(file_ptr);
//             return std::string();
//         }

//         // Read the entire file content into the buffer
//         size_t bytes_read = fread(buffer, 1, file_size, file_ptr);
//         if (bytes_read != file_size)
//         {
//             free(buffer);
//             fclose(file_ptr);
//             return std::string();
//         }

//         // Null-terminate the buffer if you intend to treat it as a string
//         buffer[file_size] = '\0';

//         // Clean up
//         fclose(file_ptr);

//         // return null terminated buffer
//         return std::string(buffer);
//     };

//     auto file_extention = [](const char* _Extention)->std::string
//     {
//         auto begin = _Extention;
//         auto end   = _Extention[strlen(_Extention)- 1];

//         while(*begin != '\0' && *begin != '.')
//             ++begin;

//         return std::string(begin);
//     };

//     std::vector<std::pair<std::string, gs_renderer_shader_type>> shaderInfos;

//     for(auto&& path : _ShaderPaths)
//     {
//         auto source    = read_file(path.c_str());
//         auto extention = file_extention(path.c_str());

//         if(extention == ".vert" || extention == ".vsh" || extention == ".vs")
//         {
//             shaderInfos.push_back(
//                 {
//                     source,
//                     gs_renderer_shader_type_::gs_renderer_shader_type_vertex
//                 }
//             );
//         }
//         else if(extention == ".frag" || extention == ".fsh" || extention == ".fs")
//         {
//             shaderInfos.push_back(
//                 {
//                     source,
//                     gs_renderer_shader_type_::gs_renderer_shader_type_fragment
//                 }
//             );
//         }
//     }

//     auto shader = construct_shader(shaderInfos);

//     return shader;
// }

void RenderingQueue::begin_use_shader(const RenderingQueueShader& _Shader)
{
     glUseProgram(_Shader.Ptr);
}

void RenderingQueue::set_shader_uniform(const RenderingQueueShader& _Shader, const char* _Name, const bool& _Value)
{
    glUniform1i(glGetUniformLocation(_Shader.Ptr, _Name), (int)_Value);
}

void RenderingQueue::set_shader_uniform(const RenderingQueueShader& _Shader, const char* _Name, const int& _Value)
{
    glUniform1i(glGetUniformLocation(_Shader.Ptr, _Name), _Value);
}

void RenderingQueue::set_shader_uniform(const RenderingQueueShader& _Shader, const char* _Name, const float& _Value)
{
    glUniform1f(glGetUniformLocation(_Shader.Ptr, _Name), _Value);
}

void RenderingQueue::set_shader_uniform(const RenderingQueueShader& _Shader, const char* _Name, const gs_vec2f& _Value)
{
    glUniform2fv(glGetUniformLocation(_Shader.Ptr, _Name), 1, &_Value[0]);
}

void RenderingQueue::set_shader_uniform(const RenderingQueueShader& _Shader, const char* _Name, const gs_vec3f& _Value)
{
    glUniform3fv(glGetUniformLocation(_Shader.Ptr, _Name), 1, &_Value[0]);
}

void RenderingQueue::set_shader_uniform(const RenderingQueueShader& _Shader, const char* _Name, const gs_vec4f& _Value)
{
    glUniform4fv(glGetUniformLocation(_Shader.Ptr, _Name), 1, &_Value[0]);
}

void RenderingQueue::set_shader_uniform(const RenderingQueueShader& _Shader, const char* _Name, const gs_mat2f& _Value)
{
    glUniformMatrix2fv(glGetUniformLocation(_Shader.Ptr, _Name), 1, GL_FALSE, &_Value[0][0]);
}

void RenderingQueue::set_shader_uniform(const RenderingQueueShader& _Shader, const char* _Name, const gs_mat3f& _Value)
{
    glUniformMatrix3fv(glGetUniformLocation(_Shader.Ptr, _Name), 1, GL_FALSE, &_Value[0][0]);
}

void RenderingQueue::set_shader_uniform(const RenderingQueueShader& _Shader, const char* _Name, const gs_mat4f& _Value)
{
    glUniformMatrix4fv(glGetUniformLocation(_Shader.Ptr, _Name), 1, GL_FALSE, &_Value[0][0]);
}

void RenderingQueue::end_use_shader()
{
    glUseProgram(0);
}

void RenderingQueue::destroy_shader(const RenderingQueueShader& _Shader)
{
    glDeleteProgram(_Shader.Ptr);
}

RenderingQueueMesh RenderingQueue::construct_mesh(
    const RenderingQueueVertex* _Vertexes,
    const int&                  _VertexesCount,
    const int*                  _Indexes,
    const int&                  _IndexesCount)
{
    // create mesh
    unsigned int m_VBO = 0;
    unsigned int m_VAO = 0;
    unsigned int m_EBO = 0;
    
    // create buffers and vertex array
    glGenBuffers(1, &m_VBO);
    glGenBuffers(1, &m_EBO);
    glGenVertexArrays(1, &m_VAO);

    // bind VAO to remember VBO/EBO configuration and layout
    glBindVertexArray(m_VAO);

    // load vertexes and indexes on GPU
    glBindBuffer(GL_ARRAY_BUFFER, m_VBO);
    glBufferData(GL_ARRAY_BUFFER, _VertexesCount * sizeof(RenderingQueueVertex), _Vertexes, GL_DYNAMIC_DRAW);

    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_EBO);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, _IndexesCount * sizeof(int),  _Indexes, GL_DYNAMIC_DRAW);

    // setup attributes pointers
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(RenderingQueueVertex), (void*)(offsetof(RenderingQueueVertex, Position)));
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, sizeof(RenderingQueueVertex), (void*)(offsetof(RenderingQueueVertex, Normal)));
    glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, sizeof(RenderingQueueVertex), (void*)(offsetof(RenderingQueueVertex, UV)));
    glVertexAttribPointer(3, 4, GL_FLOAT, GL_FALSE, sizeof(RenderingQueueVertex), (void*)(offsetof(RenderingQueueVertex, Color)));
    glEnableVertexAttribArray(0);
    glEnableVertexAttribArray(1);
    glEnableVertexAttribArray(2);
    glEnableVertexAttribArray(3);

    glBindVertexArray(0);

    return RenderingQueueMesh(m_VBO, m_VAO, m_EBO);
}

void RenderingQueue::begin_use_mesh(
    const RenderingQueueMesh&          _Mesh,
    const RenderingQueueRendererHints& _MeshRenderHints)
{
    // bind VAO containing VBO, EBO
    glBindVertexArray(_Mesh.VAO);

    // get EBO size
    int bufferSize; 
    glGetBufferParameteriv(GL_ELEMENT_ARRAY_BUFFER, GL_BUFFER_SIZE, &bufferSize);

    // draw EBO
    switch (_MeshRenderHints)
    {
    case RenderingQueueRendererHints_::RenderingQueueRendererHints_Points:
    glDrawArrays(GL_POINTS, 0, bufferSize);
        break;
    case RenderingQueueRendererHints_::RenderingQueueRendererHints_Lines:
    glDrawElements(GL_LINE_LOOP, bufferSize, GL_UNSIGNED_INT, 0);
        break;
    case RenderingQueueRendererHints_::RenderingQueueRendererHints_Triangles:
    glDrawElements(GL_TRIANGLES, bufferSize, GL_UNSIGNED_INT, 0);
        break;

    default:
        glDrawElements(GL_TRIANGLES, bufferSize, GL_UNSIGNED_INT, 0);
        break;
    }
}

void RenderingQueue::end_use_mesh()
{
    glBindVertexArray(0);
}

void RenderingQueue::destroy_mesh(const RenderingQueueMesh& _Mesh)
{
    glDeleteBuffers(1, &_Mesh.VBO);
    glDeleteBuffers(1, &_Mesh.EBO);
    glDeleteVertexArrays(1, &_Mesh.VAO);
}

void RenderingQueue::push_command(
    const RenderingQueueMesh&    _Mesh,
    const RenderingQueueShader&  _Shader,
    const RenderingQueueTexture& _Texture,
    const gs_mat4f&              _Transform)
{
    m_Commands.push_back(RenderingQueueCommand(_Mesh, _Shader, _Texture, _Transform));
}