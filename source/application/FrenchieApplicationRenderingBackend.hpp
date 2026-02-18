#pragma once

// Core
#include <FrenchieCoreMath.hpp>

// STL
#include <vector>
#include <memory>
#include <vector>
#include <string>

namespace Frenchie
{
    namespace Application
    {
        enum ApplicationRenderingBackendTextureFormat_ : int
        {
            ApplicationRenderingBackendTextureFormat_ALPHA,
            ApplicationRenderingBackendTextureFormat_RGB,
            ApplicationRenderingBackendTextureFormat_RGBA,
        };

        enum ApplicationRenderingBackendTextureWrapMode_ : int
        {
            ApplicationRenderingBackendTextureWrapMode_Repeat,
            ApplicationRenderingBackendTextureWrapMode_Mirrored,
            ApplicationRenderingBackendTextureWrapMode_ClampToEdge,
            ApplicationRenderingBackendTextureWrapMode_ClampToBorder
        };

        enum ApplicationRenderingBackendTextureMinFilter_ : int
        {
            ApplicationRenderingBackendTextureMinFilter_Linear,
            ApplicationRenderingBackendTextureMinFilter_Nearest,
            ApplicationRenderingBackendTextureMinFilter_NearestMipMapLinear,
            ApplicationRenderingBackendTextureMinFilter_NearestMipMapNearest,
            ApplicationRenderingBackendTextureMinFilter_LinearMipMapLinear,
            ApplicationRenderingBackendTextureMinFilter_LinearMipMapNearest,
        };

        enum ApplicationRenderingBackendTextureMaxFilter_ : int
        {
            ApplicationRenderingBackendTextureMaxFilter_Linear,
            ApplicationRenderingBackendTextureMaxFilter_Nearest,
        };

        enum ApplicationRenderingBackendGraphicsApiHints_ : int
        {
            ApplicationRenderingBackendGraphicsApiHints_Blending    = 1 << 0,
            ApplicationRenderingBackendGraphicsApiHints_DepthTest   = 1 << 1,
            ApplicationRenderingBackendGraphicsApiHints_StencilTest = 1 << 2,
            ApplicationRenderingBackendGraphicsApiHints_ScissorTest = 1 << 3,
        };

        enum ApplicationRenderingBackendGraphicsApiBuffers_ : int
        {
            ApplicationRenderingBackendGraphicsApiBuffers_Color   = 1 << 0,
            ApplicationRenderingBackendGraphicsApiBuffers_Depth   = 1 << 1,
            ApplicationRenderingBackendGraphicsApiBuffers_Stencil = 1 << 2,
        };

        enum ApplicationRenderingBackendGraphicsApiRenderingHints_ : int
        {
            ApplicationRenderingBackendGraphicsApiRenderingHints_Lines     = 1 << 1,
            ApplicationRenderingBackendGraphicsApiRenderingHints_Points    = 1 << 2,
            ApplicationRenderingBackendGraphicsApiRenderingHints_Triangles = 1 << 3,
            ApplicationRenderingBackendGraphicsApiRenderingHints_Default   = ApplicationRenderingBackendGraphicsApiRenderingHints_Triangles
        };

        enum ApplicationRenderingBackendShaderType_ : int
        {
            ApplicationRenderingBackendShaderType_Vertex,
            ApplicationRenderingBackendShaderType_Fragment
        };

        typedef int ApplicationRenderingBackendTextureFormat;
        typedef int ApplicationRenderingBackendTextureWrapMode;
        typedef int ApplicationRenderingBackendTextureMinFilter;
        typedef int ApplicationRenderingBackendTextureMaxFilter;

        typedef int ApplicationRenderingBackendGraphicsApiHints;
        typedef int ApplicationRenderingBackendGraphicsApiBuffers;
        typedef int ApplicationRenderingBackendGraphicsApiRenderingHints;

        typedef int ApplicationRenderingBackendShaderType;
        typedef unsigned int ApplicationRenderingBackendColor;

        // Enities
        struct ApplicationRenderingBackendTexture final
        {
            ApplicationRenderingBackendTexture(
                const unsigned int&                                _Ptr       = 0,
                const int&                                         _Width     = 128,
                const int&                                         _Height    = 128,
                const ApplicationRenderingBackendColor&            _Color     = 1, // white
                const ApplicationRenderingBackendTextureFormat&    _Format    = ApplicationRenderingBackendTextureFormat_RGBA,
                const ApplicationRenderingBackendTextureWrapMode&  _Wrap      = ApplicationRenderingBackendTextureWrapMode_::ApplicationRenderingBackendTextureWrapMode_Repeat,
                const ApplicationRenderingBackendTextureMinFilter& _MinFilter = ApplicationRenderingBackendTextureMinFilter_::ApplicationRenderingBackendTextureMinFilter_Linear,
                const ApplicationRenderingBackendTextureMaxFilter& _MaxFilter = ApplicationRenderingBackendTextureMaxFilter_::ApplicationRenderingBackendTextureMaxFilter_Linear) :
            Ptr(_Ptr),
            Width(_Width),
            Height(_Height),
            Color(_Color),
            Format(_Format),
            Wrap(_Wrap),
            MinFilter(_MinFilter),
            MaxFilter(_MaxFilter){}

            bool is_null() const
            {
                return Ptr == 0;
            }

            unsigned int                                Ptr       {+0};
            int                                         Width     {-1};
            int                                         Height    {-1};
            ApplicationRenderingBackendColor            Color     {1}; // white
            ApplicationRenderingBackendTextureFormat    Format    {ApplicationRenderingBackendTextureFormat_::ApplicationRenderingBackendTextureFormat_RGBA};
            ApplicationRenderingBackendTextureWrapMode  Wrap      {ApplicationRenderingBackendTextureWrapMode_::ApplicationRenderingBackendTextureWrapMode_Repeat};
            ApplicationRenderingBackendTextureMinFilter MinFilter {ApplicationRenderingBackendTextureMinFilter_::ApplicationRenderingBackendTextureMinFilter_Linear};
            ApplicationRenderingBackendTextureMaxFilter MaxFilter {ApplicationRenderingBackendTextureMaxFilter_::ApplicationRenderingBackendTextureMaxFilter_Linear};
        };

        struct ApplicationRenderingBackendShader final
        {
            ApplicationRenderingBackendShader(const unsigned int& _Ptr = 0) : Ptr(_Ptr){}

            bool is_null() const
            {
                return Ptr == 0;
            }

            unsigned int Ptr{0};
        };

        struct ApplicationRenderingBackendVertex final
        {
            ApplicationRenderingBackendVertex(
                const gs_vec3f&                         _Position = gs_vec3f(0),
                const gs_vec3f&                         _Normal   = gs_vec3f(0),
                const gs_vec2f&                         _UV       = gs_vec2f(0),
                const ApplicationRenderingBackendColor& _Color    = 1) :
                Position(_Position),
                Normal(_Normal),
                UV(_UV),
                Color(_Color){}

            gs_vec3f                         Position{gs_vec3f(0.f, 0.f, 0.f)};
            gs_vec3f                         Normal  {gs_vec3f(0.f, 0.f, 0.f)};
            gs_vec2f                         UV      {gs_vec3f(0.f, 0.f, 0.f)};
            ApplicationRenderingBackendColor Color   {1}; // white
        };

        struct ApplicationRenderingBackendGlyph
        {
            ApplicationRenderingBackendGlyph(
                const gs_2dboxf& _Box     = gs_2dboxf(gs_vec2f(0.f), gs_vec2f(0.f)),
                const gs_vec2f&  _MinUV   = gs_vec2f(0.f),
                const gs_vec2f&  _MaxUV   = gs_vec2f(0.f),
                const gs_vec2f&  _Bearing = gs_vec2f(0.f),
                const float&     _Advance = 0.f) :
                    Box(_Box),
                    MinUV(_MinUV),
                    MaxUV(_MaxUV),
                    Bearing(_Bearing),
                    Advance(_Advance){}

            gs_2dboxf Box    {gs_2dboxf(gs_vec2f(0.f), gs_vec2f(0.f))};
            gs_vec2f  MinUV  {gs_vec2f(0.f)};
            gs_vec2f  MaxUV  {gs_vec2f(0.f)};
            gs_vec2f  Bearing{gs_vec2f(0.f)};
            float     Advance{0.f};
        };

        struct ApplicationRenderingBackendFont
        {
            ApplicationRenderingBackendFont(
                const int&                                               _SizeInPixels = 0,
                const float&                                             _Ascent       = 0.f,
                const float&                                             _Descent      = 0.f,
                const float&                                             _LineGap      = 0.f,
                const unsigned int&                                      _UnicodeMin   = 0,
                const unsigned int&                                      _UnicodeMax   = 0,
                const std::shared_ptr<ApplicationRenderingBackendGlyph>& _Glyphs       = nullptr,
                const ApplicationRenderingBackendTexture&                _AtlasTexture = ApplicationRenderingBackendTexture()) :
                    SizeInPixels(_SizeInPixels),
                    Ascent(_Ascent),
                    Descent(_Descent),
                    LineGap(_LineGap),
                    UnicodeMin(_UnicodeMin),
                    UnicodeMax(_UnicodeMax),
                    Glyphs(_Glyphs),
                    AtlasTexture(_AtlasTexture){}

            int                                               SizeInPixels{0};
            float                                             Ascent      {0.f};
            float                                             Descent     {0.f};
            float                                             LineGap     {0.f};
            unsigned int                                      UnicodeMin  {0};
            unsigned int                                      UnicodeMax  {0};
            std::shared_ptr<ApplicationRenderingBackendGlyph> Glyphs      {nullptr};
            ApplicationRenderingBackendTexture                AtlasTexture{ApplicationRenderingBackendTexture()};

            // API
            bool is_null() const
            {
                return Glyphs == nullptr || AtlasTexture.is_null();
            }

            bool contains_glyph(const unsigned int& _UTF8Codepoint) const
            {
                return _UTF8Codepoint >= UnicodeMin &&
                       _UTF8Codepoint <= UnicodeMax;
            }

            ApplicationRenderingBackendGlyph retrieve_glyph(const unsigned int& _UTF8Codepoint) const
            {
                return Glyphs.get()[_UTF8Codepoint - UnicodeMin];
            }
        };

        class ApplicationRenderingBackend
        {
        public:

            typedef void* (*Loader)(const char*);

            struct Projections
            {
                gs_mat4f cameraview;
                gs_mat4f projection;
            };

            // API loader
            // TODO: this might differ for different rendering APIs...
            static bool awake(Loader);
            static void quit();

            static ApplicationRenderingBackendFont    get_default_font();
            static ApplicationRenderingBackendShader  get_default_shader();
            static ApplicationRenderingBackendTexture get_default_texture();
            

            // viewport API
            static void set_viewport(const gs_vec2f&, const gs_vec2f&);

            // font API
            static ApplicationRenderingBackendFont construct_font(const unsigned char* _Memory, const int& _Size);
            static ApplicationRenderingBackendFont construct_font(const void* _CompressedTTF, const unsigned int& _CompressedTTFSize, const int& _Size);
            static ApplicationRenderingBackendFont construct_font(const char* _FilePath, const int& _Size);
            static void destroy_font(const ApplicationRenderingBackendFont& _Font);

            // texture API
            static ApplicationRenderingBackendTexture construct_texture(
                const char*                           _FilePath,
                const ApplicationRenderingBackendTextureFormat&    _Format    = ApplicationRenderingBackendTextureFormat_::ApplicationRenderingBackendTextureFormat_RGBA,
                const ApplicationRenderingBackendTextureWrapMode&  _Wrap      = ApplicationRenderingBackendTextureWrapMode_::ApplicationRenderingBackendTextureWrapMode_Repeat,
                const ApplicationRenderingBackendTextureMinFilter& _MinFilter = ApplicationRenderingBackendTextureMinFilter_::ApplicationRenderingBackendTextureMinFilter_Linear, 
                const ApplicationRenderingBackendTextureMaxFilter& _MaxFilter = ApplicationRenderingBackendTextureMaxFilter_::ApplicationRenderingBackendTextureMaxFilter_Linear);

            static ApplicationRenderingBackendTexture construct_texture(
                const unsigned char*                               _RawBuffer,
                const int&                                         _Width,
                const int&                                         _Height,
                const ApplicationRenderingBackendTextureFormat&    _Format    = ApplicationRenderingBackendTextureFormat_::ApplicationRenderingBackendTextureFormat_RGBA,
                const ApplicationRenderingBackendTextureWrapMode&  _Wrap      = ApplicationRenderingBackendTextureWrapMode_::ApplicationRenderingBackendTextureWrapMode_Repeat,
                const ApplicationRenderingBackendTextureMinFilter& _MinFilter = ApplicationRenderingBackendTextureMinFilter_::ApplicationRenderingBackendTextureMinFilter_Linear, 
                const ApplicationRenderingBackendTextureMaxFilter& _MaxFilter = ApplicationRenderingBackendTextureMaxFilter_::ApplicationRenderingBackendTextureMaxFilter_Linear);

            static void begin_use_texture(const ApplicationRenderingBackendTexture& _Texture);
            static void end_use_texture();

            static void destroy_texture(const ApplicationRenderingBackendTexture& _Texture);

            // shader API
            static ApplicationRenderingBackendShader construct_shader(const std::vector<std::pair<std::string, ApplicationRenderingBackendShaderType>>& _ShaderInfos);

            static void begin_use_shader(const ApplicationRenderingBackendShader&   _Shader);
            static void set_shader_uniform(const ApplicationRenderingBackendShader& _Shader, const char* _Uniform, const bool&     _Value);
            static void set_shader_uniform(const ApplicationRenderingBackendShader& _Shader, const char* _Uniform, const int&      _Value);
            static void set_shader_uniform(const ApplicationRenderingBackendShader& _Shader, const char* _Uniform, const float&    _Value);
            static void set_shader_uniform(const ApplicationRenderingBackendShader& _Shader, const char* _Uniform, const gs_vec2f& _Value);
            static void set_shader_uniform(const ApplicationRenderingBackendShader& _Shader, const char* _Uniform, const gs_vec3f& _Value);
            static void set_shader_uniform(const ApplicationRenderingBackendShader& _Shader, const char* _Uniform, const gs_vec4f& _Value);
            static void set_shader_uniform(const ApplicationRenderingBackendShader& _Shader, const char* _Uniform, const gs_mat2f& _Value);
            static void set_shader_uniform(const ApplicationRenderingBackendShader& _Shader, const char* _Uniform, const gs_mat3f& _Value);
            static void set_shader_uniform(const ApplicationRenderingBackendShader& _Shader, const char* _Uniform, const gs_mat4f& _Value);
            static void end_use_shader();

            static void destroy_shader(const ApplicationRenderingBackendShader& _Shader);

            // mesh API
            static void construct_mesh(
                const ApplicationRenderingBackendVertex* _Vertexes,
                const int&                               _VertexesCount,
                const int&                               _VertexesOffset,
                const int*                               _Indexes,
                const int&                               _IndexesCount,
                const int&                               _IndexesOffset);

            static void begin_use_mesh(
                const ApplicationRenderingBackendVertex*                    _Vertexes,
                const int&                                                  _VertexesCount,
                const int&                                                  _VertexesOffset,
                const int*                                                  _Indexes,
                const int&                                                  _IndexesCount,
                const int&                                                  _IndexesOffset,
                const ApplicationRenderingBackendGraphicsApiRenderingHints& _MeshRenderHints = ApplicationRenderingBackendGraphicsApiRenderingHints_Default);

            static void ApplicationRenderingBackend::destroy_mesh();

            // color API
            static ApplicationRenderingBackendColor construct_rgba_color(
                const ApplicationRenderingBackendColor& _R,
                const ApplicationRenderingBackendColor& _G,
                const ApplicationRenderingBackendColor& _B,
                const ApplicationRenderingBackendColor& _A);

            static ApplicationRenderingBackendColor retrieve_red_component(const ApplicationRenderingBackendColor& _Color);
            static ApplicationRenderingBackendColor retrieve_green_component(const ApplicationRenderingBackendColor& _Color);
            static ApplicationRenderingBackendColor retrieve_blue_component(const ApplicationRenderingBackendColor& _Color);
            static ApplicationRenderingBackendColor retrieve_alpha_component(const ApplicationRenderingBackendColor& _Color);

            // camera and view projection API
            static Projections calculate_2d_camera_view_and_projection(
                const gs_vec2f& _CameraWorldPosition,
                const gs_vec3f& _CameraWorldUpAxisDirection,
                const gs_vec3f& _CameraWorldFrontAxisDirection,
                const gs_vec2f& _CameraResolution,
                const float&    _CameraRotationAngle,
                const float&    _CameraNearPlanePosition,
                const float&    _CameraFarPlanePosition);

            // rendering platoform API
            static void enable(const ApplicationRenderingBackendGraphicsApiHints&);
            static void disable(const ApplicationRenderingBackendGraphicsApiHints&);
            static void clear_color(const ApplicationRenderingBackendColor&);
            static void scissor_box(const gs_2dboxf&);
            static void clear_buffers(const ApplicationRenderingBackendGraphicsApiBuffers&);

        private:
            std::shared_ptr<>

            static ApplicationRenderingBackendFont    m_DefaultFont;
            static ApplicationRenderingBackendShader  m_DefaultShader;
            static ApplicationRenderingBackendTexture m_DefaultTexture;
        };
    }
}