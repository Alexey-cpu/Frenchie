#pragma once

#include <functional>

// GLM
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <glm/gtx/euler_angles.hpp>

namespace Frenchie
{
    namespace Application
    {
        enum PlatformBackendContextWindowHints_ : int
        {
            // tweaks
            PlatformBackendContextWindowHints_None,
            PlatformBackendContextWindowHints_Visible,
            PlatformBackendContextWindowHints_Decorated,
            PlatformBackendContextWindowHints_Resizable,
            PlatformBackendContextWindowHints_Iconified,
            PlatformBackendContextWindowHints_Focused,

            // defaults
            PlatformBackendContextWindowHints_Default =
                PlatformBackendContextWindowHints_Visible   |
                PlatformBackendContextWindowHints_Decorated |
                PlatformBackendContextWindowHints_Resizable |
                PlatformBackendContextWindowHints_Focused
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
            PlatformBackendRendererShaderType_Fragment,
            PlatformBackendRendererShaderType_Program
        };

        typedef int PlatformBackendContextWindowHints;
        typedef int PlatformBackendRendererHints;
        typedef int PlatformBackendRendererTextureFormat;
        typedef int PlatformBackendRendererTextureWrap;
        typedef int PlatformBackendRendererTextureMinFilter;
        typedef int PlatformBackendRendererTextureMaxFilter;
        typedef int PlatformBackendRendererShaderType;

        template<typename Type>
        class PlatformBackendAllocator;

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
            const unsigned int                    Ptr       {+0};
            const int                             Width     {-1};
            const int                             Height    {-1};
            const PlatformBackendRendererTextureFormat    Format    {PlatformBackendRendererTextureFormat_::PlatformBackendRendererTextureFormat_RGBA};
            const PlatformBackendRendererTextureWrap      Wrap      {PlatformBackendRendererTextureWrap_::PlatformBackendRendererTextureWrap_Repeat};
            const PlatformBackendRendererTextureMinFilter MinFilter {PlatformBackendRendererTextureMinFilter_::PlatformBackendRendererTextureMinFilter_Linear};
            const PlatformBackendRendererTextureMaxFilter MaxFilter {PlatformBackendRendererTextureMaxFilter_::PlatformBackendRendererTextureMaxFilter_Linear};
        };

        struct PlatformBackendRendererShader final
        {
            PlatformBackendRendererShader(
                const unsigned int&                      _Ptr,
                const PlatformBackendRendererShaderType& _Type) :
                Ptr(_Ptr),
                Type(_Type){}

            // info
            const unsigned int                      Ptr {0};
            const PlatformBackendRendererShaderType Type{PlatformBackendRendererShaderType_::PlatformBackendRendererShaderType_Vertex};
        };

        class PlatformBackendInstance
        {
        public:
            PlatformBackendInstance();
            virtual ~PlatformBackendInstance();

            // getters
            void* get_context() const;

            // virtual API
            virtual bool awake(
                const char*                       _Name        = "DefaultPlatformWindow",
                void*                             _Share       = nullptr,
                PlatformBackendContextWindowHints _WindowHints = PlatformBackendContextWindowHints_::PlatformBackendContextWindowHints_Default);

            virtual void frame_start(
                const PlatformBackendRendererHints& _RendererHints           = PlatformBackendRendererHints_Default,
                const int                           _FrameBufferSwapInterval = 1);

            virtual void frame_update();
            virtual void frame_render();
            virtual void frame_finish();
            virtual void finish();
            virtual void quit();
            virtual bool is_closed() const;
            virtual void close();

            // loaders
            virtual PlatformBackendRendererTexture* construct_image(
                const unsigned char*                           _RawBuffer,
                const int&                                     _Width,
                const int&                                     _Height,
                const PlatformBackendRendererTextureFormat&    _Format    = PlatformBackendRendererTextureFormat_::PlatformBackendRendererTextureFormat_RGBA,
                const PlatformBackendRendererTextureWrap&      _Wrap      = PlatformBackendRendererTextureWrap_::PlatformBackendRendererTextureWrap_Repeat,
                const PlatformBackendRendererTextureMinFilter& _MinFilter = PlatformBackendRendererTextureMinFilter_::PlatformBackendRendererTextureMinFilter_Linear, 
                const PlatformBackendRendererTextureMaxFilter& _MaxFilter = PlatformBackendRendererTextureMaxFilter_::PlatformBackendRendererTextureMaxFilter_Linear
            );

            virtual PlatformBackendRendererTexture* construct_image(
                const char*                                    _FilePath,
                const PlatformBackendRendererTextureFormat&    _Format                = PlatformBackendRendererTextureFormat_::PlatformBackendRendererTextureFormat_RGBA,
                const PlatformBackendRendererTextureWrap&      _Wrap                  = PlatformBackendRendererTextureWrap_::PlatformBackendRendererTextureWrap_Repeat,
                const PlatformBackendRendererTextureMinFilter& _MinFilter             = PlatformBackendRendererTextureMinFilter_::PlatformBackendRendererTextureMinFilter_Linear, 
                const PlatformBackendRendererTextureMaxFilter& _MaxFilter             = PlatformBackendRendererTextureMaxFilter_::PlatformBackendRendererTextureMaxFilter_Linear,
                char**                                         _ImageLoadStatusBuffer = nullptr // null terminated buffer
            );

            void destroy_image(PlatformBackendRendererTexture* _Image);

        protected:

            PlatformBackendAllocator<PlatformBackendRendererTexture>* m_Textures{nullptr};
            PlatformBackendAllocator<PlatformBackendRendererShader>*  m_Shaders {nullptr};
            void*                                                     m_Context {nullptr};
        };
    }
}