// Application
#include <FrenchieApplicationPlatformBackend.hpp>
#include <FrenchieApplicationRenderingBackend.hpp>
#include <FrenchieApplicationRenderingBackendDefaultFont.hpp>

// GLAD
#include <glad/glad.h>

#include <iostream>

using namespace Frenchie::Application;

namespace Frenchie
{
    namespace Application
    {
        struct ApplicationRenderingBackendOpenGL : public ApplicationRenderingBackendGraphicsApi
        {
            ApplicationRenderingBackendOpenGL(){}
            virtual ~ApplicationRenderingBackendOpenGL(){}

            mutable unsigned int m_VBO   {0};
            mutable unsigned int m_VAO   {0};
            mutable unsigned int m_EBO   {0};
            mutable unsigned int m_Shader{0};
            mutable bool         m_Ready {false};
        };
    }
}

bool ApplicationRenderingBackend::awake(const std::any& _Stuff)
{
    // auxiliary lambdas
    auto construct_shader = [](const std::vector<std::pair<std::string, ApplicationRenderingBackendShaderType>>& _ShaderInfos)->unsigned int
    {
        unsigned int shaderProgram = glCreateProgram();

        for(auto&& shaderInfo : _ShaderInfos)
        {
            auto         shaderSourceCode = shaderInfo.first;
            auto         shaderType       = shaderInfo.second;
            unsigned int shader           = 0;

            if(shaderType == ApplicationRenderingBackendShaderType_::ApplicationRenderingBackendShaderType_Vertex)
                shader = glCreateShader(GL_VERTEX_SHADER);
            else if(shaderType == ApplicationRenderingBackendShaderType_::ApplicationRenderingBackendShaderType_Fragment)
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

            return 0;
        }

        return shaderProgram;
    };

    // load backend API
    try
    {
        if(!gladLoadGLLoader((GLADloadproc)std::any_cast<void*(*)(const char*)>(_Stuff)))
            return false;
    }
    catch(...)
    {
        return false;
    }

    m_Api = std::make_shared<ApplicationRenderingBackendOpenGL>();

    // create openGL backend handles
    glGenBuffers(1, &graphics_api<ApplicationRenderingBackendOpenGL>()->m_VBO);
    glGenBuffers(1, &graphics_api<ApplicationRenderingBackendOpenGL>()->m_EBO);
    glGenVertexArrays(1, &graphics_api<ApplicationRenderingBackendOpenGL>()->m_VAO);

    graphics_api<ApplicationRenderingBackendOpenGL>()->m_Shader = construct_shader(
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
layout (location = 3) in vec4 a_Color;

// outputs
out vec3 Normal;
out vec2 UV;
out vec4 Color;

// uniforms
uniform mat4 u_ModelMatrix;

void main()
{
    // setup position
    gl_Position = u_ModelMatrix * vec4(a_Position, 1.0);

    // setup outputs
    Normal = a_Normal;
    UV     = a_UV;
    Color  = a_Color;
}            
)"),
                ApplicationRenderingBackendShaderType_::ApplicationRenderingBackendShaderType_Vertex
            },

            // fragment shader
            {
                std::string(
R"(
#version 330 core

// inputs
in vec2 UV;
in vec4 Color;

//outputs
out vec4 fragColor;

//uniforms
uniform sampler2D u_Texture;

void main()
{
    fragColor = Color * texture(u_Texture, UV);
}
)"),
                ApplicationRenderingBackendShaderType_::ApplicationRenderingBackendShaderType_Fragment
                },
        }
    );    

    // create default font
    graphics_api<ApplicationRenderingBackendOpenGL>()->m_DefaultFont = construct_font(
        ApplicationRenderingBackendDefaultFont::BUFFER,
        ApplicationRenderingBackendDefaultFont::COMPRESSED_SIZE,
        128);

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

    graphics_api<ApplicationRenderingBackendOpenGL>()->m_DefaultTexture = ApplicationRenderingBackend::construct_texture(image, width, height);

    return true;
}

void ApplicationRenderingBackend::quit()
{
    // destroy default font ant texture
    destroy_font(graphics_api<ApplicationRenderingBackendOpenGL>()->m_DefaultFont);
    destroy_texture(graphics_api<ApplicationRenderingBackendOpenGL>()->m_DefaultTexture);

    // destroy OpenGL state
    if(m_Api == nullptr) return;

    glDeleteBuffers(1, &graphics_api<ApplicationRenderingBackendOpenGL>()->m_VBO);
    glDeleteBuffers(1, &graphics_api<ApplicationRenderingBackendOpenGL>()->m_EBO);
    glDeleteVertexArrays(1, &graphics_api<ApplicationRenderingBackendOpenGL>()->m_VAO);
    glDeleteProgram(graphics_api<ApplicationRenderingBackendOpenGL>()->m_Shader);
}

void ApplicationRenderingBackend::set_viewport(const gs_vec2f& _Position, const gs_vec2f& _Size)
{
    glViewport((int)_Position.x, (int)_Position.y, (int)_Size.x, (int)_Size.y);
}

ApplicationRenderingBackendTexture ApplicationRenderingBackend::construct_texture(
    const unsigned char*                               _RawBuffer,
    const int&                                         _Width,
    const int&                                         _Height,
    const ApplicationRenderingBackendTextureFormat&    _Format,
    const ApplicationRenderingBackendTextureWrapMode&  _Wrap,
    const ApplicationRenderingBackendTextureMinFilter& _MinFilter,
    const ApplicationRenderingBackendTextureMaxFilter& _MaxFilter)
{
    if(_RawBuffer == nullptr)
        return ApplicationRenderingBackendTexture();

    // register image within platform specific low level grphics API
    //glPixelStorei(GL_UNPACK_ALIGNMENT, 1);
    
    unsigned int sampler;
    glGenTextures(1, &sampler);
    glBindTexture(GL_TEXTURE_2D, sampler); 
    
    // set format
    switch (_Format)
    {
    case ApplicationRenderingBackendTextureFormat_::ApplicationRenderingBackendTextureFormat_ALPHA:
        glTexImage2D(GL_TEXTURE_2D, 0, GL_R8, _Width, _Height, 0, GL_RED, GL_UNSIGNED_BYTE, _RawBuffer);
        break;
    
    case ApplicationRenderingBackendTextureFormat_::ApplicationRenderingBackendTextureFormat_RGB:
        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, _Width, _Height, 0, GL_RGB, GL_UNSIGNED_BYTE, _RawBuffer);
        break;

    case ApplicationRenderingBackendTextureFormat_::ApplicationRenderingBackendTextureFormat_RGBA:
        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, _Width, _Height, 0, GL_RGBA, GL_UNSIGNED_BYTE, _RawBuffer);
        break;

    default:
        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, _Width, _Height, 0, GL_RGBA, GL_UNSIGNED_BYTE, _RawBuffer);
        break;
    }

    // set the texture wrapping parameters
    switch (_Wrap)
    {
    case ApplicationRenderingBackendTextureWrapMode_::ApplicationRenderingBackendTextureWrapMode_Repeat:
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
        break;
    
    case ApplicationRenderingBackendTextureWrapMode_::ApplicationRenderingBackendTextureWrapMode_Mirrored:
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_MIRRORED_REPEAT);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_MIRRORED_REPEAT);
        break;

    case ApplicationRenderingBackendTextureWrapMode_::ApplicationRenderingBackendTextureWrapMode_ClampToEdge:
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
        break;

    case ApplicationRenderingBackendTextureWrapMode_::ApplicationRenderingBackendTextureWrapMode_ClampToBorder:
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
    case ApplicationRenderingBackendTextureMinFilter_::ApplicationRenderingBackendTextureMinFilter_Linear:
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
        break;
    
    case ApplicationRenderingBackendTextureMinFilter_::ApplicationRenderingBackendTextureMinFilter_Nearest:
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
        break;

    case ApplicationRenderingBackendTextureMinFilter_::ApplicationRenderingBackendTextureMinFilter_LinearMipMapLinear:
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
        break;

    case ApplicationRenderingBackendTextureMinFilter_::ApplicationRenderingBackendTextureMinFilter_LinearMipMapNearest:
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_NEAREST);
        break;

    case ApplicationRenderingBackendTextureMinFilter_::ApplicationRenderingBackendTextureMinFilter_NearestMipMapLinear:
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST_MIPMAP_LINEAR);
        break;

    case ApplicationRenderingBackendTextureMinFilter_::ApplicationRenderingBackendTextureMinFilter_NearestMipMapNearest:
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST_MIPMAP_NEAREST);
        break;

    default:
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
        break;
    }

    // set magnifying filter
    switch (_MaxFilter)
    {
    case ApplicationRenderingBackendTextureMaxFilter_::ApplicationRenderingBackendTextureMaxFilter_Linear:
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
        break;
    
    case ApplicationRenderingBackendTextureMaxFilter_::ApplicationRenderingBackendTextureMaxFilter_Nearest:
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

    return ApplicationRenderingBackendTexture(sampler, _Width, _Height, 1, _Format, _Wrap, _MinFilter, _MaxFilter);
}

void ApplicationRenderingBackend::destroy_texture(const ApplicationRenderingBackendTexture& _Texture)
{
    glDeleteTextures(1, &_Texture.Ptr);
}

bool ApplicationRenderingBackend::begin_render()
{
    if(m_Api == nullptr) return false;

    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    glEnable(GL_DEPTH_TEST);
    glEnable(GL_STENCIL_TEST);

    glClear(GL_COLOR_BUFFER_BIT);
    glClear(GL_DEPTH_BUFFER_BIT);
    glClear(GL_STENCIL_BUFFER_BIT);

    // mark we are not ready yet
    graphics_api<ApplicationRenderingBackendOpenGL>()->m_Ready = false;
    
    // check that everything has been instantiated
    return graphics_api<ApplicationRenderingBackendOpenGL>()->m_VBO &&
           graphics_api<ApplicationRenderingBackendOpenGL>()->m_EBO &&
           graphics_api<ApplicationRenderingBackendOpenGL>()->m_VAO &&
           graphics_api<ApplicationRenderingBackendOpenGL>()->m_Shader;
}

void ApplicationRenderingBackend::render_mesh(
    const ApplicationRenderingBackendMeshVertex*                    _Vertexes,
    const ApplicationRenderingBackendMeshVertexIndex&           _VertexesCount,
    const ApplicationRenderingBackendMeshVertexIndex&           _MeshVertexesCount,
    const ApplicationRenderingBackendMeshVertexIndex&           _MeshVertexesOffset,
    const ApplicationRenderingBackendMeshVertexIndex*           _Indexes,
    const ApplicationRenderingBackendMeshVertexIndex&           _IndexesCount,
    const ApplicationRenderingBackendMeshVertexIndex&           _MeshIndexesCount,
    const ApplicationRenderingBackendMeshVertexIndex&           _MeshIndexesOffset,
    const ApplicationRenderingBackendTexture&                   _Texture,
    const gs_mat4f&                                             _MeshProjectionMatrix,
    const ApplicationRenderingBackendGraphicsApiRenderingHints& _MeshRenderHints)
{
    if(m_Api == nullptr || _Vertexes == nullptr || _Indexes == nullptr || _VertexesCount <= 0 || _IndexesCount <= 0 || _MeshVertexesCount <= 0 || _MeshIndexesCount <= 0)
        return;

    // bind texture
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, _Texture.Ptr);

    // load mesh and bind shader
    if(!graphics_api<ApplicationRenderingBackendOpenGL>()->m_Ready)
    {
        // bind shader
        glUseProgram(graphics_api<ApplicationRenderingBackendOpenGL>()->m_Shader);

        // load mesh

        // bind VAO to remember VBO/EBO configuration and layout
        glBindVertexArray(graphics_api<ApplicationRenderingBackendOpenGL>()->m_VAO);

        // load vertexes and indexes on GPU
        glBindBuffer(GL_ARRAY_BUFFER, graphics_api<ApplicationRenderingBackendOpenGL>()->m_VBO);
        glBufferData(GL_ARRAY_BUFFER, _VertexesCount * sizeof(ApplicationRenderingBackendMeshVertex), _Vertexes, GL_DYNAMIC_DRAW);

        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, graphics_api<ApplicationRenderingBackendOpenGL>()->m_EBO);
        glBufferData(GL_ELEMENT_ARRAY_BUFFER, _IndexesCount * sizeof(ApplicationRenderingBackendMeshVertexIndex), _Indexes, GL_DYNAMIC_DRAW);

        // setup attributes pointers
        glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(ApplicationRenderingBackendMeshVertex), (void*)(GS_OFFSET_OF(ApplicationRenderingBackendMeshVertex, Position)));
        glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, sizeof(ApplicationRenderingBackendMeshVertex), (void*)(GS_OFFSET_OF(ApplicationRenderingBackendMeshVertex, Normal)));
        glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, sizeof(ApplicationRenderingBackendMeshVertex), (void*)(GS_OFFSET_OF(ApplicationRenderingBackendMeshVertex, UV)));
        glVertexAttribPointer(3, 4, GL_UNSIGNED_BYTE, GL_TRUE, sizeof(ApplicationRenderingBackendMeshVertex), (void*)(GS_OFFSET_OF(ApplicationRenderingBackendMeshVertex, Color)));
        glEnableVertexAttribArray(0);
        glEnableVertexAttribArray(1);
        glEnableVertexAttribArray(2);
        glEnableVertexAttribArray(3);

        graphics_api<ApplicationRenderingBackendOpenGL>()->m_Ready = true;
    }

    glBindVertexArray(graphics_api<ApplicationRenderingBackendOpenGL>()->m_VAO);

    // configure shader
    glUniformMatrix4fv(glGetUniformLocation(graphics_api<ApplicationRenderingBackendOpenGL>()->m_Shader, "u_ModelMatrix"), 1, GL_FALSE, &_MeshProjectionMatrix[0][0]);
    glUniform1i(glGetUniformLocation(graphics_api<ApplicationRenderingBackendOpenGL>()->m_Shader, "u_Texture"), 0);

    // render mesh
    if((_MeshRenderHints & ApplicationRenderingBackendGraphicsApiRenderingHints_::ApplicationRenderingBackendGraphicsApiRenderingHints_Lines))
        glDrawElements(GL_LINE_LOOP, _MeshIndexesCount - _MeshIndexesOffset, GL_UNSIGNED_INT, (void*)(intptr_t)(_MeshIndexesOffset * sizeof(ApplicationRenderingBackendMeshVertexIndex)));

    if((_MeshRenderHints & ApplicationRenderingBackendGraphicsApiRenderingHints_::ApplicationRenderingBackendGraphicsApiRenderingHints_Triangles))
        glDrawElements(GL_TRIANGLES, _MeshIndexesCount - _MeshIndexesOffset, GL_UNSIGNED_INT, (void*)(intptr_t)(_MeshIndexesOffset * sizeof(ApplicationRenderingBackendMeshVertexIndex)));

    // unbind everything
    glBindTexture(GL_TEXTURE_2D, 0);
}

void ApplicationRenderingBackend::end_render()
{
    if(m_Api == nullptr) return;

    // disable all tests
    glDisable(GL_BLEND);
    glDisable(GL_DEPTH_TEST);
    glDisable(GL_STENCIL_TEST);
    glDisable(GL_SCISSOR_TEST);

    // unbind everything
    glBindVertexArray(0);
    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
    glUseProgram(0);

    // mark we are not ready yet
    graphics_api<ApplicationRenderingBackendOpenGL>()->m_Ready = false;
}

void ApplicationRenderingBackend::clear_color(const gs_color& _Color)
{
    glClearColor(
        (float)gs_color_rgba_get_r(_Color) / 255.f,
        (float)gs_color_rgba_get_g(_Color) / 255.f,
        (float)gs_color_rgba_get_b(_Color) / 255.f,
        (float)gs_color_rgba_get_a(_Color) / 255.f);
}

void ApplicationRenderingBackend::scissor_box(const gs_2dboxf& _ClippingRect)
{
    glEnable(GL_SCISSOR_TEST);

    glScissor(
        (int)_ClippingRect.Min.x,
        (int)(ApplicationPlatformBackend::get_window_framebuffer_size().y - _ClippingRect.Max.y),
        (int)_ClippingRect.width(),
        (int)_ClippingRect.height());
}

// camera and view projection API
ApplicationRenderingBackend::Projections ApplicationRenderingBackend::calculate_2d_camera_view_and_projection(
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

gs_vec2f ApplicationRenderingBackend::convert_to_NDC(const gs_vec2f& _Position, const gs_vec2f& _Screen)
{
    return gs_vec2f((2.0f * _Position.x) / _Screen.x - 1.0f, 1.0f - (2.0f * _Position.y) / _Screen.y);
}