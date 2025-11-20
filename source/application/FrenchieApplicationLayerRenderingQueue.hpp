#pragma once

// Core
#include <FrenchieCoreMath.hpp>

// Application
#include <FrenchieApplicationLayer.hpp>


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
                const gs_vec2f& _UV       = gs_vec2f(0)) :
            Position(_Position),
            Normal(_Normal),
            UV(_UV){}

            gs_vec3f Position;
            gs_vec3f Normal;
            gs_vec2f UV;
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

            virtual bool awake() override;
            virtual void frame_start() override;
            virtual void frame_update() override;
            virtual void frame_render() override;
            virtual void frame_finish() override;
            virtual void finish() override;
            virtual void quit() override;
            virtual bool allows_multiple_instances() const override;

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
                const gs_mat4f&              _Transform)
            {
                m_Commands.push_back(RenderingQueueCommand(_Mesh, _Shader, _Texture, _Transform));
            }

            // 2D graphics API
            void PushTriangle2D(const gs_vec3f& _P1, const gs_vec3f& _P2, const gs_vec3f& _P3, const gs_vec4f& _Color, const gs_mat4f& _Transform)
            {
                m_Vertexes.push_back(RenderingQueueVertex(_P1, gs_vec3f(0.f), gs_vec2f(_P1[0] / m_DefaultTexture.Width, _P1[1] / m_DefaultTexture.Height)));
                m_Vertexes.push_back(RenderingQueueVertex(_P2, gs_vec3f(0.f), gs_vec2f(_P2[0] / m_DefaultTexture.Width, _P2[1] / m_DefaultTexture.Height)));
                m_Vertexes.push_back(RenderingQueueVertex(_P3, gs_vec3f(0.f), gs_vec2f(_P3[0] / m_DefaultTexture.Width, _P3[1] / m_DefaultTexture.Height)));
                m_Indexes.push_back(0);
                m_Indexes.push_back(1);
                m_Indexes.push_back(2);

                push_command(
                    // construct mesh
                    construct_mesh(
                        &m_Vertexes[0],
                        (int)m_Vertexes.size(),
                        &m_Indexes[0],
                        (int)m_Indexes.size()),
                    
                    // provide default shader
                    m_DefaultShader,

                    // setup texture
                    RenderingQueueTexture(
                        m_DefaultTexture.Ptr,
                        m_DefaultTexture.Width,
                        m_DefaultTexture.Height,
                        _Color,
                        m_DefaultTexture.Format,
                        m_DefaultTexture.Wrap,
                        m_DefaultTexture.MinFilter,
                        m_DefaultTexture.MaxFilter),
                    _Transform);

                // clean-up
                m_Indexes.clear();
                m_Vertexes.clear();
            }

        protected:

            gs_mat4f                           m_ProjectionMatrix{gs_mat4f(1)};
            gs_mat4f                           m_CameraViewMatrix{gs_mat4f(1)};
            std::vector<RenderingQueueVertex>  m_Vertexes        {std::vector<RenderingQueueVertex>()};
            std::vector<int>                   m_Indexes         {std::vector<int>()};
            std::vector<RenderingQueueCommand> m_Commands        {std::vector<RenderingQueueCommand>()};
            RenderingQueueShader               m_DefaultShader   {RenderingQueueShader()};
            RenderingQueueTexture              m_DefaultTexture  {RenderingQueueTexture()};
        };
    }
}