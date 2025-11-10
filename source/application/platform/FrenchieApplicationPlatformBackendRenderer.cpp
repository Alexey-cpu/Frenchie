#include <FrenchieApplicationPlatformRendererBackend.hpp>

// Core
#include <FrenchieCoreLogger.hpp>
#include <FrenchieCoreFileSystem.hpp>
#include <FrenchieCoreStringUnicode.hpp>

// GLAD
#include <glad/glad.h> 

// GLFW
#include <GLFW/glfw3.h>

// STB
#include <stb_image.h>
#include <stb_image_write.h>

// STL
#include <fstream>
#include <iostream>

using namespace Frenchie::Core::FileSystem;
using namespace Frenchie::Application;

namespace Frenchie
{
    namespace Application
    {
        // Callbacks
        void OpenGLPlatformBackendOnWindowResize(GLFWwindow* _Window, int _Width, int _Height)
        {
            (void)_Window;
            glViewport(0, 0, _Width, _Height);
        }

        void OpenGLPlatformBackendOnWindowMaximizedCallback(GLFWwindow* _Window, int _Maximized)
        {
            int width  = 0;
            int height = 0;
            glfwGetWindowSize(_Window, &width, &height);
            glViewport(0, 0, width, height);
        }

        // void OpenGLPlatformBackendPushStatusMessage(char*  _Message, char** _ImageLoadStatusBuffer)
        // {
        //     if(_ImageLoadStatusBuffer == nullptr)
        //         return;

        //     if(strlen(*_ImageLoadStatusBuffer) < strlen(_Message))
        //     {
        //         *_ImageLoadStatusBuffer = 
        //             (char*)realloc(*_ImageLoadStatusBuffer, 2 * strlen(_Message) * sizeof(char));
        //     }

        //     strcpy(*_ImageLoadStatusBuffer, _Message);
        // }

        // template<typename Head>
        // inline Head __max__(const Head& _A, const Head& _B)
        // {
        //     return _A > _B ? _A : _B;
        // }

        // template<typename Head, typename ... Args>
        // Head __max__(const Head& _A, const Head& _B, Args... _Args)
        // {
        //     return __max__(__max__(_A, _B), _Args...);
        // }

        // template<typename Head>
        // inline  Head __min__(const Head& _A, const Head&)
        // {
        //     return _A < _B ? _A : _B;
        // }

        // template<typename Head, typename ... Args>
        // Head __min__(const Head& _A, const Head& _B, Args... _Args)
        // {
        //     return __min__(__min__( _A, _B ), _Args...);
        // }
    }
}

PlatformRendererBackend::PlatformRendererBackend() //:
    // m_Textures(std::make_unique<Frenchie::Core::Memory::MemoryChunkAllocator<PlatformRendererBackendTexture>>(32)),
    // m_Meshes(std::make_unique<Frenchie::Core::Memory::MemoryChunkAllocator<PlatformRendererBackendMesh>>(32)),
    // m_Shaders(std::make_unique<Frenchie::Core::Memory::MemoryChunkAllocator<PlatformRendererBackendShader>>(32))
{
}

PlatformRendererBackend::~PlatformRendererBackend()
{
}

void* PlatformRendererBackend::get_context() const
{
    return m_Context;
}

glm::vec4 PlatformRendererBackend::get_context_window_clear_color() const
{
    return m_ClearColor;
}

glm::vec2 PlatformRendererBackend::get_context_window_position() const
{
    int x = 0;
    int y = 0;
    glfwGetWindowPos(reinterpret_cast<GLFWwindow*>(m_Context), &x, &y);
    return {x, y};
}

glm::vec2 PlatformRendererBackend::get_context_window_size() const
{
    int x = 0;
    int y = 0;
    glfwGetWindowSize(reinterpret_cast<GLFWwindow*>(m_Context), &x, &y);
    return {x, y};
}

void PlatformRendererBackend::set_context_window_clear_color(const glm::vec4& _Value)
{
    m_ClearColor = _Value;
}

void PlatformRendererBackend::set_context_window_position(const glm::vec2& _Value)
{
    glfwSetWindowPos(reinterpret_cast<GLFWwindow*>(m_Context), (int)_Value.x, (int)_Value.y);
}

void PlatformRendererBackend::set_context_window_size(const glm::vec2& _Value)
{
    glfwSetWindowSize(reinterpret_cast<GLFWwindow*>(m_Context), (int)_Value.x, (int)_Value.y);
}

bool PlatformRendererBackend::awake(
    const char*                       _Name,
    void*                             _Share,
    PlatformRendererBackendContextHints _WindowHints)
{
    // initialization
    if(glfwInit() == GLFW_FALSE)
        return false;

    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

#ifdef __APPLE__
    glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
#endif

    if(_WindowHints & PlatformRendererBackendContextHints_::PlatformRendererBackendContextHints_Visible)
        glfwWindowHint(GLFW_VISIBLE, GLFW_TRUE);
    else
        glfwWindowHint(GLFW_VISIBLE, GLFW_FALSE);

    if(_WindowHints & PlatformRendererBackendContextHints_::PlatformRendererBackendContextHints_Decorated)
        glfwWindowHint(GLFW_DECORATED, GLFW_TRUE);
    else
        glfwWindowHint(GLFW_DECORATED, GLFW_FALSE);

    if(_WindowHints & PlatformRendererBackendContextHints_::PlatformRendererBackendContextHints_Resizable)
        glfwWindowHint(GLFW_RESIZABLE, GLFW_TRUE);
    else
        glfwWindowHint(GLFW_RESIZABLE, GLFW_FALSE);

    if(_WindowHints & PlatformRendererBackendContextHints_::PlatformRendererBackendContextHints_Iconified)
        glfwWindowHint(GLFW_ICONIFIED, GLFW_TRUE);
    else
        glfwWindowHint(GLFW_ICONIFIED, GLFW_FALSE);

    if(_WindowHints & PlatformRendererBackendContextHints_::PlatformRendererBackendContextHints_Focused)
        glfwWindowHint(GLFW_FOCUSED, GLFW_TRUE);
    else
        glfwWindowHint(GLFW_FOCUSED, GLFW_FALSE);

    // create context
    m_Context = glfwCreateWindow(
        512,
        256,
        _Name,
        nullptr,
        reinterpret_cast<GLFWwindow*>(_Share));

    if(m_Context == nullptr)
    {
        glfwTerminate();
        return false;
    }

    glfwMakeContextCurrent(reinterpret_cast<GLFWwindow*>(m_Context));

    // setup callbacks
    glfwSetWindowSizeCallback(reinterpret_cast<GLFWwindow*>(m_Context), &OpenGLPlatformBackendOnWindowResize);
    glfwSetFramebufferSizeCallback(reinterpret_cast<GLFWwindow*>(m_Context), &OpenGLPlatformBackendOnWindowResize);
    glfwSetWindowMaximizeCallback(reinterpret_cast<GLFWwindow*>(m_Context), &OpenGLPlatformBackendOnWindowMaximizedCallback);

    // load OpenGL interface using GLAD
    if(!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress))
    {
        glfwTerminate();
        return false;
    }

    // maximize context window
    glfwMaximizeWindow(reinterpret_cast<GLFWwindow*>(m_Context));

    // call window maximize callback if the Window has been maximized
    OpenGLPlatformBackendOnWindowMaximizedCallback(
        reinterpret_cast<GLFWwindow*>(m_Context),
        glfwGetWindowAttrib(reinterpret_cast<GLFWwindow*>(m_Context), GLFW_MAXIMIZED));

    return true;
}

void PlatformRendererBackend::frame_start(const PlatformRendererBackendHints& _PlatformRendererBackendHints)
{
    if((_PlatformRendererBackendHints & PlatformRendererBackendHints_::PlatformRendererBackendHints_ClearColorBuffer))
        glClear(GL_COLOR_BUFFER_BIT);
    
    if((_PlatformRendererBackendHints & PlatformRendererBackendHints_::PlatformRendererBackendHints_ClearDepthBuffer))
        glClear(GL_DEPTH_BUFFER_BIT);
    
    if((_PlatformRendererBackendHints & PlatformRendererBackendHints_::PlatformRendererBackendHints_ClearStencilBuffer))
        glClear(GL_STENCIL_BUFFER_BIT);

    if((_PlatformRendererBackendHints & PlatformRendererBackendHints_::PlatformRendererBackendHints_PollEvents))
        glfwPollEvents();

    glClearColor(m_ClearColor.x / 255.f, m_ClearColor.y / 255.f, m_ClearColor.z / 255.f, m_ClearColor.w / 255.f);

    glfwSwapInterval(1);
}

void PlatformRendererBackend::frame_update()
{
    // stretch viewport to cover all the context window
    int display_w = 0;
    int display_h = 0;
    glfwGetFramebufferSize(reinterpret_cast<GLFWwindow*>(m_Context), &display_w, &display_h);
    glViewport(0, 0, display_w, display_h);

    // setup context info
}

void PlatformRendererBackend::frame_render()
{
    // TODO: execute rendering commands here
}

void PlatformRendererBackend::frame_finish()
{
    glfwSwapBuffers(reinterpret_cast<GLFWwindow*>(m_Context));
}

void PlatformRendererBackend::finish()
{
    // TODO: clean up renderer commands here
}

void PlatformRendererBackend::quit()
{
    glfwDestroyWindow(reinterpret_cast<GLFWwindow*>(m_Context));
    glfwTerminate();
    m_Context = nullptr;
}

bool PlatformRendererBackend::is_closed() const
{
    return glfwWindowShouldClose(reinterpret_cast<GLFWwindow*>(m_Context));
}

void PlatformRendererBackend::close()
{
    glfwSetWindowShouldClose(reinterpret_cast<GLFWwindow*>(m_Context), GL_TRUE);
}

PlatformRendererBackendTexture PlatformRendererBackend::construct_image(
    const unsigned char*                           _RawBuffer,
    const int&                                     _Width,
    const int&                                     _Height,
    const PlatformRendererBackendTextureFormat&    _Format,
    const PlatformRendererBackendTextureWrap&      _Wrap,
    const PlatformRendererBackendTextureMinFilter& _MinFilter, 
    const PlatformRendererBackendTextureMaxFilter& _MaxFilter) const
{
    if(_RawBuffer == nullptr)
        return PlatformRendererBackendTexture();

    // register image within platform specific low level grphics API
    unsigned int sampler;
    glGenTextures(1, &sampler);
    glBindTexture(GL_TEXTURE_2D, sampler); 
    
    // set the texture wrapping parameters
    if((_Wrap & PlatformRendererBackendTextureWrap_::PlatformRendererBackendTextureWrap_Repeat))
    {
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
    }
    else if((_Wrap & PlatformRendererBackendTextureWrap_::PlatformRendererBackendTextureWrap_Mirrored))
    {
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_MIRRORED_REPEAT);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_MIRRORED_REPEAT);
    }
    else if((_Wrap & PlatformRendererBackendTextureWrap_::PlatformRendererBackendTextureWrap_ClampToEdge))
    {
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    }
    else if((_Wrap & PlatformRendererBackendTextureWrap_::PlatformRendererBackendTextureWrap_ClampToBorder))
    {
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_BORDER);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_BORDER);
    }
    
    // set minifying filter
    if((_MinFilter & PlatformRendererBackendTextureMinFilter_::PlatformRendererBackendTextureMinFilter_Linear))
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    else if((_MinFilter & PlatformRendererBackendTextureMinFilter_::PlatformRendererBackendTextureMinFilter_Nearest))
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    else if((_MinFilter & PlatformRendererBackendTextureMinFilter_::PlatformRendererBackendTextureMinFilter_LinearMipMapLinear))
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
    else if((_MinFilter & PlatformRendererBackendTextureMinFilter_::PlatformRendererBackendTextureMinFilter_LinearMipMapNearest))
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_NEAREST);
    else if((_MinFilter & PlatformRendererBackendTextureMinFilter_::PlatformRendererBackendTextureMinFilter_NearestMipMapLinear))
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST_MIPMAP_LINEAR);
    else if((_MinFilter & PlatformRendererBackendTextureMinFilter_::PlatformRendererBackendTextureMinFilter_NearestMipMapNearest))
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST_MIPMAP_NEAREST);
    
    // set magnifying filter
    if((_MaxFilter & PlatformRendererBackendTextureMaxFilter_::PlatformRendererBackendTextureMaxFilter_Linear))
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    else if((_MaxFilter & PlatformRendererBackendTextureMaxFilter_::PlatformRendererBackendTextureMaxFilter_Nearest))
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);

    // set format
    if((_Format & PlatformRendererBackendTextureFormat_::PlatformRendererBackendTextureFormat_ALPHA))
        glTexImage2D(GL_TEXTURE_2D, 0, GL_ALPHA, _Width, _Height, 0, GL_ALPHA, GL_UNSIGNED_BYTE, _RawBuffer);
    else if((_Format & PlatformRendererBackendTextureFormat_::PlatformRendererBackendTextureFormat_RGB))
        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, _Width, _Height, 0, GL_RGB, GL_UNSIGNED_BYTE, _RawBuffer);
    else if((_Format & PlatformRendererBackendTextureFormat_::PlatformRendererBackendTextureFormat_RGBA))
        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, _Width, _Height, 0, GL_RGBA, GL_UNSIGNED_BYTE, _RawBuffer);

    // always generate mipmaps as texture can be displayed at size lower than it realy is
    glGenerateMipmap(GL_TEXTURE_2D);

    // bind texture
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, sampler);

    return PlatformRendererBackendTexture(sampler, _Width, _Height, _Format, _Wrap, _MinFilter, _MaxFilter);
}

PlatformRendererBackendTexture PlatformRendererBackend::construct_image(
    const std::filesystem::path&                   _FilePath,
    const PlatformRendererBackendTextureFormat&    _Format,
    const PlatformRendererBackendTextureWrap&      _Wrap,
    const PlatformRendererBackendTextureMinFilter& _MinFilter, 
    const PlatformRendererBackendTextureMaxFilter& _MaxFilter) const
{
    // auxiliary lambdas
    auto formatToRequestdChannels = [](PlatformRendererBackendTextureFormat _Format)->int
    {
        if(_Format & PlatformRendererBackendTextureFormat_::PlatformRendererBackendTextureFormat_RGBA)  return 4;
        if(_Format & PlatformRendererBackendTextureFormat_::PlatformRendererBackendTextureFormat_RGB)   return 3;
        if(_Format & PlatformRendererBackendTextureFormat_::PlatformRendererBackendTextureFormat_ALPHA) return 1;

        return 0; // extract everything by default
    };

    int width   {0};
    int height  {0};
    int channels{0};

    stbi_uc* buffer = stbi_load(
        Frenchie::Core::String::convert_utf32_to_utf8(_FilePath.u32string()).c_str(),
        &width,
        &height,
        &channels,
        formatToRequestdChannels(_Format));

    if(buffer == nullptr)
    {
        // TODO: add log here
        return PlatformRendererBackendTexture();
    }

    // construct image
    auto image =
        construct_image(buffer, width, height, _Format, _Wrap, _MinFilter, _MaxFilter);

    // clear raw image buffer
    stbi_image_free(buffer);

    return image;
}

void PlatformRendererBackend::destroy_image(const PlatformRendererBackendTexture& _Texture)
{
    glDeleteTextures(1, &_Texture.Ptr);
}

PlatformRendererBackendShader PlatformRendererBackend::construct_shader(
const std::vector<std::pair<std::string, PlatformRendererBackendShaderType>>& _ShaderInfos) const
{
    unsigned int shaderProgram = glCreateProgram();

    for(auto&& shaderInfo : _ShaderInfos)
    {
        auto[shaderSourceCode, shaderType] = shaderInfo;

        unsigned int shader = 0;
        if(shaderType == PlatformRendererBackendShaderType_::PlatformRendererBackendShaderType_Vertex)
        {
            shader = glCreateShader(GL_VERTEX_SHADER);
        }
        else if(shaderType == PlatformRendererBackendShaderType_::PlatformRendererBackendShaderType_Fragment)
        {
            shader = glCreateShader(GL_FRAGMENT_SHADER);
        }

        if(shader == 0) Frenchie::Core::Logger::instance()->critical("Shader creation error !!! {}", shaderType);

        int status = 1;
        const char* shaderSourceCodePtr = shaderSourceCode.c_str();
        glShaderSource(shader, 1, &shaderSourceCodePtr, nullptr);
        glCompileShader(shader);
        glGetShaderiv(shader, GL_COMPILE_STATUS, &status);

        if(status == GL_TRUE)
        {
            glAttachShader(shaderProgram, shader);

            // after shader is loaded on GPU we can delete it
            glDeleteShader(shader);
        }
        else
        {
            const int logBufferSize = 512;
            char logBuffer[logBufferSize];
            glGetShaderInfoLog(shader, logBufferSize, nullptr, logBuffer);
            Frenchie::Core::Logger::instance()->critical("Could not compile shader");
            Frenchie::Core::Logger::instance()->critical(std::string(logBuffer));
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
        Frenchie::Core::Logger::instance()->critical("Could not link shaders");
        Frenchie::Core::Logger::instance()->critical(std::string(logBuffer));
        return PlatformRendererBackendShader();
    }

    return PlatformRendererBackendShader(shaderProgram);
}

PlatformRendererBackendShader PlatformRendererBackend::construct_shader(
    const std::vector<std::filesystem::path>& _ShaderPaths )const
{
    std::vector<std::pair<
        std::string,
        PlatformRendererBackendShaderType>> shaderInfos;

    for(auto&& path : _ShaderPaths)
    {
        std::ifstream ifsream(path);

        std::string source = 
            std::string(
                (std::istreambuf_iterator<char>(ifsream)), 
                (std::istreambuf_iterator<char>()));

        auto extention = get_file_extention(path);

        if(extention == ".vert" || extention == ".vsh" || extention == ".vs")
        {
            shaderInfos.push_back(
                {
                    source,
                    PlatformRendererBackendShaderType_::PlatformRendererBackendShaderType_Vertex
                }
            );
        }
        else if(extention == ".frag" || extention == ".fsh" || extention == ".fs")
        {
            shaderInfos.push_back(
                {
                    source,
                    PlatformRendererBackendShaderType_::PlatformRendererBackendShaderType_Vertex
                }
            );
        }
    }

    return construct_shader(shaderInfos);
}

void PlatformRendererBackend::destroy_shader(const PlatformRendererBackendShader& _Shader)
{
    glDeleteProgram(_Shader.Ptr);
}

void PlatformRendererBackend::set_shader_uniform(const PlatformRendererBackendShader& _Shader, const std::string& _Name, const bool& _Value)
{
    glUniform1i(glGetUniformLocation(_Shader.Ptr, _Name.c_str()), (int)_Value);
}

void PlatformRendererBackend::set_shader_uniform(const PlatformRendererBackendShader& _Shader, const std::string& _Name, const int& _Value)
{
    glUniform1i(glGetUniformLocation(_Shader.Ptr, _Name.c_str()), _Value);
}

void PlatformRendererBackend::set_shader_uniform(const PlatformRendererBackendShader& _Shader, const std::string& _Name, const float& _Value)
{
    glUniform1f(glGetUniformLocation(_Shader.Ptr, _Name.c_str()), _Value);
}

void PlatformRendererBackend::set_shader_uniform(const PlatformRendererBackendShader& _Shader, const std::string& _Name, const glm::vec2& _Value)
{
    glUniform2fv(glGetUniformLocation(_Shader.Ptr, _Name.c_str()), 1, &_Value[0]);
}

void PlatformRendererBackend::set_shader_uniform(const PlatformRendererBackendShader& _Shader, const std::string& _Name, const glm::vec3& _Value)
{
    glUniform3fv(glGetUniformLocation(_Shader.Ptr, _Name.c_str()), 1, &_Value[0]);
}

void PlatformRendererBackend::set_shader_uniform(const PlatformRendererBackendShader& _Shader, const std::string& _Name, const glm::vec4& _Value)
{
    glUniform4fv(glGetUniformLocation(_Shader.Ptr, _Name.c_str()), 1, &_Value[0]);
}

void PlatformRendererBackend::set_shader_uniform(const PlatformRendererBackendShader& _Shader, const std::string& _Name, const glm::mat2& _Value)
{
    glUniformMatrix2fv(glGetUniformLocation(_Shader.Ptr, _Name.c_str()), 1, GL_FALSE, &_Value[0][0]);
}

void PlatformRendererBackend::set_shader_uniform(const PlatformRendererBackendShader& _Shader, const std::string& _Name, const glm::mat3& _Value)
{
    glUniformMatrix3fv(glGetUniformLocation(_Shader.Ptr, _Name.c_str()), 1, GL_FALSE, &_Value[0][0]);
}

void PlatformRendererBackend::set_shader_uniform(const PlatformRendererBackendShader& _Shader, const std::string& _Name, const glm::mat4& _Value)
{
    glUniformMatrix4fv(glGetUniformLocation(_Shader.Ptr, _Name.c_str()), 1, GL_FALSE, &_Value[0][0]);
}

void PlatformRendererBackend::begin_use_shader(const PlatformRendererBackendShader& _Shader)
{
    glUseProgram(_Shader.Ptr);
}

void PlatformRendererBackend::endup_use_shader()
{
    glUseProgram(0);
}

PlatformRendererBackendMesh PlatformRendererBackend::construct_mesh(
    const PlatformRendererBackendMeshVertex* _Vertexes,
    const int&                               _VertexesCount,
    const int*                               _Indexes,
    const int&                               _IndexesCount
)
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
    glBufferData(GL_ARRAY_BUFFER, _VertexesCount * sizeof(PlatformRendererBackendMeshVertex), _Vertexes, GL_DYNAMIC_DRAW);

    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_EBO);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, _IndexesCount * sizeof(int),  _Indexes, GL_DYNAMIC_DRAW);

    // setup attributes pointers
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(PlatformRendererBackendMeshVertex), (void*)(offsetof(PlatformRendererBackendMeshVertex, Position)));
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, sizeof(PlatformRendererBackendMeshVertex), (void*)(offsetof(PlatformRendererBackendMeshVertex, Normal)));
    glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, sizeof(PlatformRendererBackendMeshVertex), (void*)(offsetof(PlatformRendererBackendMeshVertex, UV)));
    glEnableVertexAttribArray(0);
    glEnableVertexAttribArray(1);
    glEnableVertexAttribArray(2);

    return PlatformRendererBackendMesh(m_VBO, m_VAO, m_EBO);
}

void PlatformRendererBackend::destroy_mesh(const PlatformRendererBackendMesh& _Mesh)
{
    glDeleteBuffers(1, &_Mesh.VBO);
    glDeleteBuffers(1, &_Mesh.EBO);
    glDeleteVertexArrays(1, &_Mesh.VAO);
}

void PlatformRendererBackend::begin_render_mesh(
    const PlatformRendererBackendMesh&        _Mesh,
    PlatformRendererBackendMeshRenderingHints _MeshRenderHints)
{
    // bind VAO containing VBO, EBO
    glBindVertexArray(_Mesh.VAO);

    // get EBO size
    int bufferSize; 
    glGetBufferParameteriv(GL_ELEMENT_ARRAY_BUFFER, GL_BUFFER_SIZE, &bufferSize);

    // draw EBO
    if((_MeshRenderHints & PlatformRendererBackendMeshRenderingHints_::PlatformRendererBackendMeshRenderingHints_Points))
        glDrawArrays(GL_POINTS, 0, bufferSize);

    if((_MeshRenderHints & PlatformRendererBackendMeshRenderingHints_::PlatformRendererBackendMeshRenderingHints_Lines))
        glDrawElements(GL_LINE_LOOP, bufferSize, GL_UNSIGNED_INT, 0);
    
    if((_MeshRenderHints & PlatformRendererBackendMeshRenderingHints_::PlatformRendererBackendMeshRenderingHints_Triangles))
        glDrawElements(GL_TRIANGLES, bufferSize, GL_UNSIGNED_INT, 0);
}

void PlatformRendererBackend::endup_render_mesh()
{
    glBindVertexArray(0);
}