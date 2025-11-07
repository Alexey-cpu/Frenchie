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
            PlatformBackendContextWindowHint_None,
            PlatformBackendContextWindowHint_Visible,
            PlatformBackendContextWindowHint_Decorated,
            PlatformBackendContextWindowHint_Resizable,
            PlatformBackendContextWindowHint_Iconified,
            PlatformBackendContextWindowHint_Focused,
        };

        enum PlatformBackendTextureFormat_ : int
        {
            PlatformBackendTextureFormat_ALPHA,
            PlatformBackendTextureFormat_RGB,
            PlatformBackendTextureFormat_RGBA,
        };

        enum PlatformBackendTextureWrap_ : int
        {
            PlatformBackendTextureWrap_Repeat,
            PlatformBackendTextureWrap_Mirrored,
            PlatformBackendTextureWrap_ClampToEdge,
            PlatformBackendTextureWrap_ClampToBorder
        };

        enum PlatformBackendTextureMinFilter_ : int
        {
            PlatformBackendTextureMinFilter_Linear,
            PlatformBackendTextureMinFilter_Nearest,
            PlatformBackendTextureMinFilter_NearestMipMapLinear,
            PlatformBackendTextureMinFilter_NearestMipMapNearest,
            PlatformBackendTextureMinFilter_LinearMipMapLinear,
            PlatformBackendTextureMinFilter_LinearMipMapNearest,
        };

        enum PlatformBackendTextureMaxFilter_ : int
        {
            PlatformBackendTextureMaxFilter_Linear,
            PlatformBackendTextureMaxFilter_Nearest
        };

        enum PlatformBackendShaderType_ : int
        {
            PlatformBackendPlatformBackendShaderType_Vertex,
            PlatformBackendPlatformBackendShaderType_Fragment,
            PlatformBackendPlatformBackendShaderType_Program
        };

        typedef int PlatformBackendContextWindowHints;
        typedef int PlatformBackendTextureFormat;
        typedef int PlatformBackendTextureWrap;
        typedef int PlatformBackendTextureMinFilter;
        typedef int PlatformBackendTextureMaxFilter;
        typedef int PlatformBackendShaderType;

        // forward declaration
        template<typename Type> class PlatformBackendAllocator;

        struct PlatformBackendTexture
        {
            PlatformBackendTexture(
                const unsigned int&                    _Ptr,
                const int&                             _Width,
                const int&                             _Height,
                const PlatformBackendTextureFormat&    _Format,
                const PlatformBackendTextureWrap&      _Wrap,
                const PlatformBackendTextureMinFilter& _MinFilter,
                const PlatformBackendTextureMaxFilter& _MaxFilter) : 
                Width(_Width),
                Height(_Height),
                Ptr(_Ptr),
                Format(_Format),
                Wrap(_Wrap),
                MinFilter(_MinFilter),
                MaxFilter(_MaxFilter){}

            const unsigned int                    Ptr       {+0};
            const int                             Width     {-1};
            const int                             Height    {-1};
            const PlatformBackendTextureFormat    Format    {PlatformBackendTextureFormat_::PlatformBackendTextureFormat_RGBA};
            const PlatformBackendTextureWrap      Wrap      {PlatformBackendTextureWrap_::PlatformBackendTextureWrap_Repeat};
            const PlatformBackendTextureMinFilter MinFilter {PlatformBackendTextureMinFilter_::PlatformBackendTextureMinFilter_Linear};
            const PlatformBackendTextureMaxFilter MaxFilter {PlatformBackendTextureMaxFilter_::PlatformBackendTextureMaxFilter_Linear};
        };

        struct PlatformBackendShader
        {
            PlatformBackendShader(
                const unsigned int&              _Ptr,
                const PlatformBackendShaderType& _Type) :
                Ptr(_Ptr),
                Type(_Type){}

            const unsigned int              Ptr {0};
            const PlatformBackendShaderType Type{PlatformBackendShaderType_::PlatformBackendPlatformBackendShaderType_Vertex};
        };

        class PlatformBackendInstance
        {
        public:
            PlatformBackendInstance();
            virtual ~PlatformBackendInstance();

            // getters
            int get_context_window_width() const;
            int get_context_window_height() const;

            // setters
            void set_context_window_width(const int& _Width);
            void set_context_window_height(const int& _Height);

            // API
            bool initialize();

            void setup_context_window_hints(PlatformBackendContextWindowHints _PlatformBackendContextWindowHints);

            bool create_context_window(
                const char*                       _PlatformBackendContextWindowName    = "DefaultPlatformWindow",
                const int&                        _PlatformBackendContextWindowWidth   = 2048,
                const int&                        _PlatformBackendContextWindowHeight  = 1024,
                void*                             _PlatformBackendContextWindowShare   = nullptr,
                PlatformBackendContextWindowHints _PlatformBackendContextWindowHints   =
                    PlatformBackendContextWindowHints_::PlatformBackendContextWindowHint_None);
            
            void maximize_context_window();

            void set_buffers_swap_interval(const int& _Milliseconds = 1);
            void clear_color_buffer();
            void clear_depth_buffer();
            void clear_stencil_buffer();
            void swap_buffers();
            void destroy_context_window();

            // loaders
            PlatformBackendTexture* construct_image(
                const char*                            _FilePath,
                const PlatformBackendTextureFormat&    _Format                  = PlatformBackendTextureFormat_::PlatformBackendTextureFormat_RGBA,
                const PlatformBackendTextureWrap&      _Wrap                    = PlatformBackendTextureWrap_::PlatformBackendTextureWrap_Repeat,
                const PlatformBackendTextureMinFilter& _MinFilter               = PlatformBackendTextureMinFilter_::PlatformBackendTextureMinFilter_Linear, 
                const PlatformBackendTextureMaxFilter& _MaxFilter               = PlatformBackendTextureMaxFilter_::PlatformBackendTextureMaxFilter_Linear,
                char**                                 _ImageLoadStatusBuffer   = nullptr // null terminated buffer
            );

            void destroy_image(PlatformBackendTexture* _Image);

        protected:

            PlatformBackendAllocator<PlatformBackendTexture>* m_Textures{nullptr};
            PlatformBackendAllocator<PlatformBackendShader>*  m_Shaders {nullptr};
            void*                                             m_Context {nullptr};
        };
    }
}