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
        // Texture foramt enum
        enum ApplicationRenderingBackendTextureFormat_ : int
        {
            ApplicationRenderingBackendTextureFormat_ALPHA, // 8 bit alpha only
            ApplicationRenderingBackendTextureFormat_RGB,   // 8 bit RGB
            ApplicationRenderingBackendTextureFormat_RGBA,  // 8 bit RGBA
        };

        // Texture wrap mode enum
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

        enum ApplicationRenderingBackendGraphicsApiRenderingHints_ : int
        {
            ApplicationRenderingBackendGraphicsApiRenderingHints_Lines     = 1 << 0,
            ApplicationRenderingBackendGraphicsApiRenderingHints_Triangles = 1 << 1,
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

        typedef int ApplicationRenderingBackendGraphicsApiRenderingHints;

        typedef int ApplicationRenderingBackendShaderType;

        // Enities
        struct ApplicationRenderingBackendGraphicsApiState;

        struct ApplicationRenderingBackendTexture final
        {
            ApplicationRenderingBackendTexture(
                const unsigned int&                                _Ptr       = 0,
                const int&                                         _Width     = 128,
                const int&                                         _Height    = 128,
                const gs_color&                                    _Color     = 1, // white
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
            gs_color                                    Color     {1}; // white
            ApplicationRenderingBackendTextureFormat    Format    {ApplicationRenderingBackendTextureFormat_::ApplicationRenderingBackendTextureFormat_RGBA};
            ApplicationRenderingBackendTextureWrapMode  Wrap      {ApplicationRenderingBackendTextureWrapMode_::ApplicationRenderingBackendTextureWrapMode_Repeat};
            ApplicationRenderingBackendTextureMinFilter MinFilter {ApplicationRenderingBackendTextureMinFilter_::ApplicationRenderingBackendTextureMinFilter_Linear};
            ApplicationRenderingBackendTextureMaxFilter MaxFilter {ApplicationRenderingBackendTextureMaxFilter_::ApplicationRenderingBackendTextureMaxFilter_Linear};
        };

        struct ApplicationRenderingBackendVertex final
        {
            ApplicationRenderingBackendVertex(
                const gs_vec3f&                         _Position = gs_vec3f(0),
                const gs_vec3f&                         _Normal   = gs_vec3f(0),
                const gs_vec2f&                         _UV       = gs_vec2f(0),
                const gs_color& _Color    = 1) :
                Position(_Position),
                Normal(_Normal),
                UV(_UV),
                Color(_Color){}

            gs_vec3f                         Position{gs_vec3f(0.f, 0.f, 0.f)};
            gs_vec3f                         Normal  {gs_vec3f(0.f, 0.f, 0.f)};
            gs_vec2f                         UV      {gs_vec3f(0.f, 0.f, 0.f)};
            gs_color Color   {1}; // white
        };

        typedef unsigned int ApplicationRenderingBackendMeshVertexIndex;

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

            float get_scale(const float& _Size) const
            {
                return _Size / (float)SizeInPixels;
            }

            float get_offset(const float& _Size) const
            {
                return (Ascent + Descent + LineGap) * get_scale(_Size);
            }
            
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
            static ApplicationRenderingBackendTexture get_default_texture();

            // viewport API
            static void set_viewport(const gs_vec2f&, const gs_vec2f&);

            // font API
            static ApplicationRenderingBackendFont construct_font(const void* _Memory, const int& _Size);
            static ApplicationRenderingBackendFont construct_font(const void* _CompressedTTF, const unsigned int& _CompressedTTFSize, const int& _Size);
            static ApplicationRenderingBackendFont construct_font(const char* _FilePath, const int& _Size);
            static void destroy_font(const ApplicationRenderingBackendFont& _Font);

            // texture API
            static ApplicationRenderingBackendTexture construct_texture(
                const char*                                        _FilePath,
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

            static void destroy_texture(const ApplicationRenderingBackendTexture& _Texture);

            // mesh API
            static bool begin_render();

            static void render_mesh(
                const ApplicationRenderingBackendVertex*                    _Vertexes,
                const ApplicationRenderingBackendMeshVertexIndex&           _VertexesCount,
                const ApplicationRenderingBackendMeshVertexIndex&           _MeshVertexesCount,
                const ApplicationRenderingBackendMeshVertexIndex&           _MeshVertexesOffset,
                const ApplicationRenderingBackendMeshVertexIndex*           _Indexes,
                const ApplicationRenderingBackendMeshVertexIndex&           _IndexesCount,
                const ApplicationRenderingBackendMeshVertexIndex&           _MeshIndexesCount,
                const ApplicationRenderingBackendMeshVertexIndex&           _MeshIndexesOffset,
                const ApplicationRenderingBackendTexture&                   _Texture,
                const gs_mat4f&                                             _MeshProjectionMatrix,
                const ApplicationRenderingBackendGraphicsApiRenderingHints& _MeshRenderHints = ApplicationRenderingBackendGraphicsApiRenderingHints_Default);

            static void end_render();

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
            static void clear_color(const gs_color&);
            static void scissor_box(const gs_2dboxf&);

        private:
            static ApplicationRenderingBackendFont                              m_DefaultFont;
            static ApplicationRenderingBackendTexture                           m_DefaultTexture;
            static std::shared_ptr<ApplicationRenderingBackendGraphicsApiState> m_GraphicsApiState;
        };
    }
}