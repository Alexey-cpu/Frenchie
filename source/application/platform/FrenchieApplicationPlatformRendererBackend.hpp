#pragma once

// Core
#include <FrenchieCoreMemoryChunkAllocator.hpp>

// GLM
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <glm/gtx/euler_angles.hpp>

// STL
#include <filesystem>
#include <utility>
#include <memory>
#include <vector>
#include <string>

namespace Frenchie
{
    namespace Application
    {
        enum PlatformRendererBackendContextHints_ : int
        {
            // tweaks
            PlatformRendererBackendContextHints_None,
            PlatformRendererBackendContextHints_Visible,
            PlatformRendererBackendContextHints_Decorated,
            PlatformRendererBackendContextHints_Resizable,
            PlatformRendererBackendContextHints_Iconified,
            PlatformRendererBackendContextHints_Focused,

            // defaults
            PlatformRendererBackendContextHints_Default =
                PlatformRendererBackendContextHints_Visible   |
                PlatformRendererBackendContextHints_Decorated |
                PlatformRendererBackendContextHints_Resizable |
                PlatformRendererBackendContextHints_Focused
        };

        enum PlatformRendererBackendHints_ : int
        {
            // tweaks
            PlatformRendererBackendHints_None,
            PlatformRendererBackendHints_ClearColorBuffer,
            PlatformRendererBackendHints_ClearDepthBuffer,
            PlatformRendererBackendHints_ClearStencilBuffer,
            PlatformRendererBackendHints_PollEvents,

            // defaults
            PlatformRendererBackendHints_Default =
                PlatformRendererBackendHints_ClearColorBuffer   |
                PlatformRendererBackendHints_ClearDepthBuffer   |
                PlatformRendererBackendHints_ClearStencilBuffer |
                PlatformRendererBackendHints_PollEvents
        };

        enum PlatformRendererBackendTextureFormat_ : int
        {
            PlatformRendererBackendTextureFormat_ALPHA,
            PlatformRendererBackendTextureFormat_RGB,
            PlatformRendererBackendTextureFormat_RGBA,
        };

        enum PlatformRendererBackendTextureWrap_ : int
        {
            PlatformRendererBackendTextureWrap_Repeat,
            PlatformRendererBackendTextureWrap_Mirrored,
            PlatformRendererBackendTextureWrap_ClampToEdge,
            PlatformRendererBackendTextureWrap_ClampToBorder
        };

        enum PlatformRendererBackendTextureMinFilter_ : int
        {
            PlatformRendererBackendTextureMinFilter_Linear,
            PlatformRendererBackendTextureMinFilter_Nearest,
            PlatformRendererBackendTextureMinFilter_NearestMipMapLinear,
            PlatformRendererBackendTextureMinFilter_NearestMipMapNearest,
            PlatformRendererBackendTextureMinFilter_LinearMipMapLinear,
            PlatformRendererBackendTextureMinFilter_LinearMipMapNearest,
        };

        enum PlatformRendererBackendTextureMaxFilter_ : int
        {
            PlatformRendererBackendTextureMaxFilter_Linear,
            PlatformRendererBackendTextureMaxFilter_Nearest,
        };

        enum PlatformRendererBackendShaderType_ : int
        {
            PlatformRendererBackendShaderType_Vertex,
            PlatformRendererBackendShaderType_Fragment
        };

        enum PlatformRendererBackendMeshRenderingHints_ : int
        {
            PlatformRendererBackendMeshRenderingHints_Points,
            PlatformRendererBackendMeshRenderingHints_Lines,
            PlatformRendererBackendMeshRenderingHints_Triangles,
        };

        typedef int PlatformRendererBackendContextHints;
        typedef int PlatformRendererBackendHints;
        typedef int PlatformRendererBackendTextureFormat;
        typedef int PlatformRendererBackendTextureWrap;
        typedef int PlatformRendererBackendTextureMinFilter;
        typedef int PlatformRendererBackendTextureMaxFilter;
        typedef int PlatformRendererBackendShaderType;
        typedef int PlatformRendererBackendMeshRenderingHints;

        // enities
        struct PlatformRendererBackendTexture final
        {
            PlatformRendererBackendTexture(
                const unsigned int&                            _Ptr       = 0,
                const int&                                     _Width     = 128,
                const int&                                     _Height    = 128,
                const PlatformRendererBackendTextureFormat&    _Format    = PlatformRendererBackendTextureFormat_RGBA,
                const PlatformRendererBackendTextureWrap&      _Wrap      = PlatformRendererBackendTextureWrap_::PlatformRendererBackendTextureWrap_Repeat,
                const PlatformRendererBackendTextureMinFilter& _MinFilter = PlatformRendererBackendTextureMinFilter_::PlatformRendererBackendTextureMinFilter_Linear,
                const PlatformRendererBackendTextureMaxFilter& _MaxFilter = PlatformRendererBackendTextureMaxFilter_::PlatformRendererBackendTextureMaxFilter_Linear) : 
            Width(_Width),
            Height(_Height),
            Ptr(_Ptr),
            Format(_Format),
            Wrap(_Wrap),
            MinFilter(_MinFilter),
            MaxFilter(_MaxFilter){}

            // info
            const unsigned int                            Ptr       {+0};
            const int                                     Width     {-1};
            const int                                     Height    {-1};
            const PlatformRendererBackendTextureFormat    Format    {PlatformRendererBackendTextureFormat_::PlatformRendererBackendTextureFormat_RGBA};
            const PlatformRendererBackendTextureWrap      Wrap      {PlatformRendererBackendTextureWrap_::PlatformRendererBackendTextureWrap_Repeat};
            const PlatformRendererBackendTextureMinFilter MinFilter {PlatformRendererBackendTextureMinFilter_::PlatformRendererBackendTextureMinFilter_Linear};
            const PlatformRendererBackendTextureMaxFilter MaxFilter {PlatformRendererBackendTextureMaxFilter_::PlatformRendererBackendTextureMaxFilter_Linear};
        };

        struct PlatformRendererBackendShader final
        {
            PlatformRendererBackendShader(const unsigned int& _Ptr = 0) : Ptr(_Ptr){}

            // info
            const unsigned int Ptr {0};
        };

        struct PlatformRendererBackendMesh final
        {
            PlatformRendererBackendMesh(
                const unsigned int& _VBO = 0,
                const unsigned int& _VAO = 0,
                const unsigned int& _EBO = 0) :
            VBO(_VBO),
            VAO(_VAO),
            EBO(_EBO){}

            const unsigned int VBO{0};
            const unsigned int VAO{0};
            const unsigned int EBO{0};
        };

        struct PlatformRendererBackendMeshVertex
        {
            PlatformRendererBackendMeshVertex(
                const glm::vec3& _Position = glm::vec3(0),
                const glm::vec3& _Normal   = glm::vec3(0),
                const glm::vec2& _UV       = glm::vec2(0)) :
            Position(_Position),
            Normal(_Normal),
            UV(_UV){}

            glm::vec3 Position;
            glm::vec3 Normal;
            glm::vec2 UV;
        };

        class PlatformRendererBackend final
        {
        public:
            PlatformRendererBackend();
            ~PlatformRendererBackend();

            // getters
            void*     get_context() const;
            glm::vec4 get_context_window_clear_color() const;
            glm::vec2 get_context_window_position() const;
            glm::vec2 get_context_window_size() const;

            // setters
            void set_context_window_clear_color(const glm::vec4&);
            void set_context_window_position(const glm::vec2&);
            void set_context_window_size(const glm::vec2&);

            // API
            bool awake(
                const char*                         _Name        = "DefaultPlatformWindow",
                void*                               _Share       = nullptr,
                PlatformRendererBackendContextHints _WindowHints = PlatformRendererBackendContextHints_::PlatformRendererBackendContextHints_Default);

            void frame_start(
                const PlatformRendererBackendHints& _RendererHints = PlatformRendererBackendHints_Default);

            void frame_update();
            void frame_render();
            void frame_finish();
            void finish();
            void quit();
            bool is_closed() const;
            void close();

            // images API
            std::shared_ptr<PlatformRendererBackendTexture> construct_image(
                const unsigned char*                           _RawBuffer,
                const int&                                     _Width,
                const int&                                     _Height,
                const PlatformRendererBackendTextureFormat&    _Format    = PlatformRendererBackendTextureFormat_::PlatformRendererBackendTextureFormat_RGBA,
                const PlatformRendererBackendTextureWrap&      _Wrap      = PlatformRendererBackendTextureWrap_::PlatformRendererBackendTextureWrap_Repeat,
                const PlatformRendererBackendTextureMinFilter& _MinFilter = PlatformRendererBackendTextureMinFilter_::PlatformRendererBackendTextureMinFilter_Linear, 
                const PlatformRendererBackendTextureMaxFilter& _MaxFilter = PlatformRendererBackendTextureMaxFilter_::PlatformRendererBackendTextureMaxFilter_Linear) const;

            std::shared_ptr<PlatformRendererBackendTexture> construct_image(
                const std::filesystem::path&                   _FilePath,
                const PlatformRendererBackendTextureFormat&    _Format    = PlatformRendererBackendTextureFormat_::PlatformRendererBackendTextureFormat_RGBA,
                const PlatformRendererBackendTextureWrap&      _Wrap      = PlatformRendererBackendTextureWrap_::PlatformRendererBackendTextureWrap_Repeat,
                const PlatformRendererBackendTextureMinFilter& _MinFilter = PlatformRendererBackendTextureMinFilter_::PlatformRendererBackendTextureMinFilter_Linear, 
                const PlatformRendererBackendTextureMaxFilter& _MaxFilter = PlatformRendererBackendTextureMaxFilter_::PlatformRendererBackendTextureMaxFilter_Linear) const;

            void destroy_image();

            // shaders API
            std::shared_ptr<PlatformRendererBackendShader> construct_shader(
                const std::vector<std::pair<std::string, PlatformRendererBackendShaderType>>& ShaderInfos =
                    std::vector<std::pair<std::string, PlatformRendererBackendShaderType>>()) const;
            
            std::shared_ptr<PlatformRendererBackendShader> construct_shader(
                const std::vector<std::filesystem::path>& _ShaderFilesPaths =
                    std::vector<std::filesystem::path>()) const;

            void set_shader_uniform(const std::shared_ptr<PlatformRendererBackendShader>& _Shader, const std::string& _Uniform, const bool& _Value);
            void set_shader_uniform(const std::shared_ptr<PlatformRendererBackendShader>& _Shader, const std::string& _Uniform, const int& _Value);
            void set_shader_uniform(const std::shared_ptr<PlatformRendererBackendShader>& _Shader, const std::string& _Uniform, const float& _Value);
            void set_shader_uniform(const std::shared_ptr<PlatformRendererBackendShader>& _Shader, const std::string& _Uniform, const glm::vec2& _Value);
            void set_shader_uniform(const std::shared_ptr<PlatformRendererBackendShader>& _Shader, const std::string& _Uniform, const glm::vec3& _Value);
            void set_shader_uniform(const std::shared_ptr<PlatformRendererBackendShader>& _Shader, const std::string& _Uniform, const glm::vec4& _Value);
            void set_shader_uniform(const std::shared_ptr<PlatformRendererBackendShader>& _Shader, const std::string& _Uniform, const glm::mat2& _Value);
            void set_shader_uniform(const std::shared_ptr<PlatformRendererBackendShader>& _Shader, const std::string& _Uniform, const glm::mat3& _Value);
            void set_shader_uniform(const std::shared_ptr<PlatformRendererBackendShader>& _Shader, const std::string& _Uniform, const glm::mat4& _Value);

            void begin_use_shader(const std::shared_ptr<PlatformRendererBackendShader>& _Shader);
            void endup_use_shader();
            
            // mesh
            std::shared_ptr<PlatformRendererBackendMesh> construct_mesh(std::vector<PlatformRendererBackendMeshVertex> _Vertexes);
           
            void begin_render_mesh(
                const std::shared_ptr<PlatformRendererBackendMesh>& _Mesh,
                PlatformRendererBackendMeshRenderingHints           _MeshRenderHints);

            void endup_render_mesh();

        protected:

            std::unique_ptr<Frenchie::Core::Memory::MemoryChunkAllocator<PlatformRendererBackendMesh>>    m_Meshes    {nullptr};
            std::unique_ptr<Frenchie::Core::Memory::MemoryChunkAllocator<PlatformRendererBackendShader>>  m_Shaders   {nullptr};
            std::unique_ptr<Frenchie::Core::Memory::MemoryChunkAllocator<PlatformRendererBackendTexture>> m_Textures  {nullptr};
            void*                                                                                         m_Context   {nullptr};
            glm::vec4                                                                                     m_ClearColor{128, 128, 128, 255};
        };

        // class PlatformImmediate2DRenderer
        // {
        // public:
        //     PlatformImmediate2DRenderer(){}
        //     ~PlatformImmediate2DRenderer(){}

        //     void push_vertex(const PlatformRendererBackendMeshVertex& _Vertex)
        //     {
        //         m_Vertexes[m_CurrentVertex++] = _Vertex;
        //     }

        // protected:
        //     std::vector<PlatformRendererBackendMeshVertex> m_Vertexes     {std::vector<PlatformRendererBackendMeshVertex>(1024)};
        //     std::vector<int>                               m_Indexes      {std::vector<int>(1024)};
        //     int                                            m_CurrentVertex{0};
        //     int                                            m_CurrentIndex {0};
        // };
    }
}