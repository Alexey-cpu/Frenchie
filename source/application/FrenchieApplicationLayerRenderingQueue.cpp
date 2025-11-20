#include <FrenchieApplicationLayerRenderingQueue.hpp>

using namespace Frenchie::Application;

// Application
#include <FrenchieApplication.hpp>

// GLAD
#include <glad/glad.h>

// STB
#include <stb_image.h>
#include <stb_image_write.h>

RenderingQueue::RenderingQueue(){}
RenderingQueue::~RenderingQueue(){}

bool RenderingQueue::awake()
{
    // register default shader here
    m_DefaultShader = construct_shader(
        {
            // Vertex shader
            {
                std::string(
R"(
#version 330 core

// vertex attributes
layout (location = 0) in vec3 a_Position;
layout (location = 1) in vec3 a_Normal;
layout (location = 2) in vec2 a_UV;

// outputs
out vec3 Normal;
out vec2 UV;

// uniforms
uniform mat4 u_ModelMatrix;
uniform mat4 u_CameraViewMatrix;
uniform mat4 u_ProjectionMatrix;

void main()
{
    // setup position
    gl_Position = u_ProjectionMatrix * u_CameraViewMatrix * u_ModelMatrix * vec4(a_Position, 1.0);

    // setup outputs
    Normal = a_Normal;
    UV     = a_UV;
}            
)"),
                RenderingQueueShaderType_::RenderingQueueShaderType_Vertex
            },

            // fragment shader
            {
                std::string(
R"(
#version 330 core

// inputs
in vec2 UV;

//outputs
out vec4 fragColor;

//uniforms
uniform vec4      u_Color;
uniform sampler2D u_Texture;

void main()
{
    // setup vertex color
    //fragColor = u_Color * texture(u_Texture, UV);
    fragColor = u_Color;
}
)"),
                RenderingQueueShaderType_::RenderingQueueShaderType_Fragment
                },
        }
    );    

    // create default white pattern texture
    const int     height   = 4;
    const int     width    = 4;
    const int     channels = 4;
    const int     red      = 0;
    const int     green    = 1;
    const int     blue     = 2;
    const int     alpha    = 3;
    unsigned char image[width * height * channels]{};

    for (int y = 0; y < height; y++)
    {
        for (int x = 0; x < width; x++)
        {
            image[channels * (y * width + x) + red  ] = 255;
            image[channels * (y * width + x) + green] = 255;
            image[channels * (y * width + x) + blue ] = 255;
            image[channels * (y * width + x) + alpha] = 255;
        }
    }

    m_DefaultTexture = construct_texture(image, width, height);

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
    for (auto&& command : m_Commands)
    {
        auto mesh      = command.Mesh;
        auto shader    = command.Shader;
        auto color     = command.Texture.Color;
        auto texture   = command.Texture;
        auto transform = command.Transform;

        // compute shader projection matrix
        float width  = Frenchie::Application::application()->get_size()[0];
        float height = Frenchie::Application::application()->get_size()[1];
        float left   = -width  * 0.5f + width  * 0.5f; // The x-coordinate of the left edge of the viewable area.
        float right  = +width  * 0.5f + width  * 0.5f; // The x-coordinate of the right edge of the viewable area.
        float bottom = -height * 0.5f - height * 0.5f; // The y-coordinate of the bottom edge of the viewable area.
        float top    = +height * 0.5f - height * 0.5f; // The y-coordinate of the top edge of the viewable area.

        m_ProjectionMatrix = gs_matrix_ortho(left, right, bottom, top, -1000.0f, 1000.0f);
        m_CameraViewMatrix = gs_mat4f(1.f);

        // setup shader projection matrix
        begin_use_shader(shader);
        begin_use_mesh(mesh);
        begin_use_texture(texture);
        
        set_shader_uniform(shader, "u_ModelMatrix", transform);
        set_shader_uniform(shader, "u_CameraViewMatrix", m_CameraViewMatrix);
        set_shader_uniform(shader, "u_ProjectionMatrix", m_ProjectionMatrix);
        set_shader_uniform(shader, "u_Color", gs_vec4f(color[0] / 255.f, color[1] / 255.f, color[2] / 255.f, color[3] / 255.f));
        set_shader_uniform(shader, "u_Texture", 0);
        
        end_use_shader();
        end_use_mesh();
        end_use_texture();

        // destroy mesh
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
    destroy_shader(m_DefaultShader);
    destroy_texture(m_DefaultTexture);
}

bool RenderingQueue::allows_multiple_instances() const
{
    return false;
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
    unsigned int sampler;
    glGenTextures(1, &sampler);
    glBindTexture(GL_TEXTURE_2D, sampler); 
    
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

    // set format
    switch (_Format)
    {
    case RenderingQueueTextureFormat_::RenderingQueueTextureFormat_ALPHA:
        glTexImage2D(GL_TEXTURE_2D, 0, GL_ALPHA, _Width, _Height, 0, GL_ALPHA, GL_UNSIGNED_BYTE, _RawBuffer);
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

    // always generate mipmaps as texture can be displayed at size lower than it realy is
    glGenerateMipmap(GL_TEXTURE_2D);

    // bind texture
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, sampler);

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
    glEnableVertexAttribArray(0);
    glEnableVertexAttribArray(1);
    glEnableVertexAttribArray(2);

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