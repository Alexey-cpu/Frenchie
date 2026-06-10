#pragma once

// Core
#include <FrenchieCoreMath.hpp>

// STL
#include <vector>
#include <memory>
#include <vector>
#include <string>
#include <any>

/*! \defgroup <Application> (Application)
*  @brief The module contains application launching instance.
    @{
*/

/*! @} */

namespace Frenchie
{
    namespace Application
    {
        /*! \defgroup <ApplicationRenderingBackend> (Application rendering backend)
        *  @ingroup Application
        *  @brief The module contains utility classes that wrap application rendering backend and graphics API state.
        *  @{
        */

        /**
         * @brief This enum encodes texture format
         * @enum ApplicationRenderingBackendTextureFormat_
         */
        enum ApplicationRenderingBackendTextureFormat_ : int
        {
            ApplicationRenderingBackendTextureFormat_ALPHA, ///< 8 bit alpha only
            ApplicationRenderingBackendTextureFormat_RGB,   ///< 8 bit RGB
            ApplicationRenderingBackendTextureFormat_RGBA,  ///< 8 bit RGBA
        };

        /**
         * @brief This enum encodes texture wrap mode
         * @enum ApplicationRenderingBackendTextureWrapMode_
         */
        enum ApplicationRenderingBackendTextureWrapMode_ : int
        {
            ApplicationRenderingBackendTextureWrapMode_Repeat,       ///< repeat
            ApplicationRenderingBackendTextureWrapMode_Mirrored,     ///< mirrored
            ApplicationRenderingBackendTextureWrapMode_ClampToEdge,  ///< clamp to edge
            ApplicationRenderingBackendTextureWrapMode_ClampToBorder ///< clamp to a borader
        };

        /**
         * @brief This enum encodes texture minimum filter
         * @enum ApplicationRenderingBackendTextureMinFilter_
         */
        enum ApplicationRenderingBackendTextureMinFilter_ : int
        {
            ApplicationRenderingBackendTextureMinFilter_Linear,               ///< linear
            ApplicationRenderingBackendTextureMinFilter_Nearest,              ///< nearest
            ApplicationRenderingBackendTextureMinFilter_NearestMipMapLinear,  ///< nearest mip map linear
            ApplicationRenderingBackendTextureMinFilter_NearestMipMapNearest, ///< nearest mip map nearest
            ApplicationRenderingBackendTextureMinFilter_LinearMipMapLinear,   ///< linear mip map linear
            ApplicationRenderingBackendTextureMinFilter_LinearMipMapNearest,  ///< linear mip map nearest
        };

        /**
         * @brief This enum encodes texture maximum filter
         * @enum ApplicationRenderingBackendTextureMaxFilter_
         */
        enum ApplicationRenderingBackendTextureMaxFilter_ : int
        {
            ApplicationRenderingBackendTextureMaxFilter_Linear,  ///< linear
            ApplicationRenderingBackendTextureMaxFilter_Nearest, ///< nearest
        };

        /**
         * @brief This enum encodes mesh rendering hints
         * @enum ApplicationRenderingBackendMeshRenderingHints_
         */
        enum ApplicationRenderingBackendMeshRenderingHints_ : int
        {
            ApplicationRenderingBackendMeshRenderingHints_Lines     = 1 << 0, ///< renders mesh as lines
            ApplicationRenderingBackendMeshRenderingHints_Triangles = 1 << 1  ///< renders mesh as triangles
        };

        typedef int ApplicationRenderingBackendTextureFormat;
        typedef int ApplicationRenderingBackendTextureWrapMode;
        typedef int ApplicationRenderingBackendTextureMinFilter;
        typedef int ApplicationRenderingBackendTextureMaxFilter;
        typedef int ApplicationRenderingBackendMeshRenderingHints;
        typedef int ApplicationRenderingBackendShaderType;

        /**
         * @brief This truct wraps texture
         * @struct ApplicationRenderingBackendTexture
         */
        struct ApplicationRenderingBackendTexture final
        {
            /**
             * @brief Initializes a new texture
             * 
             * @param _Ptr integer pointer to a texture on GPU
             * @param _Width width
             * @param _Height height
             * @param _Color mask color
             * @param _Format format
             * @param _Wrap wrap mode
             * @param _MinFilter min filter
             * @param _MaxFilter max filter
             */
            ApplicationRenderingBackendTexture(
                const uintptr_t&                                   _Ptr       = 0,
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

            /**
             * @brief Checks if texture exists on GPU
             * @return returns true if texture exists on GPU
             */
            bool is_null() const
            {
                return Ptr == 0;
            }

            mutable uintptr_t                                   Ptr       {+0};                                                                                               ///< integer pointer to a texture on GPU
            mutable int                                         Width     {-1};                                                                                               ///< width
            mutable int                                         Height    {-1};                                                                                               ///< height
            mutable gs_color                                    Color     {1 };                                                                                               ///< mask color
            mutable ApplicationRenderingBackendTextureFormat    Format    {ApplicationRenderingBackendTextureFormat_::ApplicationRenderingBackendTextureFormat_RGBA        }; ///< format
            mutable ApplicationRenderingBackendTextureWrapMode  Wrap      {ApplicationRenderingBackendTextureWrapMode_::ApplicationRenderingBackendTextureWrapMode_Repeat  }; ///< wrap mode
            mutable ApplicationRenderingBackendTextureMinFilter MinFilter {ApplicationRenderingBackendTextureMinFilter_::ApplicationRenderingBackendTextureMinFilter_Linear}; ///< min filter
            mutable ApplicationRenderingBackendTextureMaxFilter MaxFilter {ApplicationRenderingBackendTextureMaxFilter_::ApplicationRenderingBackendTextureMaxFilter_Linear}; ///< max filter
        };

        /**
         * @brief This struct encapsulates a single mesh vertex
         * @struct ApplicationRenderingBackendMeshVertex
         */
        struct ApplicationRenderingBackendMeshVertex final
        {

            /**
             * @brief Initializes mesh vertex object
             * 
             * @param _Position position 
             * @param _UV texture UV coordinate
             * @param _Color color
             */
            ApplicationRenderingBackendMeshVertex(
                const gs_vec3f& _Position = gs_vec3f(0),
                const gs_vec2f& _UV       = gs_vec2f(0),
                const gs_color& _Color    = 1)
            {
                Position[0] = _Position.x;
                Position[1] = _Position.y;
                Position[2] = _Position.z;

                UV      [0] = _UV.x;
                UV      [1] = _UV.y;

                Color = _Color;
            }

            float    Position[3]{}; ///< position
            float    UV      [2]{}; ///< texture UV coordinate
            gs_color Color   {1};   ///< color
        };

        /**
         * @brief This type represents mesh vertex index
         */
        typedef unsigned int ApplicationRenderingBackendMeshVertexIndex;

        /**
         * @brief This structs encapsulates font glyph state.
         * @struct ApplicationRenderingBackendGlyph
         */
        struct ApplicationRenderingBackendGlyph
        {
            /**
             * @brief Initializes font glyph object
             * 
             * @param _Box bounding box
             * @param _MinUV minimum UV coordiante within font atlas
             * @param _MaxUV maximum UV coordiante within font atlas
             * @param _Bearing glyph bearing
             * @param _Advance glyph advance
             */
            ApplicationRenderingBackendGlyph(
                const gs_2d_boxf& _Box    = gs_2d_boxf(gs_vec2f(0.f), gs_vec2f(0.f)),
                const gs_vec2f&   _MinUV   = gs_vec2f(0.f),
                const gs_vec2f&   _MaxUV   = gs_vec2f(0.f),
                const gs_vec2f&   _Bearing = gs_vec2f(0.f),
                const float&      _Advance = 0.f) :
                    Box(_Box),
                    MinUV(_MinUV),
                    MaxUV(_MaxUV),
                    Bearing(_Bearing),
                    Advance(_Advance){}

            gs_2d_boxf Box    {gs_2d_boxf(gs_vec2f(0.f), gs_vec2f(0.f))}; ///< bounding box
            gs_vec2f  MinUV  {gs_vec2f(0.f)};                           ///< minimum UV coordiante within font atlas
            gs_vec2f  MaxUV  {gs_vec2f(0.f)};                           ///< maximum UV coordiante within font atlas
            gs_vec2f  Bearing{gs_vec2f(0.f)};                           ///< glyph bearing
            float     Advance{0.f};                                     ///< glyph advance
        };

        /**
         * @brief This structs encapsulates font state.
         * @struct ApplicationRenderingBackendFont
         */
        struct ApplicationRenderingBackendFont
        {
            /**
             * @brief Initializes font object
             * 
             * @param _SizeInPixels font size in pixels 
             * @param _Ascent font ascent
             * @param _Descent font descent
             * @param _LineGap font line gap
             * @param _UnicodeMin starting unicode symbol
             * @param _UnicodeMax ending unicode symbol
             * @param _Glyphs array of font glyphs
             * @param _AtlasTexture font atlas texture
             */
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

            /**
             * @brief Calculates font scale relative to font size in pixels.
             * @param _Size size in pixels for which we want calculate scale relative to font size in pixels.
             * @return returns font scale relative to font size in pixels.
             */
            float get_scale(const float& _Size) const
            {
                return _Size / (float)SizeInPixels;
            }

            /**
             * @brief Calculates text starting offset.
             * 
             * @param _Size size in pixels for which we want calculate text starting offset.
             * @return returns text starting offset.
             */
            float get_offset(const float& _Size) const
            {
                return (Ascent + Descent + LineGap) * get_scale(_Size);
            }
            
            /**
             * @brief Checks whether font has been instantiated
             * @return returns true if font has been instantiated
             */
            bool is_null() const
            {
                return Glyphs == nullptr || AtlasTexture.is_null();
            }

            /**
             * @brief Checks whether font contains some Unicode symbol.
             * @param _UTF8Codepoint input UTF-8 character
             * @return returns true if font contains some Unicode symbol.
             */
            bool contains_glyph(const unsigned int& _UTF8Codepoint) const
            {
                return !is_null()                   &&
                       _UTF8Codepoint >= UnicodeMin &&
                       _UTF8Codepoint <= UnicodeMax;
            }

            /**
             * @brief Extracts glyph for a given unicode code point.
             * @param _UTF8Codepoint unciode code point. 
             * @return returns glyph for a given unicode code point.
             */
            ApplicationRenderingBackendGlyph retrieve_glyph(const unsigned int& _UTF8Codepoint) const
            {
                return Glyphs.get()[_UTF8Codepoint - UnicodeMin];
            }

            int                                               SizeInPixels{0  };                                  ///< font size in pixels
            float                                             Ascent      {0.f};                                  ///< font ascent
            float                                             Descent     {0.f};                                  ///< font descent
            float                                             LineGap     {0.f};                                  ///< font line gap
            unsigned int                                      UnicodeMin  {0  };                                  ///< starting unicode symbol
            unsigned int                                      UnicodeMax  {0  };                                  ///< ending unicode symbol
            std::shared_ptr<ApplicationRenderingBackendGlyph> Glyphs      {nullptr};                              ///< array of font glyphs
            ApplicationRenderingBackendTexture                AtlasTexture{ApplicationRenderingBackendTexture()}; ///< font atlas texture
        };

        /**
         * @brief This struct encapsulates graphics API state.
         * @struct ApplicationRenderingBackendGraphicsApi
         */
        struct ApplicationRenderingBackendGraphicsApi
        {
            ApplicationRenderingBackendGraphicsApi(){}
            virtual ~ApplicationRenderingBackendGraphicsApi(){}

            mutable ApplicationRenderingBackendFont    m_DefaultFont;                ///< default font
            mutable ApplicationRenderingBackendTexture m_DefaultTexture;             ///< default texture
            mutable bool                               m_DefaultFontLoaded   {false};
            mutable bool                               m_DefaultTextureLoaded{false};
        };

        class ApplicationRenderingBackend
        {
        public:

            /**
             * @brief This struct encapsulates camera view and projection matrixes
             * @struct Projections
             */
            struct Projections
            {
                gs_mat4f CameraView;
                gs_mat4f Projection;
            };
            
            /**
             * @brief This function loads rendering backend API
             * @param _Data data provided by platform backend that is needed to load graphics API 
             * @return returns true if graphics API has been loaded successfully
             */
            static bool awake(const std::any& _Data);

            /**
             * @brief This function starts frame rendering (clears color, depth, stencil buffers e.t.c)
             */
            static void frame_start();

            /**
             * @brief This function finishes frame rendering (passes data to GPU to render)
             */
            static void frame_finish();

            /**
             * @brief This function destroys rendering backend API
             */
            static void quit();

            /**
             * @brief returns default font.
             * @return returns default font.
             */
            static ApplicationRenderingBackendFont    get_default_font();
           
            /**
             * @brief returns default texture.
             * @return returns default texture.
             */
            static ApplicationRenderingBackendTexture get_default_texture();

            /**
             * @brief This function sets viewport position and size
             * @param _Position viewport position
             * @param _Size viewport size
             */
            static void set_viewport(const gs_vec2f& _Position, const gs_vec2f& _Size);

            /**
             * @brief This function constructs font loaded to a memory
             * @param _Memory pointer to font raw memory buffer
             * @param _SizeInPixels font size in pixels
             * @return returns constructed font object.
             */
            static ApplicationRenderingBackendFont construct_font(const void* _Memory, const int& _SizeInPixels);

            /**
             * @brief This function constructs font loaded to a memory
             * @param _CompressedTTF pointer to compressed font raw memory buffer
             * @param _CompressedTTFSize compressed font raw memory buffer size
             * @param _SizeInPixels font size in pixels
             * @return returns constructed font object.
             */
            static ApplicationRenderingBackendFont construct_font(const void* _CompressedTTF, const unsigned int& _CompressedTTFSize, const int& _SizeInPixels);

            /**
             * @brief This function loads and constructs font from TTF format
             * @param _FilePath TTF font filepath
             * @param _SizeInPixels font size in pixels
             * @return returns constructed font object.
             */
            static ApplicationRenderingBackendFont construct_font(const char* _FilePath, const int& _SizeInPixels);

            /**
             * @brief This function destroys font on GPU.
             * @param _Font font to destroy 
             */
            static void destroy_font(const ApplicationRenderingBackendFont& _Font);

            // texture API
            /**
             * @brief This function constructs texture
             * @param _FilePath JPEG, PNG, TGA, BMP, PSD, GIF, PIC, PNM texture filepath
             * @param _Format texture format
             * @param _Wrap texture wrap mode
             * @param _MinFilter texture minimum filter
             * @param _MaxFilter texture maximum filter
             * @return returns constructed texture object.
             */
            static ApplicationRenderingBackendTexture construct_texture(
                const char*                                        _FilePath,
                const ApplicationRenderingBackendTextureFormat&    _Format    = ApplicationRenderingBackendTextureFormat_::ApplicationRenderingBackendTextureFormat_RGBA,
                const ApplicationRenderingBackendTextureWrapMode&  _Wrap      = ApplicationRenderingBackendTextureWrapMode_::ApplicationRenderingBackendTextureWrapMode_Repeat,
                const ApplicationRenderingBackendTextureMinFilter& _MinFilter = ApplicationRenderingBackendTextureMinFilter_::ApplicationRenderingBackendTextureMinFilter_Linear, 
                const ApplicationRenderingBackendTextureMaxFilter& _MaxFilter = ApplicationRenderingBackendTextureMaxFilter_::ApplicationRenderingBackendTextureMaxFilter_Linear);

            /**
             * @brief This function constructs texture
             * @param _RawBuffer texture raw buffer
             * @param _Width texture width
             * @param _Height texture height
             * @param _Format texture format
             * @param _Wrap texture wrap mode
             * @param _MinFilter texture minimum filter
             * @param _MaxFilter texture maximum filter
             * @return returns constructed texture object.
             */
            static ApplicationRenderingBackendTexture construct_texture(
                const unsigned char*                               _RawBuffer,
                const int&                                         _Width,
                const int&                                         _Height,
                const ApplicationRenderingBackendTextureFormat&    _Format    = ApplicationRenderingBackendTextureFormat_::ApplicationRenderingBackendTextureFormat_RGBA,
                const ApplicationRenderingBackendTextureWrapMode&  _Wrap      = ApplicationRenderingBackendTextureWrapMode_::ApplicationRenderingBackendTextureWrapMode_Repeat,
                const ApplicationRenderingBackendTextureMinFilter& _MinFilter = ApplicationRenderingBackendTextureMinFilter_::ApplicationRenderingBackendTextureMinFilter_Linear, 
                const ApplicationRenderingBackendTextureMaxFilter& _MaxFilter = ApplicationRenderingBackendTextureMaxFilter_::ApplicationRenderingBackendTextureMaxFilter_Linear);

            /**
             * @brief This function destorys texture on GPU.
             * @param _Texture texture to destroy
             */
            static void destroy_texture(const ApplicationRenderingBackendTexture& _Texture);

            // mesh API
            /**
             * @brief This function creates mesh vertex and index buffers for GPU to prepare for rendering
             * @param _Vertexes meshes vertexes buffer
             * @param _VertexesCount meshes vertexes buffer size
             * @param _Indexes meshes indexes buffer
             * @param _IndexesCount meshes indexes buffer count
             * @return returns true if preparing for rendering succeeded. 
             */
            static bool begin_render(
                const ApplicationRenderingBackendMeshVertex*      _Vertexes,
                const ApplicationRenderingBackendMeshVertexIndex& _VertexesCount,
                const ApplicationRenderingBackendMeshVertexIndex* _Indexes,
                const ApplicationRenderingBackendMeshVertexIndex& _IndexesCount);

            /**
             * @brief This function renders indexed mesh
             * @param _SourceMeshVertex starting index of a mesh vertex within mesh vertex index buffer
             * @param _TargetMeshVertex ending index of a mesh vertex within mesh vertex index buffer
             * @param _Texture mesh texture
             * @param _MeshProjectionMatrix mesh projection matrix
             * @param _MeshRenderHints mesh rendering hints
             */
            static void render_mesh(
                const ApplicationRenderingBackendMeshVertexIndex& _SourceMeshVertex,
                const ApplicationRenderingBackendMeshVertexIndex& _TargetMeshVertex,
                const ApplicationRenderingBackendTexture&         _Texture,
                const gs_mat4f&                                   _MeshProjectionMatrix);

            // camera and view projection API
            /**
             * @brief This function calculates 2D orthographics camera projection and view matrixes
             * @param _CameraWorldPosition camera position in world space
             * @param _CameraResolution camera resolution
             * @param _CameraRotationAngle camera rotation angle
             * @param _CameraNearPlanePosition camera near plane position
             * @param _CameraFarPlanePosition camera far plane position
             * @return returns resulting camera view and orthogonal projection matrixes 
             */
            static Projections calculate_2d_camera_view_and_projection(
                const gs_vec2f& _CameraWorldPosition,
                const gs_vec2f& _CameraResolution,
                const float&    _CameraRotationAngle,
                const float&    _CameraNearPlanePosition,
                const float&    _CameraFarPlanePosition);

            /**
             * @brief This function calculates 2D transform matrix.
             * @param _Depth depth
             * @param _Position translate position
             * @param _Rotation 2D rotation XY vector
             * @param _Scale 2D scale XY vector
             * @return 
             */
            static gs_mat4f calculate_2d_transform_matrix(
                const float&    _Depth,
                const gs_vec2f& _Position = gs_vec2f(0.f, 0.f),
                const float&    _Rotation = 0.f,
                const gs_vec2f& _Scale    = gs_vec2f(1.f, 1.f));

            /**
             * @brief This function compares two objects depths.
             * @param _A first object depth
             * @param _B second object depth
             * @return returns true if first obkject depth is less than second object depth
             */
            static bool compare_objects_depths(const float& _A, const float& _B);

            /**
             * @brief This function sets renderer clear color 
             * @param _Color wanted clear color
             */
            static void clear_color(const gs_color& _Color);

            /**
             * @brief This function sets renderer scissor box
             * @param _Box wanted renderer scissors box
             */
            static void scissor_box(const gs_2d_boxf& _Box);

            /**
             * @brief This function sets renderer mesh rendering hints
             * @param _Hints renderer mesh rendering hints
             */
            static void mesh_rendering_hints(const ApplicationRenderingBackendMeshRenderingHints& _Hints);

            // other API

            /**
             * @brief This function coverts input postion vector to platform normalized device coordinates (NDC).
             * 
             * @param _Position input position vector 
             * @param _Screen context window screen size
             * @return returns input position vector converted to platform normalized device coordinates (NDC).
             */
            static gs_vec2f convert_to_NDC(const gs_vec2f& _Position, const gs_vec2f& _Screen);

            // A static member function that is also a template

            /**
             * @brief returns graphics backend state object.
             * @return returns graphics backend state object.
             */
            template <typename T = ApplicationRenderingBackendGraphicsApi>
            static std::shared_ptr<T> graphics_api()
            {
                return std::dynamic_pointer_cast<T>(m_Api);
            }

        private:
            static std::shared_ptr<ApplicationRenderingBackendGraphicsApi> m_Api;
        };

        /*! @} */
    }
}