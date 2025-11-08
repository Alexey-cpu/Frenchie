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
        enum PlatformBackendRendererContextHints_ : int
        {
            // tweaks
            PlatformBackendRendererContextHints_None,
            PlatformBackendRendererContextHints_Visible,
            PlatformBackendRendererContextHints_Decorated,
            PlatformBackendRendererContextHints_Resizable,
            PlatformBackendRendererContextHints_Iconified,
            PlatformBackendRendererContextHints_Focused,

            // defaults
            PlatformBackendContextWindowHints_Default =
                PlatformBackendRendererContextHints_Visible   |
                PlatformBackendRendererContextHints_Decorated |
                PlatformBackendRendererContextHints_Resizable |
                PlatformBackendRendererContextHints_Focused
        };

        enum PlatformBackendRendererHints_ : int
        {
            // tweaks
            PlatformBackendRendererHints_None,
            PlatformBackendRendererHints_ClearColorBuffer,
            PlatformBackendRendererHints_ClearDepthBuffer,
            PlatformBackendRendererHints_ClearStencilBuffer,
            PlatformBackendRendererHints_PollEvents,

            // defaults
            PlatformBackendRendererHints_Default =
                PlatformBackendRendererHints_ClearColorBuffer   |
                PlatformBackendRendererHints_ClearDepthBuffer   |
                PlatformBackendRendererHints_ClearStencilBuffer |
                PlatformBackendRendererHints_PollEvents
        };

        enum PlatformBackendRendererTextureFormat_ : int
        {
            PlatformBackendRendererTextureFormat_ALPHA,
            PlatformBackendRendererTextureFormat_RGB,
            PlatformBackendRendererTextureFormat_RGBA,
        };

        enum PlatformBackendRendererTextureWrap_ : int
        {
            PlatformBackendRendererTextureWrap_Repeat,
            PlatformBackendRendererTextureWrap_Mirrored,
            PlatformBackendRendererTextureWrap_ClampToEdge,
            PlatformBackendRendererTextureWrap_ClampToBorder
        };

        enum PlatformBackendRendererTextureMinFilter_ : int
        {
            PlatformBackendRendererTextureMinFilter_Linear,
            PlatformBackendRendererTextureMinFilter_Nearest,
            PlatformBackendRendererTextureMinFilter_NearestMipMapLinear,
            PlatformBackendRendererTextureMinFilter_NearestMipMapNearest,
            PlatformBackendRendererTextureMinFilter_LinearMipMapLinear,
            PlatformBackendRendererTextureMinFilter_LinearMipMapNearest,
        };

        enum PlatformBackendRendererTextureMaxFilter_ : int
        {
            PlatformBackendRendererTextureMaxFilter_Linear,
            PlatformBackendRendererTextureMaxFilter_Nearest
        };

        enum PlatformBackendRendererShaderType_ : int
        {
            PlatformBackendRendererShaderType_Vertex,
            PlatformBackendRendererShaderType_Fragment
        };

        typedef int PlatformBackendContextWindowHints;
        typedef int PlatformBackendRendererHints;
        typedef int PlatformBackendRendererTextureFormat;
        typedef int PlatformBackendRendererTextureWrap;
        typedef int PlatformBackendRendererTextureMinFilter;
        typedef int PlatformBackendRendererTextureMaxFilter;
        typedef int PlatformBackendRendererShaderType;

        template<typename Type>
        class PlatformBackendRendererAllocator;

        // enities
        struct PlatformBackendRendererTexture final
        {
            PlatformBackendRendererTexture(
                const unsigned int&                            _Ptr,
                const int&                                     _Width,
                const int&                                     _Height,
                const PlatformBackendRendererTextureFormat&    _Format,
                const PlatformBackendRendererTextureWrap&      _Wrap,
                const PlatformBackendRendererTextureMinFilter& _MinFilter,
                const PlatformBackendRendererTextureMaxFilter& _MaxFilter) : 
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
            const PlatformBackendRendererTextureFormat    Format    {PlatformBackendRendererTextureFormat_::PlatformBackendRendererTextureFormat_RGBA};
            const PlatformBackendRendererTextureWrap      Wrap      {PlatformBackendRendererTextureWrap_::PlatformBackendRendererTextureWrap_Repeat};
            const PlatformBackendRendererTextureMinFilter MinFilter {PlatformBackendRendererTextureMinFilter_::PlatformBackendRendererTextureMinFilter_Linear};
            const PlatformBackendRendererTextureMaxFilter MaxFilter {PlatformBackendRendererTextureMaxFilter_::PlatformBackendRendererTextureMaxFilter_Linear};
        };

        struct PlatformBackendRendererShader final
        {
            PlatformBackendRendererShader(const unsigned int& _Ptr) : Ptr(_Ptr){}

            // info
            const unsigned int Ptr {0};
        };

        class PlatformBackendRenderer final
        {
        public:
            PlatformBackendRenderer();
            ~PlatformBackendRenderer();

            // getters
            void* get_context() const;

            // API
            bool awake(
                const char*                       _Name        = "DefaultPlatformWindow",
                void*                             _Share       = nullptr,
                PlatformBackendContextWindowHints _WindowHints = PlatformBackendRendererContextHints_::PlatformBackendContextWindowHints_Default);

            void frame_start(
                const PlatformBackendRendererHints& _RendererHints = PlatformBackendRendererHints_Default);

            void frame_update();
            void frame_render();
            void frame_finish();
            void finish();
            void quit();
            bool is_closed() const;
            void close();

            // images API
            std::shared_ptr<PlatformBackendRendererTexture> construct_image(
                const unsigned char*                           _RawBuffer,
                const int&                                     _Width,
                const int&                                     _Height,
                const PlatformBackendRendererTextureFormat&    _Format    = PlatformBackendRendererTextureFormat_::PlatformBackendRendererTextureFormat_RGBA,
                const PlatformBackendRendererTextureWrap&      _Wrap      = PlatformBackendRendererTextureWrap_::PlatformBackendRendererTextureWrap_Repeat,
                const PlatformBackendRendererTextureMinFilter& _MinFilter = PlatformBackendRendererTextureMinFilter_::PlatformBackendRendererTextureMinFilter_Linear, 
                const PlatformBackendRendererTextureMaxFilter& _MaxFilter = PlatformBackendRendererTextureMaxFilter_::PlatformBackendRendererTextureMaxFilter_Linear) const;

            std::shared_ptr<PlatformBackendRendererTexture> construct_image(
                const std::filesystem::path&                   _FilePath,
                const PlatformBackendRendererTextureFormat&    _Format    = PlatformBackendRendererTextureFormat_::PlatformBackendRendererTextureFormat_RGBA,
                const PlatformBackendRendererTextureWrap&      _Wrap      = PlatformBackendRendererTextureWrap_::PlatformBackendRendererTextureWrap_Repeat,
                const PlatformBackendRendererTextureMinFilter& _MinFilter = PlatformBackendRendererTextureMinFilter_::PlatformBackendRendererTextureMinFilter_Linear, 
                const PlatformBackendRendererTextureMaxFilter& _MaxFilter = PlatformBackendRendererTextureMaxFilter_::PlatformBackendRendererTextureMaxFilter_Linear) const;

            // shaders API
            std::shared_ptr<PlatformBackendRendererShader> construct_shader(
                const std::vector<std::pair<std::string, PlatformBackendRendererShaderType>>& ShaderInfos) const;
            
            std::shared_ptr<PlatformBackendRendererShader> construct_shader(
                const std::vector<std::filesystem::path>& _ShaderFilesPaths) const;

            void set_shader_uniform(PlatformBackendRendererShader* _Shader, const std::string& _Uniform, const bool& _Value);
            void set_shader_uniform(PlatformBackendRendererShader* _Shader, const std::string& _Uniform, const int& _Value);
            void set_shader_uniform(PlatformBackendRendererShader* _Shader, const std::string& _Uniform, const float& _Value);
            void set_shader_uniform(PlatformBackendRendererShader* _Shader, const std::string& _Uniform, const glm::vec2& _Value);
            void set_shader_uniform(PlatformBackendRendererShader* _Shader, const std::string& _Uniform, const glm::vec3& _Value);
            void set_shader_uniform(PlatformBackendRendererShader* _Shader, const std::string& _Uniform, const glm::vec4& _Value);
            void set_shader_uniform(PlatformBackendRendererShader* _Shader, const std::string& _Uniform, const glm::mat2& _Value);
            void set_shader_uniform(PlatformBackendRendererShader* _Shader, const std::string& _Uniform, const glm::mat3& _Value);
            void set_shader_uniform(PlatformBackendRendererShader* _Shader, const std::string& _Uniform, const glm::mat4& _Value);
            
        protected:

            std::unique_ptr<Frenchie::Core::Memory::MemoryChunkAllocator<PlatformBackendRendererTexture>> m_Textures{nullptr};
            std::unique_ptr<Frenchie::Core::Memory::MemoryChunkAllocator<PlatformBackendRendererShader>>  m_Shaders {nullptr};
            void*                                                                                         m_Context {nullptr};
        };
    }
}