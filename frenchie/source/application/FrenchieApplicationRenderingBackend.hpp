#pragma once

// Core
#include <FrenchieCoreMath.hpp>

// STL
#include <optional>
#include <vector>
#include <memory>
#include <string>
#include <cstdint>
#include <cstddef>
#include <limits>
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
            ApplicationRenderingBackendTextureFormat_ALPHA = 1, ///< 8 bit alpha only
            ApplicationRenderingBackendTextureFormat_RGB   = 3, ///< 8 bit RGB
            ApplicationRenderingBackendTextureFormat_RGBA  = 4, ///< 8 bit RGBA
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
         * @brief This enum encodes texture maximum filter
         * @enum ApplicationRenderingBackendTextureFilter_
         */
        enum ApplicationRenderingBackendTextureFilter_ : int
        {
            ApplicationRenderingBackendTextureFilter_Linear,  ///< linear
            ApplicationRenderingBackendTextureFilter_Nearest, ///< nearest
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
        typedef int ApplicationRenderingBackendTextureFilter;
        typedef int ApplicationRenderingBackendMeshRenderingHints;
        typedef int ApplicationRenderingBackendShaderType;

        struct ApplicationRenderingBackendRegistry final
        {
            using Index  = uint32_t;
            using Packed = uint64_t;
            using Native = uintptr_t;

            class Handle final
            {
            public:
                Handle(const Handle& _Other) : Meta((static_cast<uint64_t>(_Other.version()) << 32) | _Other.identifier()){}
                Handle(Index _Version = Handle::null(), Index _Index = Handle::null()) : Meta((static_cast<uint64_t>(_Version) << 32) | _Index){}
                Index version()    const { return static_cast<Index>(Meta >> 32); }
                Index identifier() const { return static_cast<Index>(Meta);       }
                bool  operator==(const Handle& _Other) const { return _Other.Meta == Meta; }
                bool  operator!=(const Handle& _Other) const { return _Other.Meta != Meta; }
                explicit operator Index() const noexcept {return static_cast<Index>(identifier());}
                static Index null(){return std::numeric_limits<Index>::max();}
            private:
                Packed Meta{0};
            };

            class Slot final
            {
            public:
                Slot(Index _Version, Index _Next, Native _Pointer) : Meta((static_cast<uint64_t>(_Version) << 32) | _Next), Pointer(_Pointer){}
                Index  version() const { return static_cast<Index>(Meta >> 32); }
                Index  next()    const { return static_cast<Index>(Meta);       }
                Native pointer() const { return Pointer;                        }
                bool   alive()   const { return pointer() != 0;                 }
            private:
                Packed Meta   {0};
                Native Pointer{0};
            };

            bool is_null(const Handle& _Entity) const
            {
                return !is_not_null(_Entity);
            }

            bool is_not_null(const Handle& _Entity) const
            {
                const Index id = static_cast<Index>(_Entity);
                return id < m_Slots.size() && m_Slots[id].alive() && m_Slots[id].version() == _Entity.version();
            }

            template<typename Type>
            Handle create(Type* _Pointer)
            {
                if (_Pointer == nullptr)
                    return Handle();

                if(m_Next == Handle::null())
                {
                    if(m_Slots.size() >= std::numeric_limits<Index>::max())
                        return Handle();
                    m_Slots.push_back(Slot(0, m_Next, reinterpret_cast<Native>(_Pointer)));
                    return Handle(m_Slots.back().version(), m_Slots.size() - 1);
                }

                Index index = m_Next;
                m_Next = m_Slots[index].next();
                m_Slots[index] = Slot(m_Slots[index].version(), Handle::null(), reinterpret_cast<Native>(_Pointer));
                
                return Handle(m_Slots[index].version(), index);
            }

            void remove(const Handle& _Entity)
            {
                if(is_null(_Entity)) return;

                const Index id = static_cast<Index>(_Entity);
                const Index version = m_Slots[id].version();
                m_Slots[id] = Slot(
                    version == std::numeric_limits<Index>::max() ? 0 : version + 1,
                    m_Next,
                    0);
                m_Next = id;
            }

            template<typename Type>
            Type* cast(const Handle& _Entity) const
            {
                return
                    is_not_null(_Entity) ?
                        reinterpret_cast<Type*>(m_Slots[static_cast<Index>(_Entity)].pointer()) :
                            nullptr;
            }

            template<typename Callback>
            void each(const Callback& _Callback)
            {
                for (size_t i = 0; i < m_Slots.size(); i++)
                {
                    if(auto& slot = m_Slots[i]; slot.alive())
                    {
                        _Callback(Handle(slot.version(), i));
                    }
                    else
                    {
                    }
                }
            }

        private:
            std::vector<Slot> m_Slots;
            Index             m_Next {Handle::null()};
        };

        /**
         * @brief This struct wraps texture
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
             * @param _Filter filter
             */
            ApplicationRenderingBackendTexture(
                const ApplicationRenderingBackendRegistry::Handle& _Ptr        = ApplicationRenderingBackendRegistry::Handle(),
                const int                                            _Width      = 128,
                const int                                            _Height     = 128,
                const gs_color                                       _Color      = 1,
                const ApplicationRenderingBackendTextureFormat       _Format     = ApplicationRenderingBackendTextureFormat_::ApplicationRenderingBackendTextureFormat_RGBA,
                const ApplicationRenderingBackendTextureWrapMode     _Wrap       = ApplicationRenderingBackendTextureWrapMode_::ApplicationRenderingBackendTextureWrapMode_ClampToEdge,
                const ApplicationRenderingBackendTextureFilter       _Filter     = ApplicationRenderingBackendTextureFilter_::ApplicationRenderingBackendTextureFilter_Linear,
                const int                                            _Attributes = 0);

            bool is_null() const;
            bool is_not_null() const;
            bool operator==(const ApplicationRenderingBackendTexture& _Other) const;
            bool operator!=(const ApplicationRenderingBackendTexture& _Other) const;

            mutable ApplicationRenderingBackendRegistry::Handle Ptr       {ApplicationRenderingBackendRegistry::Handle()};                                                  ///< integer pointer to a texture on GPU
            mutable int                                           Width     {-1};                                                                                               ///< width
            mutable int                                           Height    {-1};                                                                                               ///< height
            mutable gs_color                                      Color     {1 };                                                                                               ///< mask color
            mutable ApplicationRenderingBackendTextureFormat      Format    {ApplicationRenderingBackendTextureFormat_::ApplicationRenderingBackendTextureFormat_RGBA        }; ///< format
            mutable ApplicationRenderingBackendTextureWrapMode    Wrap      {ApplicationRenderingBackendTextureWrapMode_::ApplicationRenderingBackendTextureWrapMode_Repeat  }; ///< wrap mode
            mutable ApplicationRenderingBackendTextureFilter      Filter    {ApplicationRenderingBackendTextureFilter_::ApplicationRenderingBackendTextureFilter_Linear};       ///< min filter
            mutable int                                           Attributes{0};                                                                                                ///< payloads
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
            ApplicationRenderingBackendMeshVertex(const gs_vec3f& _Position = gs_vec3f(0), const gs_vec2f& _UV = gs_vec2f(0), const gs_color _Color = 1);

            float    Position[3]{}; ///< position
            float    UV      [2]{}; ///< texture UV coordinate
            gs_color Color   {1};   ///< color
        };

        /**
         * @brief This type represents mesh vertex index
         */
        typedef unsigned int ApplicationRenderingBackendMeshVertexIndex;

        /**
         * @brief This struct encapsulates font glyph state.
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
                const float       _Advance = 0.f);

            gs_2d_boxf Box    {gs_2d_boxf(gs_vec2f(0.f), gs_vec2f(0.f))}; ///< bounding box
            gs_vec2f   MinUV  {gs_vec2f(0.f)};                            ///< minimum UV coordiante within font atlas
            gs_vec2f   MaxUV  {gs_vec2f(0.f)};                            ///< maximum UV coordiante within font atlas
            gs_vec2f   Bearing{gs_vec2f(0.f)};                            ///< glyph bearing
            float      Advance{0.f};                                      ///< glyph advance
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
                const int                                                _SizeInPixels = 0,
                const float                                              _Ascent       = 0.f,
                const float                                              _Descent      = 0.f,
                const float                                              _LineGap      = 0.f,
                const unsigned int                                       _UnicodeMin   = 0,
                const unsigned int                                       _UnicodeMax   = 0,
                const std::shared_ptr<ApplicationRenderingBackendGlyph>& _Glyphs       = nullptr,
                const ApplicationRenderingBackendTexture&                _AtlasTexture = ApplicationRenderingBackendTexture());

            /**
             * @brief Calculates font scale relative to font size in pixels.
             * @param _Size size in pixels for which we want calculate scale relative to font size in pixels.
             * @return returns font scale relative to font size in pixels.
             */
            float get_scale(const float _Size) const;

            /**
             * @brief Calculates text starting offset.
             * 
             * @param _Size size in pixels for which we want calculate text starting offset.
             * @return returns text starting offset.
             */
            float get_offset(const float _Size) const;
            
            /**
             * @brief Checks whether font contains some Unicode symbol.
             * @param _UTF8Codepoint input UTF-8 character
             * @return returns true if font contains some Unicode symbol.
             */
            bool contains_glyph(const unsigned int _UTF8Codepoint) const;

            /**
             * @brief Extracts glyph for a given unicode code point.
             * @param _UTF8Codepoint unciode code point. 
             * @return returns glyph for a given unicode code point.
             */
            ApplicationRenderingBackendGlyph retrieve_glyph(const unsigned int _UTF8Codepoint) const;

            /**
             * @brief Checks whether font is null
             * @return returns true if font is null
             */
            bool is_null() const;

            /**
             * @brief Checks whether font is not null
             * @return returns true if font is not null
             */
            bool is_not_null() const;

            bool operator==(const ApplicationRenderingBackendFont& _Other) const;
            bool operator!=(const ApplicationRenderingBackendFont& _Other) const;

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
         * @brief This structs encapsulates font rendering backend rendering target state.
         * @struct ApplicationRenderingBackendRenderingTarget
         */
        struct ApplicationRenderingBackendRenderingTarget final
        {
            ApplicationRenderingBackendRenderingTarget(){}
            mutable std::optional<ApplicationRenderingBackendTexture> FrameBufferTexture {std::optional<ApplicationRenderingBackendTexture>()}; ///< framebuffer texture
        };

        /**
         * @brief This struct encapsulates graphics API state.
         * @struct ApplicationRenderingBackendGraphicsApi
         */
        struct ApplicationRenderingBackendGraphicsApi
        {
            ApplicationRenderingBackendGraphicsApi(){}
            virtual ~ApplicationRenderingBackendGraphicsApi(){}

            mutable std::optional<ApplicationRenderingBackendFont>    m_DefaultFont     {std::optional<ApplicationRenderingBackendFont>()};    ///< default font
            mutable std::optional<ApplicationRenderingBackendTexture> m_DefaultTexture  {std::optional<ApplicationRenderingBackendTexture>()}; ///< default texture
            mutable ApplicationRenderingBackendRenderingTarget*       m_RenderingTarget {nullptr};                                             ///< rendering target
            mutable ApplicationRenderingBackendRegistry               m_Registry;
        };

        class ApplicationRenderingBackend
        {
        public:

            ApplicationRenderingBackend() = delete;
            ApplicationRenderingBackend(const ApplicationRenderingBackend&) = delete;
            ApplicationRenderingBackend& operator=(const ApplicationRenderingBackend&) = delete;

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
             * @brief This function constructs rendering target object
             * @returns function returns rendering target object that can be used to render into a texture
             */
            static ApplicationRenderingBackendRenderingTarget construct_rendering_target();

            /**
             * @brief This destroys rendering target object
             * @param _Target rendering target to destroy
             */
            static void destroy_rendering_target(const ApplicationRenderingBackendRenderingTarget& _Target);

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
             * @param _Filter texture filter
             * @return returns constructed texture object.
             */
            static ApplicationRenderingBackendTexture construct_texture(
                const char*                                        _FilePath,
                const ApplicationRenderingBackendTextureFormat&    _Format     = ApplicationRenderingBackendTextureFormat_::ApplicationRenderingBackendTextureFormat_RGBA,
                const ApplicationRenderingBackendTextureWrapMode&  _Wrap       = ApplicationRenderingBackendTextureWrapMode_::ApplicationRenderingBackendTextureWrapMode_Repeat,
                const ApplicationRenderingBackendTextureFilter&    _Filter     = ApplicationRenderingBackendTextureFilter_::ApplicationRenderingBackendTextureFilter_Linear,
                const int&                                         _Attributes = 0);

            /**
             * @brief This function constructs texture
             * @param _RawBuffer texture raw buffer
             * @param _Width texture width
             * @param _Height texture height
             * @param _Format texture format
             * @param _Wrap texture wrap mode
             * @param _Filter texture filter
             * @return returns constructed texture object.
             */
            static ApplicationRenderingBackendTexture construct_texture(
                const unsigned char*                               _RawBuffer,
                const int&                                         _Width,
                const int&                                         _Height,
                const ApplicationRenderingBackendTextureFormat&    _Format     = ApplicationRenderingBackendTextureFormat_::ApplicationRenderingBackendTextureFormat_RGBA,
                const ApplicationRenderingBackendTextureWrapMode&  _Wrap       = ApplicationRenderingBackendTextureWrapMode_::ApplicationRenderingBackendTextureWrapMode_Repeat,
                const ApplicationRenderingBackendTextureFilter&    _Filter     = ApplicationRenderingBackendTextureFilter_::ApplicationRenderingBackendTextureFilter_Linear,
                const int&                                         _Attributes = 0);

            /**
             * @brief This function destorys texture on GPU.
             * @param _Texture texture to destroy
             */
            static void destroy_texture(const ApplicationRenderingBackendTexture& _Texture);

            /**
             * @brief This function prepares renderer
             * @param _ToTexture if true, then rendering is done into texture
             */
            static void begin_render(ApplicationRenderingBackendRenderingTarget* _Target);

            /**
             * @brief This function restores renderer state
             */
            static void end_render();

            // mesh API
            /**
             * @brief This function loads mesh on GPU
             * @param _Vertexes meshes vertexes buffer
             * @param _VertexesCount meshes vertexes buffer size
             * @param _Indexes meshes indexes buffer
             * @param _IndexesCount meshes indexes buffer count
             * @return returns true if mesh load succeeded. 
             */
            static bool load_mesh(
                const ApplicationRenderingBackendMeshVertex*      _Vertexes,
                const ApplicationRenderingBackendMeshVertexIndex& _VertexesCount,
                const ApplicationRenderingBackendMeshVertexIndex* _Indexes,
                const ApplicationRenderingBackendMeshVertexIndex& _IndexesCount);

            /**
             * @brief This function renders indexed mesh
             * @param _SourceMeshVertex starting index of a mesh vertex within mesh vertex index buffer
             * @param _TargetMeshVertex ending index of a mesh vertex within mesh vertex index buffer
             * @param _Texture mesh texture
             * @param _Model mesh model matrix
             * @param _View camera view matrix
             * @param _Projection world projection matrix
             */
            static void render_mesh(
                const ApplicationRenderingBackendMeshVertexIndex& _SourceMeshVertex,
                const ApplicationRenderingBackendMeshVertexIndex& _TargetMeshVertex,
                const ApplicationRenderingBackendTexture&         _Texture,
                const gs_mat4f&                                   _Model,
                const gs_mat4f&                                   _View,
                const gs_mat4f&                                   _Projection);

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
             * @brief This function calculates object depth.
             * @param _Depth input depth
             * @return returns object depth
             */
            static float calculate_object_depth(const float& _Depth);

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

            static bool is_null(const ApplicationRenderingBackendRegistry::Handle& _Entity)
            {
                return graphics_api() && graphics_api()->m_Registry.is_null(_Entity);
            }

            static bool is_not_null(const ApplicationRenderingBackendRegistry::Handle& _Entity)
            {
                return graphics_api() && graphics_api()->m_Registry.is_not_null(_Entity);
            }

        private:
            static std::shared_ptr<ApplicationRenderingBackendGraphicsApi> m_Api;
        };

        /*! @} */
    }
}