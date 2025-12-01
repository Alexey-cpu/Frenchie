#pragma once

// Core
#include <FrenchieCoreMath.hpp>

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

        enum RenderingQueueRendererHints_ : int
        {
            RenderingQueueRendererHints_Lines,
            RenderingQueueRendererHints_Points,
            RenderingQueueRendererHints_Triangles,
            RenderingQueueRendererHints_Default = RenderingQueueRendererHints_Triangles
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
        typedef int RenderingQueueRendererHints;
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
                const gs_rectf& _Box     = gs_rectf(gs_vec2f(0.f), gs_vec2f(0.f)),
                const gs_vec2f& _MinUV   = gs_vec2f(0.f),
                const gs_vec2f& _MaxUV   = gs_vec2f(0.f),
                const gs_vec2f& _Bearing = gs_vec2f(0.f),
                const float&    _Advance = 0.f) :
                    Box(_Box),
                    MinUV(_MinUV),
                    MaxUV(_MaxUV),
                    Bearing(_Bearing),
                    Advance(_Advance){}

            gs_rectf Box    {gs_rectf(gs_vec2f(0.f), gs_vec2f(0.f))};
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

            int                                  SizeInPixels     {0};
            float                                Ascent           {0.f};
            float                                Descent          {0.f};
            float                                LineGap          {0.f};
            unsigned int                         UnicodeMin       {0};
            unsigned int                         UnicodeMax       {0};
            std::shared_ptr<RenderingQueueGlyph> Glyphs           {nullptr};
            RenderingQueueTexture                AtlasTexture     {RenderingQueueTexture()};

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

        struct RenderingQueueCommand final
        {
            RenderingQueueCommand(
                const RenderingQueueMesh&    _Mesh,
                const RenderingQueueShader&  _Shader,
                const RenderingQueueTexture& _Texture,
                const gs_mat4f&              _Transform) :
                Mesh(_Mesh),
                Transform(_Transform),
                Texture(_Texture),
                Shader(_Shader){}

            RenderingQueueMesh    Mesh     {RenderingQueueMesh()};
            RenderingQueueShader  Shader   {RenderingQueueShader()};
            RenderingQueueTexture Texture  {RenderingQueueTexture()};
            gs_mat4f              Transform{gs_mat4f(1.f)};
        };

        class RenderingQueue : public Layer
        {
        public:
            RenderingQueue();
            virtual ~RenderingQueue();

            // setters
            void set_projection_matrix(const gs_mat4f&);
            void set_cameraview_matrix(const gs_mat4f&);

            // getters
            gs_mat4f get_projection_matrix() const;
            gs_mat4f get_cameraview_matrix() const;

            // Layer API
            virtual bool awake() override;
            virtual void frame_start() override;
            virtual void frame_update() override;
            virtual void frame_render() override;
            virtual void frame_finish() override;
            virtual void finish() override;
            virtual void quit() override;
            virtual bool allows_multiple_instances() const override;

            gs_vec2f convert_to_NDC(const gs_vec2f&);

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

            void begin_use_shader(const RenderingQueueShader& _Shader);
            void set_shader_uniform(const RenderingQueueShader& _Shader, const char* _Uniform, const bool& _Value);
            void set_shader_uniform(const RenderingQueueShader& _Shader, const char* _Uniform, const int& _Value);
            void set_shader_uniform(const RenderingQueueShader& _Shader, const char* _Uniform, const float& _Value);
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
                const RenderingQueueMesh&          _Mesh,
                const RenderingQueueRendererHints& _MeshRenderHints = RenderingQueueRendererHints_Default);

            void end_use_mesh();

            void destroy_mesh(const RenderingQueueMesh& _Mesh);

            // commands API
            void push_command(
                const RenderingQueueMesh&    _Mesh,
                const RenderingQueueShader&  _Shader,
                const RenderingQueueTexture& _Texture,
                const gs_mat4f&              _Transform);

        protected:

            gs_mat4f                           m_ProjectionMatrix{gs_mat4f(1)};
            gs_mat4f                           m_CameraViewMatrix{gs_mat4f(1)};
            std::vector<RenderingQueueCommand> m_Commands        {std::vector<RenderingQueueCommand>()};
        };
    }
}