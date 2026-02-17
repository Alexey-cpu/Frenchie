#pragma once

// Core
#include <FrenchieCoreMath.hpp>
#include <FrenchieCoreClock.hpp>
#include <FrenchieCoreOptional.hpp>
#include <FrenchieCoreRingBuffer.hpp>

// Application
#include <FrenchieApplicationLayer.hpp>
#include <FrenchieApplicationRenderingBackend.hpp>

namespace Frenchie
{
    namespace Application
    {
        // rendering queue
        struct RenderingQueueRenderingCommand final
        {
            RenderingQueueRenderingCommand(
                const ApplicationRenderingBackendMesh&                      _Mesh,
                const ApplicationRenderingBackendShader&                    _Shader,
                const ApplicationRenderingBackendTexture&                   _Texture,
                const gs_mat4f&                                             _Transform,
                const ApplicationRenderingBackendGraphicsApiRenderingHints& _MeshRenderingHints) :
                Mesh(_Mesh),
                Shader(_Shader),
                Texture(_Texture),
                Transform(_Transform),
                MeshRendererHints(_MeshRenderingHints){}

            ApplicationRenderingBackendMesh                      Mesh             {ApplicationRenderingBackendMesh()};
            ApplicationRenderingBackendShader                    Shader           {ApplicationRenderingBackendShader()};
            ApplicationRenderingBackendTexture                   Texture          {ApplicationRenderingBackendTexture()};
            gs_mat4f                                             Transform        {gs_mat4f(1.f)};
            ApplicationRenderingBackendGraphicsApiRenderingHints MeshRendererHints{ApplicationRenderingBackendGraphicsApiRenderingHints_::ApplicationRenderingBackendGraphicsApiRenderingHints_Default};
        };

        struct RenderingQueueRendererCommandClearColor
        {
            RenderingQueueRendererCommandClearColor(const ApplicationRenderingBackendColor&  _ClearColor) : ClearColor(_ClearColor){}
            ApplicationRenderingBackendColor ClearColor;
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

        class RenderingQueue : public Layer
        {
        public:
            RenderingQueue();
            virtual ~RenderingQueue();

            // getters
            gs_mat4f              get_projection_matrix() const;
            gs_mat4f              get_cameraview_matrix() const;
            RenderingQueueMetrics get_rendering_queue_metrics() const;

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

            // commands API
            void push_rendering_command(
                const ApplicationRenderingBackendMesh&                      _Mesh,
                const ApplicationRenderingBackendShader&                    _Shader,
                const ApplicationRenderingBackendTexture&                   _Texture,
                const gs_mat4f&                                _Transform,
                const ApplicationRenderingBackendGraphicsApiRenderingHints& _RendererHints,
                const ApplicationRenderingBackendColor&                     _ClearColor,
                const gs_2dboxf&                               _ClippinBox);

        protected:

            // rendering
            gs_mat4f                                       m_ProjectionMatrix                   {gs_mat4f(1)};
            gs_mat4f                                       m_CameraViewMatrix                   {gs_mat4f(1)};
            std::vector<RenderingQueueCommand>             m_Commands                           {std::vector<RenderingQueueCommand>()};

            // metrics measurement
            std::chrono::high_resolution_clock::time_point m_FrameRateMeasurementStartTimePoint {Frenchie::Core::tic()};            
            Frenchie::Core::RingBuffer<double, 64>         m_FrameRateMeasurementFilterBuffer   {Frenchie::Core::RingBuffer<double, 64>(0.0)};
            int                                            m_RenderedTrianglesCount             {0};
            RenderingQueueMetrics                          m_Metrics                            {RenderingQueueMetrics()};
        };
    }
}