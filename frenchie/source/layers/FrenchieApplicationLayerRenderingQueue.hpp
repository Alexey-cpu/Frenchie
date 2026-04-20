#pragma once

// Core
#include <FrenchieCoreMath.hpp>
#include <FrenchieCoreClock.hpp>
#include <FrenchieCoreOptional.hpp>
#include <FrenchieCoreRingBuffer.hpp>

// Core
#include <FrenchieCoreStringUtilities.hpp>

// Application
#include <FrenchieApplicationLayer.hpp>
#include <FrenchieApplicationPlatformBackend.hpp>
#include <FrenchieApplicationRenderingBackend.hpp>

// STL
#include <iostream>

namespace Frenchie
{
    namespace Application
    {
        struct RenderingQueueMesh final
        {
            RenderingQueueMesh(
                const int _VertexesCount  = -1,
                const int _VertexesOffset = -1,
                const int _IndexesCount   = -1,
                const int _IndexesOffset  = -1) :
                VertexesCount(_VertexesCount),
                VertexesOffset(_VertexesOffset),
                IndexesCount(_IndexesCount),
                IndexesOffset(_IndexesOffset){}

            int VertexesOffset;                           
            int VertexesCount;
            int IndexesOffset;
            int IndexesCount;
        };

        struct RenderingQueueRenderingCommand final
        {
            RenderingQueueRenderingCommand(
                const RenderingQueueMesh&                                   _Mesh,
                const ApplicationRenderingBackendTexture&                   _Texture,
                const gs_mat4f&                                             _Transform,
                const ApplicationRenderingBackendGraphicsApiRenderingHints& _MeshRenderingHints) :
                Mesh(_Mesh),
                Texture(_Texture),
                Transform(_Transform),
                MeshRendererHints(_MeshRenderingHints){}

            RenderingQueueMesh                                   Mesh             {RenderingQueueMesh()};
            ApplicationRenderingBackendTexture                   Texture          {ApplicationRenderingBackendTexture()};
            gs_mat4f                                             Transform        {gs_mat4f(1.f)};
            ApplicationRenderingBackendGraphicsApiRenderingHints MeshRendererHints{ApplicationRenderingBackendGraphicsApiRenderingHints_::ApplicationRenderingBackendGraphicsApiRenderingHints_Default};
        };

        struct RenderingQueueRendererCommandClearColor final
        {
            RenderingQueueRendererCommandClearColor(
                const gs_color& _ClearColor) : ClearColor(_ClearColor){}
            gs_color ClearColor;
        };

        struct RenderingQueueRendererCommandClippingBox final
        {
            RenderingQueueRendererCommandClippingBox(
                const gs_2dboxf& _ClippinBox) : ClippingBox(_ClippinBox){}

            gs_2dboxf ClippingBox;
        };

        struct RenderingQueueCommand final
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

        struct RenderingQueueMetrics final
        {
            double FrameRate              = 0.0;
            int    RenderedTrianglesCount = 0;
            int    RenderingCommandsCount = 0;
        };

        class RenderingQueue : public Layer
        {
        public:

            // constructors
            RenderingQueue(const std::string& = STRINGIFY(RenderingQueue));

            // destructor
            virtual ~RenderingQueue();

            // getters
            RenderingQueueMetrics get_rendering_queue_metrics() const;
            float                 get_minimum_line_width() const;
            gs_vec3f              get_cursor_postion() const;
            float                 get_near_plane() const;
            float                 get_far_plane() const;

            // Layer API
            virtual bool awake() override;
            virtual void frame_start() override;
            virtual void frame_update() override;
            virtual void frame_render() override;
            virtual void frame_finish() override;
            virtual void finish() override;
            virtual void quit() override;
            virtual bool allows_multiple_instances() const override;

            // API
            void push_clip_box(const gs_2dboxf& _Value, const gs_mat4f& _Transform = gs_mat4f(1.f));
            void pop_clip_box();

            void push_clear_color(const gs_color& _Value);
            void pop_clear_color();
            
            gs_2dboxf current_viewport() const;
            gs_2dboxf current_clipping_box() const;
            gs_color  current_clear_color() const;

            gs_mat4f calculate_transform_matrix(
                const float&    _Depth,
                const gs_vec2f& _Position = gs_vec2f(0.f, 0.f),
                const float&    _Rotation = 0.f,
                const gs_vec2f& _Scale    = gs_vec2f(1.f, 1.f));

            // commands API
            void push_rendering_command(
                const gs_mat4f&                                             _Transform,
                const ApplicationRenderingBackendGraphicsApiRenderingHints& _MeshRenderingHints = ApplicationRenderingBackendGraphicsApiRenderingHints_::ApplicationRenderingBackendGraphicsApiRenderingHints_Default);

            void push_rendering_command(
                const ApplicationRenderingBackendTexture&                   _Texture,
                const gs_color&                                             _Color,
                const gs_mat4f&                                             _Transform,
                const ApplicationRenderingBackendGraphicsApiRenderingHints& _MeshRenderingHints = ApplicationRenderingBackendGraphicsApiRenderingHints_::ApplicationRenderingBackendGraphicsApiRenderingHints_Default);

            void push_rendering_command(
                const RenderingQueueMesh&                                   _Mesh,
                const ApplicationRenderingBackendTexture&                   _Texture,
                const gs_mat4f&                                             _Transform,
                const ApplicationRenderingBackendGraphicsApiRenderingHints& _RendererHints,
                const gs_color&                                             _ClearColor,
                const gs_2dboxf&                                            _ClippinBox);

        protected:

            // rendering queue data
            gs_2dboxf                                               m_Viewport                           {gs_vec2f(-gs_huge<float>(), -gs_huge<float>()), gs_vec2f(+gs_huge<float>(), +gs_huge<float>())};
            std::vector<gs_color>                                   m_ClearColors                        {std::vector<gs_color>()};
            std::vector<gs_2dboxf>                                  m_ClippingBoxes                      {std::vector<gs_2dboxf>()};
            std::vector<ApplicationRenderingBackendMeshVertex>      m_MeshVertexes                       {std::vector<ApplicationRenderingBackendMeshVertex>()};
            std::vector<ApplicationRenderingBackendMeshVertexIndex> m_MeshVertexesIndexes                {std::vector<ApplicationRenderingBackendMeshVertexIndex>()};
            float                                                   m_MinimumLineWidth                   {4.f};

            // rendering
            gs_mat4f                                                m_ProjectionMatrix                   {gs_mat4f(1)};
            gs_mat4f                                                m_CameraViewMatrix                   {gs_mat4f(1)};
            std::vector<RenderingQueueCommand>                      m_Commands                           {std::vector<RenderingQueueCommand>()};

            // metrics measurement
            Frenchie::Core::Clock::HighResolutionClockTimePoint     m_FrameRateMeasurementStartTimePoint {Frenchie::Core::Clock::tic()};
            Frenchie::Core::RingBuffer<double, 64>                  m_FrameRateMeasurementFilterBuffer   {Frenchie::Core::RingBuffer<double, 64>(0.0)};
            RenderingQueueMetrics                                   m_Metrics                            {RenderingQueueMetrics()};

            ApplicationRenderingBackendMeshVertexIndex              m_IndexesOffset                      {0};
            ApplicationRenderingBackendMeshVertexIndex              m_VertexesOffset                     {0};
        };
    }
}
