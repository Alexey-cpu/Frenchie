#pragma once

// Core
#include <FrenchieCoreMath.hpp>
#include <FrenchieCoreClock.hpp>
#include <FrenchieCoreOptional.hpp>
#include <FrenchieCoreRingBuffer.hpp>

// Application
#include <FrenchieApplicationLayer.hpp>

// STL
#include <vector>

namespace Frenchie
{
    namespace Application
    {
        enum RenderingQueueTextureFormat_ : int
        {
            RenderingQueueTextureFormat_ALPHA,
            RenderingQueueTextureFormat_RGB,
            RenderingQueueTextureFormat_RGBA,
        };

        enum RenderingQueueTextureWrapMode_ : int
        {
            RenderingQueueTextureWrapMode_Repeat,
            RenderingQueueTextureWrapMode_Mirrored,
            RenderingQueueTextureWrapMode_ClampToEdge,
            RenderingQueueTextureWrapMode_ClampToBorder
        };

        enum RenderingQueueTextureMinFilter_ : int
        {
            RenderingQueueTextureMinFilter_Linear,
            RenderingQueueTextureMinFilter_Nearest,
            RenderingQueueTextureMinFilter_Nearest_MipMapLinear,
            RenderingQueueTextureMinFilter_NearestMipMapNearest,
            RenderingQueueTextureMinFilter_LinearMipMapLinear,
            RenderingQueueTextureMinFilter_LinearMipMapNearest,
        };

        enum RenderingQueueTextureMaxFilter_ : int
        {
            RenderingQueueTextureMaxFilter_Linear,
            RenderingQueueTextureMaxFilter_Nearest,
        };

        enum RenderingQueueGraphicsApiHints_ : int
        {
            RenderingQueueGraphicsApiHints_Blending    = 1 << 0,
            RenderingQueueGraphicsApiHints_DepthTest   = 1 << 1,
            RenderingQueueGraphicsApiHints_StencilTest = 1 << 2,
            RenderingQueueGraphicsApiHints_ScissorTest = 1 << 3,
        };

        enum RenderingQueueGraphicsApiBuffers_ : int
        {
            RenderingQueueGraphicsApiBuffers_Color   = 1 << 0,
            RenderingQueueGraphicsApiBuffers_Depth   = 1 << 1,
            RenderingQueueGraphicsApiBuffers_Stencil = 1 << 2,
        };

        enum RenderingQueueGraphicsApiRenderingHints_ : int
        {
            RenderingQueueGraphicsApiRenderingHints_Lines     = 1 << 1,
            RenderingQueueGraphicsApiRenderingHints_Points    = 1 << 2,
            RenderingQueueGraphicsApiRenderingHints_Triangles = 1 << 3,
            RenderingQueueGraphicsApiRenderingHints_Default   = RenderingQueueGraphicsApiRenderingHints_Triangles
        };

        enum RenderingQueueShaderType_ : int
        {
            RenderingQueueShaderType_Vertex,
            RenderingQueueShaderType_Fragment
        };

        typedef int RenderingQueueTextureFormat;
        typedef int RenderingQueueTextureWrapMode;
        typedef int RenderingQueueTextureMinFilter;
        typedef int RenderingQueueTextureMaxFilter;
        typedef int RenderingQueueShaderType;

        typedef int RenderingQueueGraphicsApiRenderingHints;
        typedef int RenderingQueueGraphicsApiBuffers;
        typedef int RenderingQueueGraphicsApiHints;

        typedef unsigned int RenderingQueueColor;

        // Enities
        struct RenderingQueueTexture final
        {
            RenderingQueueTexture(
                const unsigned int&                   _Ptr       = 0,
                const int&                            _Width     = 128,
                const int&                            _Height    = 128,
                const RenderingQueueColor&            _Color     = 1, // white
                const RenderingQueueTextureFormat&    _Format    = RenderingQueueTextureFormat_RGBA,
                const RenderingQueueTextureWrapMode&  _Wrap      = RenderingQueueTextureWrapMode_::RenderingQueueTextureWrapMode_Repeat,
                const RenderingQueueTextureMinFilter& _MinFilter = RenderingQueueTextureMinFilter_::RenderingQueueTextureMinFilter_Linear,
                const RenderingQueueTextureMaxFilter& _MaxFilter = RenderingQueueTextureMaxFilter_::RenderingQueueTextureMaxFilter_Linear) :
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

            unsigned int                   Ptr       {+0};
            int                            Width     {-1};
            int                            Height    {-1};
            RenderingQueueColor            Color     {1}; // white
            RenderingQueueTextureFormat    Format    {RenderingQueueTextureFormat_::RenderingQueueTextureFormat_RGBA};
            RenderingQueueTextureWrapMode  Wrap      {RenderingQueueTextureWrapMode_::RenderingQueueTextureWrapMode_Repeat};
            RenderingQueueTextureMinFilter MinFilter {RenderingQueueTextureMinFilter_::RenderingQueueTextureMinFilter_Linear};
            RenderingQueueTextureMaxFilter MaxFilter {RenderingQueueTextureMaxFilter_::RenderingQueueTextureMaxFilter_Linear};
        };

        struct RenderingQueueShader final
        {
            RenderingQueueShader(const unsigned int& _Ptr = 0) : Ptr(_Ptr){}

            bool is_null() const
            {
                return Ptr == 0;
            }

            unsigned int Ptr{0};
        };

        struct RenderingQueueVertex final
        {
            gs_vec3f            Position{gs_vec3f(0.f, 0.f, 0.f)};
            gs_vec3f            Normal  {gs_vec3f(0.f, 0.f, 0.f)};
            gs_vec2f            UV      {gs_vec3f(0.f, 0.f, 0.f)};
            RenderingQueueColor Color   {1}; // white
        };

        struct RenderingQueueMesh final
        {
            RenderingQueueMesh(
                const unsigned int& _VBO = 0,
                const unsigned int& _VAO = 0,
                const unsigned int& _EBO = 0) :
            VBO(_VBO),
            VAO(_VAO),
            EBO(_EBO){}

            bool is_null() const
            {
                return VBO == 0 || VAO == 0 || EBO == 0;
            }

            unsigned int VBO{0};
            unsigned int VAO{0};
            unsigned int EBO{0};
        };

        struct RenderingQueueGlyph
        {
            RenderingQueueGlyph(
                const gs_2dboxf& _Box    = gs_2dboxf(gs_vec2f(0.f), gs_vec2f(0.f)),
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

        struct RenderingQueueFont
        {
            RenderingQueueFont(
                const int&                                  _SizeInPixels      = 0,
                const float&                                _Ascent            = 0.f,
                const float&                                _Descent           = 0.f,
                const float&                                _LineGap           = 0.f,
                const unsigned int&                         _UnicodeMin        = 0,
                const unsigned int&                         _UnicodeMax        = 0,
                const std::shared_ptr<RenderingQueueGlyph>& _Glyphs            = nullptr,
                const RenderingQueueTexture&                _AtlasTexture      = RenderingQueueTexture()) :
                    SizeInPixels(_SizeInPixels),
                    Ascent(_Ascent),
                    Descent(_Descent),
                    LineGap(_LineGap),
                    UnicodeMin(_UnicodeMin),
                    UnicodeMax(_UnicodeMax),
                    Glyphs(_Glyphs),
                    AtlasTexture(_AtlasTexture){}

            int                                  SizeInPixels{0};
            float                                Ascent      {0.f};
            float                                Descent     {0.f};
            float                                LineGap     {0.f};
            unsigned int                         UnicodeMin  {0};
            unsigned int                         UnicodeMax  {0};
            std::shared_ptr<RenderingQueueGlyph> Glyphs      {nullptr};
            RenderingQueueTexture                AtlasTexture{RenderingQueueTexture()};

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

            RenderingQueueGlyph retrieve_glyph(const unsigned int& _UTF8Codepoint) const
            {
                return Glyphs.get()[_UTF8Codepoint - UnicodeMin];
            }
        };

        struct RenderingQueueRenderingCommand final
        {
            RenderingQueueRenderingCommand(
                const RenderingQueueMesh&                      _Mesh,
                const RenderingQueueShader&                    _Shader,
                const RenderingQueueTexture&                   _Texture,
                const gs_mat4f&                                _Transform,
                const RenderingQueueGraphicsApiRenderingHints& _MeshRenderingHints) :
                Mesh(_Mesh),
                Shader(_Shader),
                Texture(_Texture),
                Transform(_Transform),
                MeshRendererHints(_MeshRenderingHints){}

            RenderingQueueMesh                      Mesh             {RenderingQueueMesh()};
            RenderingQueueShader                    Shader           {RenderingQueueShader()};
            RenderingQueueTexture                   Texture          {RenderingQueueTexture()};
            gs_mat4f                                Transform        {gs_mat4f(1.f)};
            RenderingQueueGraphicsApiRenderingHints MeshRendererHints{RenderingQueueGraphicsApiRenderingHints_::RenderingQueueGraphicsApiRenderingHints_Default};
        };

        struct RenderingQueueRendererCommandClearColor
        {
            RenderingQueueRendererCommandClearColor(const RenderingQueueColor&  _ClearColor) : ClearColor(_ClearColor){}
            RenderingQueueColor ClearColor;
        };

        struct RenderingQueueRendererCommandClippingBox
        {
            RenderingQueueRendererCommandClippingBox(const gs_2dboxf& _ClippinBox) : ClippingBox(_ClippinBox){}

            gs_2dboxf ClippingBox;
        };

        struct RenderingQueueCommand
        {
            RenderingQueueCommand(
                const RenderingQueueRenderingCommand&           _Command,
                const RenderingQueueRendererCommandClearColor&  _ClearColor,
                const RenderingQueueRendererCommandClippingBox& _ClippingBox) :
                Command(_Command),
                ClearColor(_ClearColor),
                ClippingBox(_ClippingBox){}

            Frenchie::Core::Optional<RenderingQueueRenderingCommand>           Command;
            Frenchie::Core::Optional<RenderingQueueRendererCommandClearColor>  ClearColor;
            Frenchie::Core::Optional<RenderingQueueRendererCommandClippingBox> ClippingBox;
        };

        struct RenderingQueueMetrics
        {
            double FrameRate              = 0.0;
            int    RenderedTrianglesCount = 0;
            int    RenderingCommandsCount = 0;
        };

        class RenderingQueueGraphicsApi
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
            static bool load(Loader);

            // viewport API
            static void set_viewport(const gs_vec2f&, const gs_vec2f&);

            // texture API
            static RenderingQueueTexture construct_texture(
                const unsigned char*                  _RawBuffer,
                const int&                            _Width,
                const int&                            _Height,
                const RenderingQueueTextureFormat&    _Format    = RenderingQueueTextureFormat_::RenderingQueueTextureFormat_RGBA,
                const RenderingQueueTextureWrapMode&  _Wrap      = RenderingQueueTextureWrapMode_::RenderingQueueTextureWrapMode_Repeat,
                const RenderingQueueTextureMinFilter& _MinFilter = RenderingQueueTextureMinFilter_::RenderingQueueTextureMinFilter_Linear, 
                const RenderingQueueTextureMaxFilter& _MaxFilter = RenderingQueueTextureMaxFilter_::RenderingQueueTextureMaxFilter_Linear);

            static void begin_use_texture(const RenderingQueueTexture& _Texture);
            static void end_use_texture();

            static void destroy_texture(const RenderingQueueTexture& _Texture);

            // shader API
            static RenderingQueueShader construct_shader(const std::vector<std::pair<std::string, RenderingQueueShaderType>>& _ShaderInfos);

            static void begin_use_shader(const RenderingQueueShader&   _Shader);
            static void set_shader_uniform(const RenderingQueueShader& _Shader, const char* _Uniform, const bool&     _Value);
            static void set_shader_uniform(const RenderingQueueShader& _Shader, const char* _Uniform, const int&      _Value);
            static void set_shader_uniform(const RenderingQueueShader& _Shader, const char* _Uniform, const float&    _Value);
            static void set_shader_uniform(const RenderingQueueShader& _Shader, const char* _Uniform, const gs_vec2f& _Value);
            static void set_shader_uniform(const RenderingQueueShader& _Shader, const char* _Uniform, const gs_vec3f& _Value);
            static void set_shader_uniform(const RenderingQueueShader& _Shader, const char* _Uniform, const gs_vec4f& _Value);
            static void set_shader_uniform(const RenderingQueueShader& _Shader, const char* _Uniform, const gs_mat2f& _Value);
            static void set_shader_uniform(const RenderingQueueShader& _Shader, const char* _Uniform, const gs_mat3f& _Value);
            static void set_shader_uniform(const RenderingQueueShader& _Shader, const char* _Uniform, const gs_mat4f& _Value);
            static void end_use_shader();

            static void destroy_shader(const RenderingQueueShader& _Shader);

            // mesh API
            static RenderingQueueMesh construct_mesh(
                const RenderingQueueVertex* _Vertexes,
                const int&                  _VertexesCount,
                const int*                  _Indexes,
                const int&                  _IndexesCount);
            
            static void begin_use_mesh(
                const RenderingQueueMesh&                      _Mesh,
                const RenderingQueueGraphicsApiRenderingHints& _MeshRenderHints = RenderingQueueGraphicsApiRenderingHints_Default);

            static void end_use_mesh();

            static void destroy_mesh(const RenderingQueueMesh& _Mesh);

            // mesh vertex API
            static RenderingQueueVertex construct_vertex(
                const gs_vec3f&            _Position = gs_vec3f(0),
                const gs_vec3f&            _Normal   = gs_vec3f(0),
                const gs_vec2f&            _UV       = gs_vec2f(0),
                const RenderingQueueColor& _Color    = 1)
            {
                RenderingQueueVertex vertex;
                vertex.Position = _Position;
                vertex.Normal   = _Normal;
                vertex.UV       = _UV;
                vertex.Color    = _Color;
                return vertex;
            }

            // color API
            static RenderingQueueColor construct_rgba_color(
                const RenderingQueueColor& _R,
                const RenderingQueueColor& _G,
                const RenderingQueueColor& _B,
                const RenderingQueueColor& _A);

            static RenderingQueueColor retrieve_red_component(const RenderingQueueColor& _Color);
            static RenderingQueueColor retrieve_green_component(const RenderingQueueColor& _Color);
            static RenderingQueueColor retrieve_blue_component(const RenderingQueueColor& _Color);
            static RenderingQueueColor retrieve_alpha_component(const RenderingQueueColor& _Color);

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
            static void enable(const RenderingQueueGraphicsApiHints&);
            static void disable(const RenderingQueueGraphicsApiHints&);
            static void clear_color(const RenderingQueueColor&);
            static void scissor_box(const gs_2dboxf&);
            static void clear_buffers(const RenderingQueueGraphicsApiBuffers&);
        };

        class RenderingQueue : public Layer
        {
        public:
            RenderingQueue();
            virtual ~RenderingQueue();

            // getters
            gs_mat4f              get_projection_matrix() const;
            gs_mat4f              get_cameraview_matrix() const;
            RenderingQueueMetrics get_rendering_queue_metrics() const;

            RenderingQueueFont    get_default_font() const;
            RenderingQueueShader  get_default_shader() const;
            RenderingQueueTexture get_default_texture() const;

            // setters
            void set_projection_matrix(const gs_mat4f&);
            void set_cameraview_matrix(const gs_mat4f&);

            // Layer API
            virtual bool awake() override;
            virtual void frame_start() override;
            virtual void frame_update() override;
            virtual void frame_render() override;
            virtual void frame_finish() override;
            virtual void finish() override;
            virtual void quit() override;
            virtual bool allows_multiple_instances() const override;

            // font API
            RenderingQueueFont construct_font(const unsigned char* _Memory, const int& _Size);
            RenderingQueueFont construct_font(const void* _CompressedTTF, const unsigned int& _CompressedTTFSize, const int& _Size);
            RenderingQueueFont construct_font(const char* _FilePath, const int& _Size);
            void destroy_font(const RenderingQueueFont& _Font);

            // texture API
            RenderingQueueTexture construct_texture(
                const char*                           _FilePath,
                const RenderingQueueTextureFormat&    _Format    = RenderingQueueTextureFormat_::RenderingQueueTextureFormat_RGBA,
                const RenderingQueueTextureWrapMode&  _Wrap      = RenderingQueueTextureWrapMode_::RenderingQueueTextureWrapMode_Repeat,
                const RenderingQueueTextureMinFilter& _MinFilter = RenderingQueueTextureMinFilter_::RenderingQueueTextureMinFilter_Linear, 
                const RenderingQueueTextureMaxFilter& _MaxFilter = RenderingQueueTextureMaxFilter_::RenderingQueueTextureMaxFilter_Linear);

            void destroy_texture(const RenderingQueueTexture& _Texture);

            // shader API
            RenderingQueueShader construct_shader(const std::vector<std::pair<std::string, RenderingQueueShaderType>>& _ShaderInfos);
            void destroy_shader(const RenderingQueueShader& _Shader);

            // mesh API
            RenderingQueueMesh construct_mesh(
                const RenderingQueueVertex* _Vertexes,
                const int&                  _VertexesCount,
                const int*                  _Indexes,
                const int&                  _IndexesCount);

            void destroy_mesh(const RenderingQueueMesh& _Mesh);

            // commands API
            void push_rendering_command(
                const RenderingQueueMesh&                      _Mesh,
                const RenderingQueueShader&                    _Shader,
                const RenderingQueueTexture&                   _Texture,
                const gs_mat4f&                                _Transform,
                const RenderingQueueGraphicsApiRenderingHints& _RendererHints,
                const RenderingQueueColor&                     _ClearColor,
                const gs_2dboxf&                               _ClippinBox);

        protected:

            // rendering
            gs_mat4f                                       m_ProjectionMatrix                   {gs_mat4f(1)};
            gs_mat4f                                       m_CameraViewMatrix                   {gs_mat4f(1)};
            std::vector<RenderingQueueCommand>             m_Commands                           {std::vector<RenderingQueueCommand>()};
            RenderingQueueShader                           m_DefaultShader                      {RenderingQueueShader()};
            RenderingQueueTexture                          m_DefaultTexture                     {RenderingQueueTexture()};
            RenderingQueueFont                             m_DefaultFont                        {RenderingQueueFont()};

            // metrics measurement
            std::chrono::high_resolution_clock::time_point m_FrameRateMeasurementStartTimePoint {Frenchie::Core::tic()};            
            Frenchie::Core::RingBuffer<double, 64>         m_FrameRateMeasurementFilterBuffer   {Frenchie::Core::RingBuffer<double, 64>(0.0)};
            int                                            m_RenderedTrianglesCount             {0};
            RenderingQueueMetrics                          m_Metrics                            {RenderingQueueMetrics()};
        };
    }
}