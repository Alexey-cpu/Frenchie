#pragma once

// Core
#include <FrenchieCoreMath.hpp>
#include <FrenchieCoreClock.hpp>
#include <FrenchieCoreOptional.hpp>

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

        enum RenderingQueueMeshRenderingHints_ : int
        {
            RenderingQueueMeshRenderingHints_Lines     = 1 << 1,
            RenderingQueueMeshRenderingHints_Points    = 1 << 2,
            RenderingQueueMeshRenderingHints_Triangles = 1 << 3,
            RenderingQueueMeshRenderingHints_Default   = RenderingQueueMeshRenderingHints_Triangles
        };

        // enum RenderingQueueRendererHints_ : int
        // {
        //     RenderingQueueRendererHints_Blend,
        //     RenderingQueueRendererHints_DepthTest,
        //     RenderingQueueRendererHints_StencilTest,
        // };

        enum RenderingQueueShaderType_ : int
        {
            RenderingQueueShaderType_Vertex,
            RenderingQueueShaderType_Fragment
        };

        typedef int RenderingQueueTextureFormat;
        typedef int RenderingQueueTextureWrapMode;
        typedef int RenderingQueueTextureMinFilter;
        typedef int RenderingQueueTextureMaxFilter;
        typedef int RenderingQueueMeshRenderingHints;
        typedef int RenderingQueueShaderType;

        // Enities
        struct RenderingQueueTexture final
        {
            RenderingQueueTexture(
                const unsigned int&                   _Ptr       = 0,
                const int&                            _Width     = 128,
                const int&                            _Height    = 128,
                gs_vec4f                              _Color     = {255.f, 255.f, 255.f, 255.f},
                const RenderingQueueTextureFormat&    _Format    = RenderingQueueTextureFormat_RGBA,
                const RenderingQueueTextureWrapMode&  _Wrap      = RenderingQueueTextureWrapMode_::RenderingQueueTextureWrapMode_Repeat,
                const RenderingQueueTextureMinFilter& _MinFilter = RenderingQueueTextureMinFilter_::RenderingQueueTextureMinFilter_Linear,
                const RenderingQueueTextureMaxFilter& _MaxFilter = RenderingQueueTextureMaxFilter_::RenderingQueueTextureMaxFilter_Linear) :
            Ptr(_Ptr),
            Width(_Width),
            Height(_Height),
            Color(_Color.x / 255.f, _Color.y / 255.f, _Color.z / 255.f, _Color.w / 255.f),
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
            gs_vec4f                       Color     {255.f, 255.f, 255.f, 255.f};
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
            RenderingQueueVertex(
                const gs_vec3f& _Position = gs_vec3f(0),
                const gs_vec3f& _Normal   = gs_vec3f(0),
                const gs_vec2f& _UV       = gs_vec2f(0),
                const gs_vec4f& _Color    = gs_vec4f(1.f)) :
            Position(_Position),
            Normal(_Normal),
            UV(_UV),
            Color(_Color.x / 255.f, _Color.y / 255.f, _Color.z / 255.f, _Color.w / 255.f){}

            gs_vec3f Position;
            gs_vec3f Normal;
            gs_vec2f UV;
            gs_vec4f Color;
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
            gs_vec2f MinUV  {gs_vec2f(0.f)};
            gs_vec2f MaxUV  {gs_vec2f(0.f)};
            gs_vec2f Bearing{gs_vec2f(0.f)};
            float    Advance{0.f};
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
                const RenderingQueueMesh&               _Mesh,
                const RenderingQueueShader&             _Shader,
                const RenderingQueueTexture&            _Texture,
                const gs_mat4f&                         _Transform,
                const RenderingQueueMeshRenderingHints& _MeshRenderingHints) :
                Mesh(_Mesh),
                Transform(_Transform),
                Texture(_Texture),
                Shader(_Shader),
                MeshRendererHints(_MeshRenderingHints){}

            RenderingQueueMesh               Mesh             {RenderingQueueMesh()};
            RenderingQueueShader             Shader           {RenderingQueueShader()};
            RenderingQueueTexture            Texture          {RenderingQueueTexture()};
            gs_mat4f                         Transform        {gs_mat4f(1.f)};
            RenderingQueueMeshRenderingHints MeshRendererHints{RenderingQueueMeshRenderingHints_::RenderingQueueMeshRenderingHints_Default};
        };

        struct RenderingQueueRendererCommandClearColor
        {
            RenderingQueueRendererCommandClearColor(const gs_vec4f&  _ClearColor) : ClearColor(_ClearColor){}
            gs_vec4f ClearColor;
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

        class RenderingQueue : public Layer
        {
        public:
            RenderingQueue();
            virtual ~RenderingQueue();

            // getters
            gs_mat4f get_projection_matrix() const;
            gs_mat4f get_cameraview_matrix() const;
            double   get_measured_frame_rate() const;

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

            // image API
            RenderingQueueTexture construct_texture(
                const unsigned char*                  _RawBuffer,
                const int&                            _Width,
                const int&                            _Height,
                const RenderingQueueTextureFormat&    _Format    = RenderingQueueTextureFormat_::RenderingQueueTextureFormat_RGBA,
                const RenderingQueueTextureWrapMode&  _Wrap      = RenderingQueueTextureWrapMode_::RenderingQueueTextureWrapMode_Repeat,
                const RenderingQueueTextureMinFilter& _MinFilter = RenderingQueueTextureMinFilter_::RenderingQueueTextureMinFilter_Linear, 
                const RenderingQueueTextureMaxFilter& _MaxFilter = RenderingQueueTextureMaxFilter_::RenderingQueueTextureMaxFilter_Linear);

            RenderingQueueTexture construct_texture(
                const char*                           _FilePath,
                const RenderingQueueTextureFormat&    _Format    = RenderingQueueTextureFormat_::RenderingQueueTextureFormat_RGBA,
                const RenderingQueueTextureWrapMode&  _Wrap      = RenderingQueueTextureWrapMode_::RenderingQueueTextureWrapMode_Repeat,
                const RenderingQueueTextureMinFilter& _MinFilter = RenderingQueueTextureMinFilter_::RenderingQueueTextureMinFilter_Linear, 
                const RenderingQueueTextureMaxFilter& _MaxFilter = RenderingQueueTextureMaxFilter_::RenderingQueueTextureMaxFilter_Linear);

            void begin_use_texture(const RenderingQueueTexture& _Texture);
            void end_use_texture();

            void destroy_texture(const RenderingQueueTexture& _Texture);

            // shader API
            RenderingQueueShader construct_shader(const std::vector<std::pair<std::string, RenderingQueueShaderType>>& _ShaderInfos) const;

            void begin_use_shader(const RenderingQueueShader&   _Shader);
            void set_shader_uniform(const RenderingQueueShader& _Shader, const char* _Uniform, const bool&     _Value);
            void set_shader_uniform(const RenderingQueueShader& _Shader, const char* _Uniform, const int&      _Value);
            void set_shader_uniform(const RenderingQueueShader& _Shader, const char* _Uniform, const float&    _Value);
            void set_shader_uniform(const RenderingQueueShader& _Shader, const char* _Uniform, const gs_vec2f& _Value);
            void set_shader_uniform(const RenderingQueueShader& _Shader, const char* _Uniform, const gs_vec3f& _Value);
            void set_shader_uniform(const RenderingQueueShader& _Shader, const char* _Uniform, const gs_vec4f& _Value);
            void set_shader_uniform(const RenderingQueueShader& _Shader, const char* _Uniform, const gs_mat2f& _Value);
            void set_shader_uniform(const RenderingQueueShader& _Shader, const char* _Uniform, const gs_mat3f& _Value);
            void set_shader_uniform(const RenderingQueueShader& _Shader, const char* _Uniform, const gs_mat4f& _Value);
            void end_use_shader();

            void destroy_shader(const RenderingQueueShader& _Shader);

            // mesh API
            RenderingQueueMesh construct_mesh(
                const RenderingQueueVertex* _Vertexes,
                const int&                  _VertexesCount,
                const int*                  _Indexes,
                const int&                  _IndexesCount);
            
            void begin_use_mesh(
                const RenderingQueueMesh&               _Mesh,
                const RenderingQueueMeshRenderingHints& _MeshRenderHints = RenderingQueueMeshRenderingHints_Default);

            void end_use_mesh();

            void destroy_mesh(const RenderingQueueMesh& _Mesh);

            // commands API
            void push_rendering_command(
                const RenderingQueueMesh&               _Mesh,
                const RenderingQueueShader&             _Shader,
                const RenderingQueueTexture&            _Texture,
                const gs_mat4f&                         _Transform,
                const RenderingQueueMeshRenderingHints& _RendererHints,
                const gs_vec4f&                         _ClearColor,
                const gs_2dboxf&                        _ClippinBox);

        protected:

            gs_mat4f                                       m_ProjectionMatrix                   {gs_mat4f(1)};
            gs_mat4f                                       m_CameraViewMatrix                   {gs_mat4f(1)};
            std::vector<RenderingQueueCommand>             m_Commands                           {std::vector<RenderingQueueCommand>()};
            RenderingQueueShader                           m_DefaultShader                      {RenderingQueueShader()};
            RenderingQueueTexture                          m_DefaultTexture                     {RenderingQueueTexture()};
            RenderingQueueFont                             m_DefaultFont                        {RenderingQueueFont()};
            std::chrono::high_resolution_clock::time_point m_FrameRateMeasurementStartTimePoint {Frenchie::Core::tic()};
            int                                            m_FrameRateMeasurementFramesInterval {60};
            int                                            m_FrameRateMeasurementFramesCount    {0 };
            double                                         m_FrameRate                          {0.0};
        };
    }
}