// Application
#include <FrenchieApplicationPlatformBackend.hpp>
#include <FrenchieApplicationRenderingBackend.hpp>

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

            // default rendering pipeline properties
            mutable unsigned int                                  m_VBO                     {0};
            mutable unsigned int                                  m_VAO                     {0};
            mutable unsigned int                                  m_EBO                     {0};
            mutable unsigned int                                  m_Shader                  {0};
            mutable ApplicationRenderingBackendMeshRenderingHints m_MeshRenderingHints      {ApplicationRenderingBackendMeshRenderingHints_::ApplicationRenderingBackendMeshRenderingHints_Triangles};

            // framebuffer rendering pipeline properties
            mutable unsigned int                                  m_FrameBufferTexture      {0};
            mutable unsigned int                                  m_FrameBufferTextureWidth {800};
            mutable unsigned int                                  m_FrameBufferTextureHeight{600};
            mutable unsigned int                                  m_FBO                     {0};
            mutable unsigned int                                  m_RBO                     {0};
            mutable ApplicationRenderingBackendRenderingTarget*   m_RenderingTarget         {nullptr};
        };

        enum ApplicationRenderingBackendShaderType_ : int
        {
            ApplicationRenderingBackendShaderType_Vertex,
            ApplicationRenderingBackendShaderType_Fragment
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

    std::shared_ptr<ApplicationRenderingBackendOpenGL> OpenGL3 =
        std::dynamic_pointer_cast<ApplicationRenderingBackendOpenGL>((m_Api = std::make_shared<ApplicationRenderingBackendOpenGL>()));

    // create VBO, EBO, VAO
    glGenBuffers(1, &OpenGL3->m_VBO);
    glGenBuffers(1, &OpenGL3->m_EBO);
    glGenVertexArrays(1, &OpenGL3->m_VAO);

    // compile shaders
    OpenGL3->m_Shader = construct_shader(
        {
            // Vertex shader
            {
                std::string(
R"(
#version 330 core

// vertex attributes
layout (location = 0) in vec3 a_Position;
layout (location = 1) in vec2 a_UV;
layout (location = 2) in vec4 a_Color;

// outputs
out vec2 UV;
out vec4 Color;

// uniforms
uniform mat4 u_ModelMatrix;

void main()
{
    // setup position
    gl_Position = u_ModelMatrix * vec4(a_Position, 1.0);

    // setup outputs
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

    // create frame buffer
    {
        glGenFramebuffers(1, &OpenGL3->m_FBO);
        glBindFramebuffer(GL_FRAMEBUFFER, OpenGL3->m_FBO);

        glGenTextures(1, &OpenGL3->m_FrameBufferTexture);
        glBindTexture(GL_TEXTURE_2D, OpenGL3->m_FrameBufferTexture);
        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, OpenGL3->m_FrameBufferTextureWidth, OpenGL3->m_FrameBufferTextureHeight, 0, GL_RGBA, GL_UNSIGNED_BYTE, NULL);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
        glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, OpenGL3->m_FrameBufferTexture, 0);

        glGenRenderbuffers(1, &OpenGL3->m_RBO);
        glBindRenderbuffer(GL_RENDERBUFFER, OpenGL3->m_RBO);
        glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH24_STENCIL8, OpenGL3->m_FrameBufferTextureWidth, OpenGL3->m_FrameBufferTextureHeight);
        glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_STENCIL_ATTACHMENT, GL_RENDERBUFFER, OpenGL3->m_RBO);

        if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE)
        {
            std::cout << "ERROR::FRAMEBUFFER:: Framebuffer is not complete!\n";
            return false;
        }

        glBindFramebuffer(GL_FRAMEBUFFER, 0);
        glBindTexture(GL_TEXTURE_2D, 0);
        glBindRenderbuffer(GL_RENDERBUFFER, 0);
    }

    // setup mesh rendering hints
    OpenGL3->m_MeshRenderingHints = ApplicationRenderingBackendMeshRenderingHints_::ApplicationRenderingBackendMeshRenderingHints_Triangles;

    return true;
}

void ApplicationRenderingBackend::quit()
{
    std::shared_ptr<ApplicationRenderingBackendOpenGL> OpenGL3 = graphics_api<ApplicationRenderingBackendOpenGL>();

    if(OpenGL3 == nullptr)
        return;

    // destroy defaults
    if(OpenGL3->m_DefaultFont.has_value())
        destroy_font(OpenGL3->m_DefaultFont.value());
    if(OpenGL3->m_DefaultTexture.has_value())
        destroy_texture(OpenGL3->m_DefaultTexture.value());
    
    // delete OpenGL handles
    glDeleteBuffers(1, &OpenGL3->m_VBO);
    glDeleteBuffers(1, &OpenGL3->m_EBO);
    glDeleteVertexArrays(1, &OpenGL3->m_VAO);
    glDeleteProgram(OpenGL3->m_Shader);

    // delete framebuffer
    glDeleteRenderbuffers(1, &OpenGL3->m_RBO);
    glDeleteFramebuffers(1, &OpenGL3->m_FBO);
    glDeleteTextures(1, &OpenGL3->m_FrameBufferTexture);
}

void ApplicationRenderingBackend::set_viewport(const gs_vec2f& _Position, const gs_vec2f& _Size)
{
    glViewport((int)_Position.x, (int)_Position.y, (int)_Size.x, (int)_Size.y);
}

void ApplicationRenderingBackend::begin_render(ApplicationRenderingBackendRenderingTarget* _Target)
{
    std::shared_ptr<ApplicationRenderingBackendOpenGL> OpenGL3 = graphics_api<ApplicationRenderingBackendOpenGL>();

    if(OpenGL3 == nullptr)
        return;

    // enable frame buffer
    if((OpenGL3->m_RenderingTarget = _Target) != nullptr)
    {
        OpenGL3->m_FrameBufferTextureWidth  = ApplicationPlatformBackend::get_window_size().x;
        OpenGL3->m_FrameBufferTextureHeight = ApplicationPlatformBackend::get_window_size().y;

        glBindTexture(GL_TEXTURE_2D, OpenGL3->m_FrameBufferTexture);
        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, OpenGL3->m_FrameBufferTextureWidth, OpenGL3->m_FrameBufferTextureHeight, 0, GL_RGBA, GL_UNSIGNED_BYTE, NULL);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
        glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, OpenGL3->m_FrameBufferTexture, 0);
        glBindRenderbuffer(GL_RENDERBUFFER, OpenGL3->m_RBO);
        glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH24_STENCIL8, OpenGL3->m_FrameBufferTextureWidth, OpenGL3->m_FrameBufferTextureHeight);
        glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_STENCIL_ATTACHMENT, GL_RENDERBUFFER, OpenGL3->m_RBO);
        glBindFramebuffer(GL_FRAMEBUFFER, OpenGL3->m_FBO);
    }

    // enable all needed graphics API features
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    glEnable(GL_DEPTH_TEST);
    glEnable(GL_STENCIL_TEST);

    glClear(GL_COLOR_BUFFER_BIT);
    glClear(GL_DEPTH_BUFFER_BIT);
    glClear(GL_STENCIL_BUFFER_BIT);
    
    // bind shader
    glUseProgram(graphics_api<ApplicationRenderingBackendOpenGL>()->m_Shader);
}

void ApplicationRenderingBackend::end_render()
{
    // generate texture for this frame buffer
    std::shared_ptr<ApplicationRenderingBackendOpenGL> OpenGL3 = graphics_api<ApplicationRenderingBackendOpenGL>();

    if(OpenGL3 == nullptr)
        return;

    // save this frame buffer
    if(OpenGL3->m_RenderingTarget != nullptr)
    {
        // in OpenGL we reuse memory
        if(!OpenGL3->m_RenderingTarget->Frames.empty())
        {
            // retrieve the last texture
            ApplicationRenderingBackendTexture frame =
                OpenGL3->m_RenderingTarget->Frames[OpenGL3->m_RenderingTarget->Frames.size() - 1];

            glBindTexture(GL_TEXTURE_2D, frame.Ptr);
            glCopyTexSubImage2D(GL_TEXTURE_2D, 0, 0, 0, 0, 0, OpenGL3->m_FrameBufferTextureWidth, OpenGL3->m_FrameBufferTextureHeight);
        }
        else
        {
            OpenGL3->m_RenderingTarget->Frames.push_back(
                ApplicationRenderingBackend::construct_texture(
                    nullptr,
                    OpenGL3->m_FrameBufferTextureWidth,
                    OpenGL3->m_FrameBufferTextureHeight,
                    ApplicationRenderingBackendTextureFormat_::ApplicationRenderingBackendTextureFormat_RGBA,
                    ApplicationRenderingBackendTextureWrapMode_::ApplicationRenderingBackendTextureWrapMode_Repeat,
                    ApplicationRenderingBackendTextureMinFilter_::ApplicationRenderingBackendTextureMinFilter_Linear, 
                    ApplicationRenderingBackendTextureMaxFilter_::ApplicationRenderingBackendTextureMaxFilter_Linear));
        }
    }

    // disable all tests
    glDisable(GL_BLEND);
    glDisable(GL_DEPTH_TEST);
    glDisable(GL_STENCIL_TEST);
    glDisable(GL_SCISSOR_TEST);

    // unbind everything
    glBindVertexArray(0);
    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
    glBindFramebuffer(GL_FRAMEBUFFER, 0);
    glUseProgram(0);
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
    GLuint texture = (GLuint)_Texture.Ptr;
    glDeleteTextures(1, &texture);
}

bool ApplicationRenderingBackend::load_mesh(
    const ApplicationRenderingBackendMeshVertex*      _Vertexes,
    const ApplicationRenderingBackendMeshVertexIndex& _VertexesCount,
    const ApplicationRenderingBackendMeshVertexIndex* _Indexes,
    const ApplicationRenderingBackendMeshVertexIndex& _IndexesCount)
{
    std::shared_ptr<ApplicationRenderingBackendOpenGL> OpenGL3 = graphics_api<ApplicationRenderingBackendOpenGL>();

    if(OpenGL3 == nullptr)
        return false;

    // load mesh

    // bind VAO to remember VBO/EBO configuration and layout
    glBindVertexArray(OpenGL3->m_VAO);

    // load vertexes and indexes on GPU
    glBindBuffer(GL_ARRAY_BUFFER, OpenGL3->m_VBO);
    glBufferData(GL_ARRAY_BUFFER, _VertexesCount * sizeof(ApplicationRenderingBackendMeshVertex), _Vertexes, GL_DYNAMIC_DRAW);

    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, OpenGL3->m_EBO);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, _IndexesCount * sizeof(ApplicationRenderingBackendMeshVertexIndex), _Indexes, GL_DYNAMIC_DRAW);

    // setup attributes pointers
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(ApplicationRenderingBackendMeshVertex), (void*)(GS_OFFSET_OF(ApplicationRenderingBackendMeshVertex, Position)));
    glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, sizeof(ApplicationRenderingBackendMeshVertex), (void*)(GS_OFFSET_OF(ApplicationRenderingBackendMeshVertex, UV)));
    glVertexAttribPointer(2, 4, GL_UNSIGNED_BYTE, GL_TRUE, sizeof(ApplicationRenderingBackendMeshVertex), (void*)(GS_OFFSET_OF(ApplicationRenderingBackendMeshVertex, Color)));
    glEnableVertexAttribArray(0);
    glEnableVertexAttribArray(1);
    glEnableVertexAttribArray(2);

    // check that everything has been instantiated
    return OpenGL3->m_VBO && OpenGL3->m_EBO && OpenGL3->m_VAO && OpenGL3->m_Shader;
}

void ApplicationRenderingBackend::render_mesh(
    const ApplicationRenderingBackendMeshVertexIndex& _SourceMeshVertex,
    const ApplicationRenderingBackendMeshVertexIndex& _TargetMeshVertex,
    const ApplicationRenderingBackendTexture&         _Texture,
    const gs_mat4f&                                   _MeshProjectionMatrix)
{
    std::shared_ptr<ApplicationRenderingBackendOpenGL> OpenGL3 = graphics_api<ApplicationRenderingBackendOpenGL>();

    if(OpenGL3 == nullptr || _SourceMeshVertex < 0 || _TargetMeshVertex < 0 || (_TargetMeshVertex - _SourceMeshVertex) <= 0)
        return;

    // bind texture
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, _Texture.Ptr);

    glBindVertexArray(OpenGL3->m_VAO);

    // configure shader
    glUniformMatrix4fv(glGetUniformLocation(OpenGL3->m_Shader, "u_ModelMatrix"), 1, GL_FALSE, &_MeshProjectionMatrix[0][0]);
    glUniform1i(glGetUniformLocation(OpenGL3->m_Shader, "u_Texture"), 0);

    // render mesh
    if((OpenGL3->m_MeshRenderingHints & ApplicationRenderingBackendMeshRenderingHints_::ApplicationRenderingBackendMeshRenderingHints_Lines))
        glDrawElements(GL_LINE_LOOP, (_TargetMeshVertex - _SourceMeshVertex), GL_UNSIGNED_INT, (void*)(intptr_t)(_SourceMeshVertex * sizeof(ApplicationRenderingBackendMeshVertexIndex)));

    if((OpenGL3->m_MeshRenderingHints & ApplicationRenderingBackendMeshRenderingHints_::ApplicationRenderingBackendMeshRenderingHints_Triangles))
        glDrawElements(GL_TRIANGLES, (_TargetMeshVertex - _SourceMeshVertex), GL_UNSIGNED_INT, (void*)(intptr_t)(_SourceMeshVertex * sizeof(ApplicationRenderingBackendMeshVertexIndex)));

    // unbind everything
    glBindTexture(GL_TEXTURE_2D, 0);
}

void ApplicationRenderingBackend::clear_color(const gs_color& _Color)
{
    glClearColor(
        (float)gs_color_rgba_get_r(_Color) / 255.f,
        (float)gs_color_rgba_get_g(_Color) / 255.f,
        (float)gs_color_rgba_get_b(_Color) / 255.f,
        (float)gs_color_rgba_get_a(_Color) / 255.f);
}

void ApplicationRenderingBackend::scissor_box(const gs_2d_boxf& _ClippingRect)
{
    glEnable(GL_SCISSOR_TEST);

    gs_vec2f   displayScale = ApplicationPlatformBackend::get_window_framebuffer_size() / ApplicationPlatformBackend::get_window_size();
    gs_2d_boxf clippingBox  = gs_2d_boxf(_ClippingRect.Min * displayScale, _ClippingRect.Max * displayScale);

    glScissor(
        (int)clippingBox.Min.x,
        (int)(ApplicationPlatformBackend::get_window_framebuffer_size().y - clippingBox.Max.y),
        (int)clippingBox.width(),
        (int)clippingBox.height());
}

void ApplicationRenderingBackend::mesh_rendering_hints(const ApplicationRenderingBackendMeshRenderingHints& _Hints)
{
    std::shared_ptr<ApplicationRenderingBackendOpenGL> OpenGL3 = graphics_api<ApplicationRenderingBackendOpenGL>();

    if(OpenGL3 == nullptr)
        return;

    OpenGL3->m_MeshRenderingHints = _Hints;
}

// camera and view projection API
ApplicationRenderingBackend::Projections ApplicationRenderingBackend::calculate_2d_camera_view_and_projection(
    const gs_vec2f& _CameraWorldPosition,
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
    gs_vec3f cameraWorldUpAxisDirection    = gs_vec3f(0.f, 1.f, 0.f);
    gs_vec3f cameraWorldFrontAxisDirection = gs_vec3f(0.f, 0.f, -1.f);
    gs_vec3f cameraLocalFrontAxisDirection = gs_vector_normalize(cameraWorldFrontAxisDirection);
    gs_vec3f cameraLocalRightAxisDirection = gs_vector_normalize(gs_vector_cross(cameraLocalFrontAxisDirection, cameraWorldUpAxisDirection));
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

gs_mat4f ApplicationRenderingBackend::calculate_2d_transform_matrix(const float& _Depth, const gs_vec2f& _Position, const float& _Rotation, const gs_vec2f& _Scale)
{
    gs_mat4f matrix(1.f);

    return gs_matrix_translate(matrix, gs_vec3f(_Position, _Depth)) *
            gs_matrix_rotate(matrix, gs_to_radians(_Rotation), gs_vec3f(0.f, 0.f, 1.f)) * 
            gs_matrix_scale(matrix, gs_vec3f(_Scale, 1.f));
}

bool ApplicationRenderingBackend::compare_objects_depths(const float& _A, const float& _B)
{
    return _A < _B;
}

gs_vec2f ApplicationRenderingBackend::convert_to_NDC(const gs_vec2f& _Position, const gs_vec2f& _Screen)
{
    return gs_vec2f((2.0f * _Position.x) / _Screen.x - 1.0f, 1.0f - (2.0f * _Position.y) / _Screen.y);
}

// TODO: implement 3D perspective camera when it's needed
// template<typename Type>
// auto gs_matrix_calculate_perspective_camera_view_and_projection(
//     const gs_vector<Type, 3>& _CameraWorldPosition,
//     const gs_vector<Type, 3>& _CameraWorldUpAxisDirection,
//     const gs_vector<Type, 3>& _CameraWorldFrontAxisDirection,
//     const gs_vector<Type, 2>& _CameraResolution,
//     const gs_vector<Type, 3>& _CameraEulerAngles,
//     const Type&               _CameraNearPlanePosition,
//     const Type&               _CameraFarPlanePosition,
//     const Type&               _FieldOfView = 90,
//     const Type&               _Aspect      = 1,
//     const float&              _Depth       = 10000)
// {
//     // camera rotation angles
//     // gs_mat4f rotateX  = gs_matrix_rotate(gs_mat4f(1.f), gs_to_radians(0.f), gs_vec3f(1.f, 0.f, 0.f));
//     // gs_mat4f rotateY  = gs_matrix_rotate(gs_mat4f(1.f), gs_to_radians(0.f), gs_vec3f(0.f, 1.f, 0.f));
//     gs_mat4f rotateZ  = gs_matrix_rotate(gs_mat4f(1.f), gs_to_radians(0.f), gs_vec3f(0.f, 0.f, 1.f));

//     // camera local attributes
//     gs_vec3f cameraWorldUpAxisDirection    = gs_vec3f(+0.f, +1.f, +0.f);
//     gs_vec3f cameraLocalFrontAxisDirection = gs_vector_normalize(_CameraWorldFrontAxisDirection);
//     gs_vec3f cameraLocalRightAxisDirection = gs_vector_normalize(gs_vector_cross(cameraLocalFrontAxisDirection, cameraWorldUpAxisDirection));
//     gs_vec3f cameraLocalUpAxisDirection    = gs_vector_normalize(gs_vector_cross(cameraLocalRightAxisDirection, cameraLocalFrontAxisDirection));

//     // rotate around Z axis
//     cameraLocalFrontAxisDirection = gs_vector_normalize(gs_vec3f(rotateZ * gs_vec4f(cameraLocalFrontAxisDirection, 1.f)));
//     cameraLocalUpAxisDirection    = gs_vector_normalize(gs_vec3f(rotateZ * gs_vec4f(cameraWorldUpAxisDirection, 1.f)));

//     // setup projection matrixes
//     gs_mat4f scaleMatrix = gs_matrix_scale(
//         gs_mat4f(1.f), 
//         gs_vec3f(
//             1.f / std::max<float>(_CameraResolution.x, 1.f), 
//             1.f / std::max<float>(_CameraResolution.y, 1.f), 
//             1.f / _Depth
//         )
//     );

//     gs_vec3f cameraWorldPosition = gs_vec3f(+0.0f, -0.0f, +1.f); // scaleMatrix * gs_vec4f(_CameraWorldPosition, 1.f);

//     gs_mat4f cameraview = gs_matrix_look_at(cameraWorldPosition, cameraWorldPosition + cameraLocalFrontAxisDirection, cameraLocalUpAxisDirection) * scaleMatrix;
//     gs_mat4f projection = gs_matrix_perspective(
//         gs_to_radians(_FieldOfView),
//         1.f,
//         _CameraNearPlanePosition / _Depth,
//         _CameraFarPlanePosition / _Depth);

//     struct
//     {
//         gs_matrix<Type, 4, 4> cameraview;
//         gs_matrix<Type, 4, 4> projection;
//     } result = {cameraview, projection};

//     return result;
// }