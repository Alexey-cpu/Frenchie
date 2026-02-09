#include <FrenchieApplicationLayerRenderingQueue.hpp>

// Application
#include <FrenchieApplication.hpp>

// GLAD
#include <glad/glad.h>

using namespace Frenchie::Application;

bool RenderingQueueGraphicsApi::load(Loader _Loader)
{
    return gladLoadGLLoader((GLADloadproc)_Loader);
}

void RenderingQueueGraphicsApi::set_viewport(const gs_vec2f& _Position, const gs_vec2f& _Size)
{
    glViewport((int)_Position.x, (int)_Position.y, (int)_Size.x, (int)_Size.y);
}

RenderingQueueTexture RenderingQueueGraphicsApi::construct_texture(
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

    return RenderingQueueTexture(sampler, _Width, _Height, 1, _Format, _Wrap, _MinFilter, _MaxFilter);
}

void RenderingQueueGraphicsApi::begin_use_texture(const RenderingQueueTexture& _Texture)
{
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, _Texture.Ptr);
}

void RenderingQueueGraphicsApi::end_use_texture()
{
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, 0);
}

void RenderingQueueGraphicsApi::destroy_texture(const RenderingQueueTexture& _Texture)
{
    glDeleteTextures(1, &_Texture.Ptr);
}

RenderingQueueShader RenderingQueueGraphicsApi::construct_shader(
    const std::vector<std::pair<std::string, RenderingQueueShaderType>>& _ShaderInfos)
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

void RenderingQueueGraphicsApi::begin_use_shader(const RenderingQueueShader& _Shader)
{
     glUseProgram(_Shader.Ptr);
}

void RenderingQueueGraphicsApi::set_shader_uniform(const RenderingQueueShader& _Shader, const char* _Name, const bool& _Value)
{
    glUniform1i(glGetUniformLocation(_Shader.Ptr, _Name), (int)_Value);
}

void RenderingQueueGraphicsApi::set_shader_uniform(const RenderingQueueShader& _Shader, const char* _Name, const int& _Value)
{
    glUniform1i(glGetUniformLocation(_Shader.Ptr, _Name), _Value);
}

void RenderingQueueGraphicsApi::set_shader_uniform(const RenderingQueueShader& _Shader, const char* _Name, const float& _Value)
{
    glUniform1f(glGetUniformLocation(_Shader.Ptr, _Name), _Value);
}

void RenderingQueueGraphicsApi::set_shader_uniform(const RenderingQueueShader& _Shader, const char* _Name, const gs_vec2f& _Value)
{
    glUniform2fv(glGetUniformLocation(_Shader.Ptr, _Name), 1, &_Value[0]);
}

void RenderingQueueGraphicsApi::set_shader_uniform(const RenderingQueueShader& _Shader, const char* _Name, const gs_vec3f& _Value)
{
    glUniform3fv(glGetUniformLocation(_Shader.Ptr, _Name), 1, &_Value[0]);
}

void RenderingQueueGraphicsApi::set_shader_uniform(const RenderingQueueShader& _Shader, const char* _Name, const gs_vec4f& _Value)
{
    glUniform4fv(glGetUniformLocation(_Shader.Ptr, _Name), 1, &_Value[0]);
}

void RenderingQueueGraphicsApi::set_shader_uniform(const RenderingQueueShader& _Shader, const char* _Name, const gs_mat2f& _Value)
{
    glUniformMatrix2fv(glGetUniformLocation(_Shader.Ptr, _Name), 1, GL_FALSE, &_Value[0][0]);
}

void RenderingQueueGraphicsApi::set_shader_uniform(const RenderingQueueShader& _Shader, const char* _Name, const gs_mat3f& _Value)
{
    glUniformMatrix3fv(glGetUniformLocation(_Shader.Ptr, _Name), 1, GL_FALSE, &_Value[0][0]);
}

void RenderingQueueGraphicsApi::set_shader_uniform(const RenderingQueueShader& _Shader, const char* _Name, const gs_mat4f& _Value)
{
    glUniformMatrix4fv(glGetUniformLocation(_Shader.Ptr, _Name), 1, GL_FALSE, &_Value[0][0]);
}

void RenderingQueueGraphicsApi::end_use_shader()
{
    glUseProgram(0);
}

void RenderingQueueGraphicsApi::destroy_shader(const RenderingQueueShader& _Shader)
{
    glDeleteProgram(_Shader.Ptr);
}

RenderingQueueMesh RenderingQueueGraphicsApi::construct_mesh(
    const RenderingQueueVertex* _Vertexes,
    const int&                  _VertexesCount,
    const int*                  _Indexes,
    const int&                  _IndexesCount)
{
    if(_Vertexes == nullptr || _Indexes == nullptr || _VertexesCount <= 0 || _IndexesCount <= 0)
        return RenderingQueueMesh();

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
    glVertexAttribPointer(3, 4, GL_UNSIGNED_BYTE, GL_TRUE, sizeof(RenderingQueueVertex), (void*)(offsetof(RenderingQueueVertex, Color)));
    glEnableVertexAttribArray(0);
    glEnableVertexAttribArray(1);
    glEnableVertexAttribArray(2);
    glEnableVertexAttribArray(3);

    glBindVertexArray(0);

    return RenderingQueueMesh(m_VBO, m_VAO, m_EBO);
}

void RenderingQueueGraphicsApi::begin_use_mesh(
    const RenderingQueueMesh&               _Mesh,
    const RenderingQueueGraphicsApiRenderingHints& _MeshRenderHints)
{
    // bind VAO containing VBO, EBO
    glBindVertexArray(_Mesh.VAO);

    // get EBO size
    int bufferSize; 
    glGetBufferParameteriv(GL_ELEMENT_ARRAY_BUFFER, GL_BUFFER_SIZE, &bufferSize);

    // draw EBO
    if((_MeshRenderHints & RenderingQueueGraphicsApiRenderingHints_::RenderingQueueGraphicsApiRenderingHints_Points))
        glDrawArrays(GL_POINTS, 0, bufferSize);

    if((_MeshRenderHints & RenderingQueueGraphicsApiRenderingHints_::RenderingQueueGraphicsApiRenderingHints_Lines))
        glDrawElements(GL_LINE_LOOP, bufferSize, GL_UNSIGNED_INT, 0);

    if((_MeshRenderHints & RenderingQueueGraphicsApiRenderingHints_::RenderingQueueGraphicsApiRenderingHints_Triangles))
        glDrawElements(GL_TRIANGLES, bufferSize, GL_UNSIGNED_INT, 0);
}

void RenderingQueueGraphicsApi::end_use_mesh()
{
    glBindVertexArray(0);
}

void RenderingQueueGraphicsApi::destroy_mesh(const RenderingQueueMesh& _Mesh)
{
    glDeleteBuffers(1, &_Mesh.VBO);
    glDeleteBuffers(1, &_Mesh.EBO);
    glDeleteVertexArrays(1, &_Mesh.VAO);
}

void RenderingQueueGraphicsApi::enable(const RenderingQueueGraphicsApiHints& _Hints)
{
    if(_Hints & RenderingQueueGraphicsApiHints_::RenderingQueueGraphicsApiHints_Blending)
    {
        glEnable(GL_BLEND);
        glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    }

    if(_Hints & RenderingQueueGraphicsApiHints_::RenderingQueueGraphicsApiHints_DepthTest)
        glEnable(GL_DEPTH_TEST);

    if(_Hints & RenderingQueueGraphicsApiHints_::RenderingQueueGraphicsApiHints_StencilTest)
        glEnable(GL_STENCIL_TEST);

    if(_Hints & RenderingQueueGraphicsApiHints_::RenderingQueueGraphicsApiHints_ScissorTest)
        glEnable(GL_SCISSOR_TEST);
}

void RenderingQueueGraphicsApi::disable(const RenderingQueueGraphicsApiHints& _Hints)
{
    if(_Hints & RenderingQueueGraphicsApiHints_::RenderingQueueGraphicsApiHints_Blending)
        glDisable(GL_BLEND);

    if(_Hints & RenderingQueueGraphicsApiHints_::RenderingQueueGraphicsApiHints_DepthTest)
        glDisable(GL_DEPTH_TEST);

    if(_Hints & RenderingQueueGraphicsApiHints_::RenderingQueueGraphicsApiHints_StencilTest)
        glDisable(GL_STENCIL_TEST);

    if(_Hints & RenderingQueueGraphicsApiHints_::RenderingQueueGraphicsApiHints_ScissorTest)
        glDisable(GL_SCISSOR_TEST);
}

void RenderingQueueGraphicsApi::clear_color(const RenderingQueueColor& _Color)
{
    glClearColor(
        (float)RenderingQueueGraphicsApi::retrieve_red_component(_Color)   / 255.f,
        (float)RenderingQueueGraphicsApi::retrieve_green_component(_Color) / 255.f,
        (float)RenderingQueueGraphicsApi::retrieve_blue_component(_Color)  / 255.f,
        (float)RenderingQueueGraphicsApi::retrieve_alpha_component(_Color) / 255.f);
}

void RenderingQueueGraphicsApi::scissor_box(const gs_2dboxf& _ClippingRect)
{
    RenderingQueueGraphicsApi::enable(RenderingQueueGraphicsApiHints_::RenderingQueueGraphicsApiHints_ScissorTest);

    glScissor(
        (int)_ClippingRect.Min.x,
        (int)(application()->get_window_framebuffer_size().y - _ClippingRect.Max.y),
        (int)_ClippingRect.width(),
        (int)_ClippingRect.height());
}

void RenderingQueueGraphicsApi::clear_buffers(const RenderingQueueGraphicsApiBuffers& _Buffers)
{
    if(_Buffers & RenderingQueueGraphicsApiBuffers_::RenderingQueueGraphicsApiBuffers_Color)
        glClear(GL_COLOR_BUFFER_BIT);

    if(_Buffers & RenderingQueueGraphicsApiBuffers_::RenderingQueueGraphicsApiBuffers_Depth)
        glClear(GL_DEPTH_BUFFER_BIT);

    if(_Buffers & RenderingQueueGraphicsApiBuffers_::RenderingQueueGraphicsApiBuffers_Stencil)
        glClear(GL_STENCIL_BUFFER_BIT);
}

// color API
RenderingQueueColor RenderingQueueGraphicsApi::construct_rgba_color(
    const RenderingQueueColor& _R,
    const RenderingQueueColor& _G,
    const RenderingQueueColor& _B,
    const RenderingQueueColor& _A)
{
    return (((RenderingQueueColor)(_A)<<24) |
            ((RenderingQueueColor)(_B)<<16) |
            ((RenderingQueueColor)(_G)<<8)  |
            ((RenderingQueueColor)(_R)<<0));
}

RenderingQueueColor RenderingQueueGraphicsApi::retrieve_red_component(const RenderingQueueColor& _Color)
{
    return (_Color >> 0) & 0xFF;
}

RenderingQueueColor RenderingQueueGraphicsApi::retrieve_green_component(const RenderingQueueColor& _Color)
{
    return (_Color >> 8) & 0xFF;
}

RenderingQueueColor RenderingQueueGraphicsApi::retrieve_blue_component(const RenderingQueueColor& _Color)
{
    return (_Color >> 16) & 0xFF;
}

RenderingQueueColor RenderingQueueGraphicsApi::retrieve_alpha_component(const RenderingQueueColor& _Color)
{
    return (_Color >> 24) & 0xFF;
}

// camera and view projection API
RenderingQueueGraphicsApi::Projections RenderingQueueGraphicsApi::calculate_2d_camera_view_and_projection(
    const gs_vec2f& _CameraWorldPosition,
    const gs_vec3f& _CameraWorldUpAxisDirection,
    const gs_vec3f& _CameraWorldFrontAxisDirection,
    const gs_vec2f& _CameraResolution,
    const float&    _CameraRotationAngle,
    const float&    _CameraNearPlanePosition,
    const float&    _CameraFarPlanePosition)
{
    // compute projection matrix
    float left   = -_CameraResolution.x * 0.5f + _CameraWorldPosition.x;
    float right  = +_CameraResolution.x * 0.5f + _CameraWorldPosition.x;
    float bottom = +_CameraResolution.y * 0.5f + _CameraWorldPosition.y;
    float top    = -_CameraResolution.y * 0.5f + _CameraWorldPosition.y;

    // camera orientation
    gs_vec3f cameraLocalFrontAxisDirection = gs_vector_normalize(_CameraWorldFrontAxisDirection);
    gs_vec3f cameraLocalRightAxisDirection = gs_vector_normalize(gs_vector_cross(cameraLocalFrontAxisDirection, _CameraWorldUpAxisDirection));
    gs_vec3f cameraLocalUpAxisDirection    = gs_vector_normalize(gs_vector_cross(cameraLocalRightAxisDirection, cameraLocalFrontAxisDirection));

    gs_mat4f cameraview =
        gs_matrix_look_at(
            gs_vec3f(0.f, 0.f, 1),
            gs_vec3f(0.f, 0.f, 1) + cameraLocalFrontAxisDirection, cameraLocalUpAxisDirection);
    
    gs_mat4f projection =
        gs_matrix_ortho(
            left,
            right,
            bottom,
            top,
            _CameraNearPlanePosition,
            _CameraFarPlanePosition) * gs_matrix_rotate(gs_mat4f(1.f), gs_to_radians(_CameraRotationAngle), gs_vec3f(0.f, 0.f, 1.f));

    return {cameraview, projection};
}