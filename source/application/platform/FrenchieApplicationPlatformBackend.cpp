#include <FrenchieApplicationPlatformBackend.hpp>

// Core
#include <FrenchieCoreFileSystem.hpp>

// GLAD
#include <glad/glad.h> 

// GLFW
#include <GLFW/glfw3.h>

// STB
#include <stb_image.h>
#include <stb_image_write.h>

// STL
#include <fstream>

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

PlatformBackendRenderer::PlatformBackendRenderer() :
    m_Textures(new PlatformBackendRendererAllocator<PlatformBackendRendererTexture>(32)),
    m_Shaders(new PlatformBackendRendererAllocator<PlatformBackendRendererShader>(32))
{
}

PlatformBackendRenderer::~PlatformBackendRenderer()
{
    delete m_Textures;
    delete m_Shaders;
}

void* PlatformBackendRenderer::get_context() const
{
    return m_Context;
}

bool PlatformBackendRenderer::awake(
    const char*                       _Name,
    void*                             _Share,
    PlatformBackendContextWindowHints _WindowHints)
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

    if(_WindowHints & PlatformBackendRendererContextHints_::PlatformBackendRendererContextHints_Visible)
        glfwWindowHint(GLFW_VISIBLE, GLFW_TRUE);
    else
        glfwWindowHint(GLFW_VISIBLE, GLFW_FALSE);

    if(_WindowHints & PlatformBackendRendererContextHints_::PlatformBackendRendererContextHints_Decorated)
        glfwWindowHint(GLFW_DECORATED, GLFW_TRUE);
    else
        glfwWindowHint(GLFW_DECORATED, GLFW_FALSE);

    if(_WindowHints & PlatformBackendRendererContextHints_::PlatformBackendRendererContextHints_Resizable)
        glfwWindowHint(GLFW_RESIZABLE, GLFW_TRUE);
    else
        glfwWindowHint(GLFW_RESIZABLE, GLFW_FALSE);

    if(_WindowHints & PlatformBackendRendererContextHints_::PlatformBackendRendererContextHints_Iconified)
        glfwWindowHint(GLFW_ICONIFIED, GLFW_TRUE);
    else
        glfwWindowHint(GLFW_ICONIFIED, GLFW_FALSE);

    if(_WindowHints & PlatformBackendRendererContextHints_::PlatformBackendRendererContextHints_Focused)
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

void PlatformBackendRenderer::frame_start(const PlatformBackendRendererHints& _PlatformBackendRendererHints)
{
    if((_PlatformBackendRendererHints & PlatformBackendRendererHints_::PlatformBackendRendererHints_ClearColorBuffer))
        glClear(GL_COLOR_BUFFER_BIT);
    
    if((_PlatformBackendRendererHints & PlatformBackendRendererHints_::PlatformBackendRendererHints_ClearDepthBuffer))
        glClear(GL_DEPTH_BUFFER_BIT);
    
    if((_PlatformBackendRendererHints & PlatformBackendRendererHints_::PlatformBackendRendererHints_ClearStencilBuffer))
        glClear(GL_STENCIL_BUFFER_BIT);

    if((_PlatformBackendRendererHints & PlatformBackendRendererHints_::PlatformBackendRendererHints_PollEvents))
        glfwPollEvents();

    glfwSwapInterval(1);
}

void PlatformBackendRenderer::frame_update()
{
    int display_w = 0;
    int display_h = 0;
    glfwGetFramebufferSize(reinterpret_cast<GLFWwindow*>(m_Context), &display_w, &display_h);
    glViewport(0, 0, display_w, display_h);
}

void PlatformBackendRenderer::frame_render()
{
    // TODO: execute rendering commands here
}

void PlatformBackendRenderer::frame_finish()
{
    glfwSwapBuffers(reinterpret_cast<GLFWwindow*>(m_Context));
}

void PlatformBackendRenderer::finish()
{
    // TODO: clean up renderer commands here
}

void PlatformBackendRenderer::quit()
{
    glfwDestroyWindow(reinterpret_cast<GLFWwindow*>(m_Context));
    glfwTerminate();
    m_Context = nullptr;
}

bool PlatformBackendRenderer::is_closed() const
{
    return glfwWindowShouldClose(reinterpret_cast<GLFWwindow*>(m_Context));
}

void PlatformBackendRenderer::close()
{
    glfwSetWindowShouldClose(reinterpret_cast<GLFWwindow*>(m_Context), GL_TRUE);
}

PlatformBackendRendererTexture* PlatformBackendRenderer::construct_image(
    const unsigned char*                           _RawBuffer,
    const int&                                     _Width,
    const int&                                     _Height,
    const PlatformBackendRendererTextureFormat&    _Format,
    const PlatformBackendRendererTextureWrap&      _Wrap,
    const PlatformBackendRendererTextureMinFilter& _MinFilter, 
    const PlatformBackendRendererTextureMaxFilter& _MaxFilter
)
{
    if(_RawBuffer == nullptr)
        return nullptr;

    // register image within platform specific low level grphics API
    unsigned int sampler;
    glGenTextures(1, &sampler);
    glBindTexture(GL_TEXTURE_2D, sampler); 
    
    // set the texture wrapping parameters
    if((_Wrap & PlatformBackendRendererTextureWrap_::PlatformBackendRendererTextureWrap_Repeat))
    {
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
    }
    else if((_Wrap & PlatformBackendRendererTextureWrap_::PlatformBackendRendererTextureWrap_Mirrored))
    {
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_MIRRORED_REPEAT);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_MIRRORED_REPEAT);
    }
    else if((_Wrap & PlatformBackendRendererTextureWrap_::PlatformBackendRendererTextureWrap_ClampToEdge))
    {
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    }
    else if((_Wrap & PlatformBackendRendererTextureWrap_::PlatformBackendRendererTextureWrap_ClampToBorder))
    {
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_BORDER);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_BORDER);
    }
    
    // set minifying filter
    if((_MinFilter & PlatformBackendRendererTextureMinFilter_::PlatformBackendRendererTextureMinFilter_Linear))
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    else if((_MinFilter & PlatformBackendRendererTextureMinFilter_::PlatformBackendRendererTextureMinFilter_Nearest))
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    else if((_MinFilter & PlatformBackendRendererTextureMinFilter_::PlatformBackendRendererTextureMinFilter_LinearMipMapLinear))
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
    else if((_MinFilter & PlatformBackendRendererTextureMinFilter_::PlatformBackendRendererTextureMinFilter_LinearMipMapNearest))
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_NEAREST);
    else if((_MinFilter & PlatformBackendRendererTextureMinFilter_::PlatformBackendRendererTextureMinFilter_NearestMipMapLinear))
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST_MIPMAP_LINEAR);
    else if((_MinFilter & PlatformBackendRendererTextureMinFilter_::PlatformBackendRendererTextureMinFilter_NearestMipMapNearest))
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST_MIPMAP_NEAREST);
    
    // set magnifying filter
    if((_MaxFilter & PlatformBackendRendererTextureMaxFilter_::PlatformBackendRendererTextureMaxFilter_Linear))
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    else if((_MaxFilter & PlatformBackendRendererTextureMaxFilter_::PlatformBackendRendererTextureMaxFilter_Nearest))
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);

    // set format
    if((_Format & PlatformBackendRendererTextureFormat_::PlatformBackendRendererTextureFormat_ALPHA))
        glTexImage2D(GL_TEXTURE_2D, 0, GL_ALPHA, _Width, _Height, 0, GL_ALPHA, GL_UNSIGNED_BYTE, _RawBuffer);
    else if((_Format & PlatformBackendRendererTextureFormat_::PlatformBackendRendererTextureFormat_RGB))
        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, _Width, _Height, 0, GL_RGB, GL_UNSIGNED_BYTE, _RawBuffer);
    else if((_Format & PlatformBackendRendererTextureFormat_::PlatformBackendRendererTextureFormat_RGBA))
        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, _Width, _Height, 0, GL_RGBA, GL_UNSIGNED_BYTE, _RawBuffer);

    // always generate mipmaps as texture can be displayed at size lower than it realy is
    glGenerateMipmap(GL_TEXTURE_2D);

    // bind texture
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, sampler);

    return m_Textures->construct(sampler, _Width, _Height, _Format, _Wrap, _MinFilter, _MaxFilter);
}

PlatformBackendRendererTexture* PlatformBackendRenderer::construct_image(
    const char*                                    _FilePath,
    const PlatformBackendRendererTextureFormat&    _Format,
    const PlatformBackendRendererTextureWrap&      _Wrap,
    const PlatformBackendRendererTextureMinFilter& _MinFilter, 
    const PlatformBackendRendererTextureMaxFilter& _MaxFilter
)
{
    // auxiliary lambdas
    auto formatToRequestdChannels = [](PlatformBackendRendererTextureFormat _Format)->int
    {
        if(_Format & PlatformBackendRendererTextureFormat_::PlatformBackendRendererTextureFormat_RGBA)  return 4;
        if(_Format & PlatformBackendRendererTextureFormat_::PlatformBackendRendererTextureFormat_RGB)   return 3;
        if(_Format & PlatformBackendRendererTextureFormat_::PlatformBackendRendererTextureFormat_ALPHA) return 1;

        return 0; // extract everything by default
    };

    int width   {0};
    int height  {0};
    int channels{0};

    stbi_uc* buffer =
        stbi_load(_FilePath, &width, &height, &channels, formatToRequestdChannels(_Format));

    if(buffer == nullptr)
    {
        // TODO: add log here
        return nullptr;
    }

    // construct image
    PlatformBackendRendererTexture* image =
        construct_image(buffer, width, height, _Format, _Wrap, _MinFilter, _MaxFilter);

    // clear raw image buffer
    stbi_image_free(buffer);

    return image;
}

void PlatformBackendRenderer::destroy_image(PlatformBackendRendererTexture* _Image)
{
    if(_Image == nullptr)
        return;
    
    // deallocate texture on GPU
    glDeleteTextures(1, &_Image->Ptr);

    // destroy texture data on CPU
    m_Textures->destroy(_Image);
}

PlatformBackendRendererShader* PlatformBackendRenderer::construct_shader(
const std::vector<std::pair<std::string, PlatformBackendRendererShaderType>>& _ShaderInfos
)
{
    unsigned int shaderProgram = glCreateProgram();

    for(auto&& shaderInfo : _ShaderInfos)
    {
        auto[shaderSourceCode, shaderType] = shaderInfo;

        unsigned int shader = 0;
        if((shaderType & PlatformBackendRendererShaderType_::PlatformBackendRendererShaderType_Vertex))
            shader = glCreateShader(GL_VERTEX_SHADER);
        else if((shaderType & PlatformBackendRendererShaderType_::PlatformBackendRendererShaderType_Fragment))
            shader = glCreateShader(GL_FRAGMENT_SHADER);

        int status = 1;
        const char* shaderSourceCodePtr = shaderSourceCode.c_str();
        glShaderSource(shader, 1, &shaderSourceCodePtr, nullptr);
        glCompileShader(shader);
        glGetShaderiv(shader, GL_COMPILE_STATUS, &status);

        if(status)
        {
            glAttachShader(shaderProgram, shader);
        }
        else
        {
            // TODO: add status message generation logic here
        }
    }

    int status = true;
    glLinkProgram(shaderProgram);
    glGetProgramiv(shaderProgram, GL_LINK_STATUS, &status);

    if(!status)
    {
        // TODO: add status message generation logic here
    }

    return status ? m_Shaders->construct(shaderProgram) : nullptr;
}

PlatformBackendRendererShader* PlatformBackendRenderer::construct_shader(
    const std::vector<std::filesystem::path>& _ShaderPaths)
{
    std::vector<std::pair<
        std::string,
        PlatformBackendRendererShaderType>> shaderInfos;

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
                    PlatformBackendRendererShaderType_::PlatformBackendRendererShaderType_Vertex
                }
            );
        }
        else if(extention == ".frag" || extention == ".fsh" || extention == ".fs")
        {
            shaderInfos.push_back(
                {
                    source,
                    PlatformBackendRendererShaderType_::PlatformBackendRendererShaderType_Vertex
                }
            );
        }
    }

    return construct_shader(shaderInfos);
}

void PlatformBackendRenderer::destroy_shader(PlatformBackendRendererShader* _Shader)
{
    if(_Shader == nullptr)
        return;

    // deallocate texture on GPU
    glDeleteProgram(_Shader->Ptr);

    // destroy texture data on CPU
    m_Shaders->destroy(_Shader);
}

void PlatformBackendRenderer::set_shader_uniform(PlatformBackendRendererShader* _Shader, const std::string& _Name, const bool& _Value)
{
    glUniform1i(glGetUniformLocation(_Shader->Ptr, _Name.c_str()), (int)_Value);
}

void PlatformBackendRenderer::set_shader_uniform(PlatformBackendRendererShader* _Shader, const std::string& _Name, const int& _Value)
{
    glUniform1i(glGetUniformLocation(_Shader->Ptr, _Name.c_str()), _Value);
}

void PlatformBackendRenderer::set_shader_uniform(PlatformBackendRendererShader* _Shader, const std::string& _Name, const float& _Value)
{
    glUniform1f(glGetUniformLocation(_Shader->Ptr, _Name.c_str()), _Value);
}

void PlatformBackendRenderer::set_shader_uniform(PlatformBackendRendererShader* _Shader, const std::string& _Name, const glm::vec2& _Value)
{
    glUniform2fv(glGetUniformLocation(_Shader->Ptr, _Name.c_str()), 1, &_Value[0]);
}

void PlatformBackendRenderer::set_shader_uniform(PlatformBackendRendererShader* _Shader, const std::string& _Name, const glm::vec3& _Value)
{
    glUniform3fv(glGetUniformLocation(_Shader->Ptr, _Name.c_str()), 1, &_Value[0]);
}

void PlatformBackendRenderer::set_shader_uniform(PlatformBackendRendererShader* _Shader, const std::string& _Name, const glm::vec4& _Value)
{
    glUniform4fv(glGetUniformLocation(_Shader->Ptr, _Name.c_str()), 1, &_Value[0]);
}

void PlatformBackendRenderer::set_shader_uniform(PlatformBackendRendererShader* _Shader, const std::string& _Name, const glm::mat2& _Value)
{
    glUniformMatrix2fv(glGetUniformLocation(_Shader->Ptr, _Name.c_str()), 1, GL_FALSE, &_Value[0][0]);
}

void PlatformBackendRenderer::set_shader_uniform(PlatformBackendRendererShader* _Shader, const std::string& _Name, const glm::mat3& _Value)
{
    glUniformMatrix3fv(glGetUniformLocation(_Shader->Ptr, _Name.c_str()), 1, GL_FALSE, &_Value[0][0]);
}

void PlatformBackendRenderer::set_shader_uniform(PlatformBackendRendererShader* _Shader, const std::string& _Name, const glm::mat4& _Value)
{
    glUniformMatrix4fv(glGetUniformLocation(_Shader->Ptr, _Name.c_str()), 1, GL_FALSE, &_Value[0][0]);
}