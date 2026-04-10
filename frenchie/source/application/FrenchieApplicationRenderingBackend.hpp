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

        struct ApplicationRenderingBackendMeshVertex final
        {
            ApplicationRenderingBackendMeshVertex(
                const gs_vec3f& _Position = gs_vec3f(0),
                const gs_vec3f& _Normal   = gs_vec3f(0),
                const gs_vec2f& _UV       = gs_vec2f(0),
                const gs_color& _Color    = 1)
            {
                Position[0] = _Position.x;
                Position[1] = _Position.y;
                Position[2] = _Position.z;

                Normal  [0] = _Normal.x;
                Normal  [1] = _Normal.y;
                Normal  [2] = _Normal.z;

                UV      [0] = _UV.x;
                UV      [1] = _UV.y;

                Color = _Color;
            }

            float    Position[3]{};
            float    Normal  [3]{};
            float    UV      [2]{};
            gs_color Color   {1};
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

        struct ApplicationRenderingBackendGraphicsApi
        {
            ApplicationRenderingBackendGraphicsApi(){}
            virtual ~ApplicationRenderingBackendGraphicsApi(){}

            mutable ApplicationRenderingBackendFont    m_DefaultFont;
            mutable ApplicationRenderingBackendTexture m_DefaultTexture;
        };

        class ApplicationRenderingBackend
        {
        public:

            typedef void* (*Loader)(const char*);

            struct Projections
            {
                gs_mat4f CameraView;
                gs_mat4f Projection;
            };

            // API loader
            
            // This function loads rendering backend API
            static bool awake(Loader);

            // This function destroys rendering backend API
            static void quit();

            // returns default font
            static ApplicationRenderingBackendFont    get_default_font();
           
            // returns default white pattern texture
            static ApplicationRenderingBackendTexture get_default_texture();

            // This function sets viewport position and size
            // _Position - viewport position
            // _Size     - viewport size
            static void set_viewport(const gs_vec2f& _Position, const gs_vec2f& _Size);

            // font API

            // This function constructs font loaded to a memory
            // _Memory       - pointer to font raw memory buffer
            // _SizeInPixels - font size in pixels
            static ApplicationRenderingBackendFont construct_font(const void* _Memory, const int& _SizeInPixels);

            // This function constructs font loaded to a memory in compressed form
            // _CompressedTTF     - pointer to compressed font raw memory buffer
            // _CompressedTTFSize - compressed font raw memory buffer size
            // _SizeInPixels      - font size in pixels
            static ApplicationRenderingBackendFont construct_font(const void* _CompressedTTF, const unsigned int& _CompressedTTFSize, const int& _SizeInPixels);

            // This function loads and constructs font from TTF format
            // _FilePath     - TTF font filepath
            // _SizeInPixels - font size in pixels
            static ApplicationRenderingBackendFont construct_font(const char* _FilePath, const int& _SizeInPixels);

            // This function destroys font atlas
            // _Font - font to destory
            static void destroy_font(const ApplicationRenderingBackendFont& _Font);

            // texture API

            // This function constructs texture
            // _FilePath  - JPEG, PNG, TGA, BMP, PSD, GIF, PIC, PNM texture filepath
            // _Format    - texture format
            // _Wrap      - texture wrap mode
            // _MinFilter - texture minimum filter
            // _MaxFilter - texture maximum filter
            static ApplicationRenderingBackendTexture construct_texture(
                const char*                                        _FilePath,
                const ApplicationRenderingBackendTextureFormat&    _Format    = ApplicationRenderingBackendTextureFormat_::ApplicationRenderingBackendTextureFormat_RGBA,
                const ApplicationRenderingBackendTextureWrapMode&  _Wrap      = ApplicationRenderingBackendTextureWrapMode_::ApplicationRenderingBackendTextureWrapMode_Repeat,
                const ApplicationRenderingBackendTextureMinFilter& _MinFilter = ApplicationRenderingBackendTextureMinFilter_::ApplicationRenderingBackendTextureMinFilter_Linear, 
                const ApplicationRenderingBackendTextureMaxFilter& _MaxFilter = ApplicationRenderingBackendTextureMaxFilter_::ApplicationRenderingBackendTextureMaxFilter_Linear);

            // This function constructs texture
            // _RawBuffer  - texture raw buffer
            // _Width      - texture width
            // _Height     - texture height
            // _Format     - texture format
            // _Wrap       - texture wrap mode
            // _MinFilter  - texture minimum filter
            // _MaxFilter  - texture maximum filter
            static ApplicationRenderingBackendTexture construct_texture(
                const unsigned char*                               _RawBuffer,
                const int&                                         _Width,
                const int&                                         _Height,
                const ApplicationRenderingBackendTextureFormat&    _Format    = ApplicationRenderingBackendTextureFormat_::ApplicationRenderingBackendTextureFormat_RGBA,
                const ApplicationRenderingBackendTextureWrapMode&  _Wrap      = ApplicationRenderingBackendTextureWrapMode_::ApplicationRenderingBackendTextureWrapMode_Repeat,
                const ApplicationRenderingBackendTextureMinFilter& _MinFilter = ApplicationRenderingBackendTextureMinFilter_::ApplicationRenderingBackendTextureMinFilter_Linear, 
                const ApplicationRenderingBackendTextureMaxFilter& _MaxFilter = ApplicationRenderingBackendTextureMaxFilter_::ApplicationRenderingBackendTextureMaxFilter_Linear);

            // This function destorys texture
            // _Texture - texture to destroy
            static void destroy_texture(const ApplicationRenderingBackendTexture& _Texture);

            // mesh API

            // This function tells graphics API that we are starting rendering
            static bool begin_render();

            // This function renders mesh
            // _Vertexes                - meshes vertexes buffer
            // _VertexesCount           - meshes vertexes buffer size
            // _MeshVertexesCount       - concrete mesh vertexes count
            // _MeshVertexesOffset      - concrete mesh vertexes offset
            // _Indexes                 - meshes indexes buffer
            // _IndexesCount            - meshes indexes buffer count
            // _MeshIndexesCount        - concrete mesh indexes count
            // _MeshIndexesOffset       - concrete mesh indexes offset
            // _Texture                 - mesh texture
            // _MeshProjectionMatrix    - mesh projection matrix
            // _MeshRenderHints         - mesh rendering hints
            static void render_mesh(
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
                const ApplicationRenderingBackendGraphicsApiRenderingHints& _MeshRenderHints = ApplicationRenderingBackendGraphicsApiRenderingHints_Default);

            // This function tells graphics API that we stop rendering
            static void end_render();

            // camera and view projection API

            // This function calculates 2D orthographics camera projection and view matrixes
            // _CameraWorldPosition           - camera position in world space
            // _CameraWorldUpAxisDirection    - camera up axis direction in world space
            // _CameraWorldFrontAxisDirection - camera front axis direction in world space
            // _CameraResolution              - camera resolution
            // _CameraRotationAngle           - camera rotation angle
            // _CameraNearPlanePosition       - camera near plane position
            // _CameraFarPlanePosition        - camera far plane position
            static Projections calculate_2d_camera_view_and_projection(
                const gs_vec2f& _CameraWorldPosition,
                const gs_vec3f& _CameraWorldUpAxisDirection,
                const gs_vec3f& _CameraWorldFrontAxisDirection,
                const gs_vec2f& _CameraResolution,
                const float&    _CameraRotationAngle,
                const float&    _CameraNearPlanePosition,
                const float&    _CameraFarPlanePosition);

            // rendering platoform API

            // This function sets clear color
            static void clear_color(const gs_color&);

            // This function sets scissor box
            static void scissor_box(const gs_2dboxf&);

            // A static member function that is also a template
            template <typename T = ApplicationRenderingBackendGraphicsApi>
            static std::shared_ptr<T> graphics_api()
            {
                return std::dynamic_pointer_cast<T>(m_Api);
            }

        private:
            static std::shared_ptr<ApplicationRenderingBackendGraphicsApi> m_Api;
        };
    }
}