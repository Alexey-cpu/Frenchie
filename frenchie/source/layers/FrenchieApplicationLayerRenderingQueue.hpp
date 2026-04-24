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

/*! \defgroup <ApplicationLayers> (Application layers)
*  @brief The module contains main application layers.
    @{
*/

/*! @} */

namespace Frenchie
{
    namespace Application
    {
        /*! \defgroup <ApplicationRenderingQueue> (Application rendering queue)
        *  @ingroup ApplicationLayers
        *  @brief The module contains utility instances used to load stuff on GPU for rendering through rendering backend.
        *  @{
        */

        /**
         * @brief This struct encapsulates mesh data stored within rendering queue vertexes and vertexes indexes buffers
         * @struct RenderingQueueMesh
         */
        struct RenderingQueueMesh final
        {
            /**
             * @brief Initializes a new rendering queue mesh object
             * 
             * @param _VertexesCount number of vertexes within mesh 
             * @param _VertexesOffset offset of vertexes of this mesh within rendering queue mesh vertexes buffer
             * @param _IndexesCount number of vertexes indexes within mesh
             * @param _IndexesOffset offset of vertexes indexes of this mesh within rendering queue mesh vertexes indexes buffer
             */
            RenderingQueueMesh(
                const int _VertexesCount  = -1,
                const int _VertexesOffset = -1,
                const int _IndexesCount   = -1,
                const int _IndexesOffset  = -1) :
                VertexesCount(_VertexesCount),
                VertexesOffset(_VertexesOffset),
                IndexesCount(_IndexesCount),
                IndexesOffset(_IndexesOffset){}

            int VertexesOffset; ///< number of vertexes within mesh
            int VertexesCount;  ///< offset of vertexes of this mesh within rendering queue mesh vertexes buffer
            int IndexesOffset;  ///< number of vertexes indexes within mesh
            int IndexesCount;   ///< offset of vertexes indexes of this mesh within rendering queue mesh vertexes indexes buffer
        };

        /**
         * @brief This struct encapsulates mesh rendering command.
         * @struct RenderingQueueRenderingCommand
         */
        struct RenderingQueueRenderingCommand final
        {
            /**
             * @brief Initializes mesh rendering command
             * 
             * @param _Mesh mesh to render 
             * @param _Texture mesh texture
             * @param _Transform mesh vertexes transform matrix
             * @param _MeshRenderingHints mesh rendering hints
             */
            RenderingQueueRenderingCommand(
                const RenderingQueueMesh&                                   _Mesh,
                const ApplicationRenderingBackendTexture&                   _Texture,
                const gs_mat4f&                                             _Transform,
                const ApplicationRenderingBackendGraphicsApiRenderingHints& _MeshRenderingHints) :
                Mesh(_Mesh),
                Texture(_Texture),
                Transform(_Transform),
                MeshRendererHints(_MeshRenderingHints){}

            RenderingQueueMesh                                   Mesh             {RenderingQueueMesh()};                                                                                                ///< mesh to render
            ApplicationRenderingBackendTexture                   Texture          {ApplicationRenderingBackendTexture()};                                                                                ///< mesh texture
            gs_mat4f                                             Transform        {gs_mat4f(1.f)};                                                                                                       ///< mesh vertexes transform matrix
            ApplicationRenderingBackendGraphicsApiRenderingHints MeshRendererHints{ApplicationRenderingBackendGraphicsApiRenderingHints_::ApplicationRenderingBackendGraphicsApiRenderingHints_Default}; ///< mesh rendering hints
        };

        /**
         * @brief This struct encapsulates clear color rendering command.
         * @struct RenderingQueueRendererCommandClearColor
         */
        struct RenderingQueueRendererCommandClearColor final
        {
            /**
             * @brief Initializes clear color renderer command
             * @param _ClearColor wanted renderer clear color 
             */
            RenderingQueueRendererCommandClearColor(
                const gs_color& _ClearColor) : ClearColor(_ClearColor){}
            
            gs_color ClearColor; ///< renderer clear color
        };

        /**
         * @brief This struct encapsulates clipping box rendering command.
         * @struct RenderingQueueRendererCommandClippingBox
         */
        struct RenderingQueueRendererCommandClippingBox final
        {
            /**
             * @brief Initializes clear clipping box renderer command
             * @param _ClippinBox wanted renderer clipping box
             */
            RenderingQueueRendererCommandClippingBox(
                const gs_2dboxf& _ClippinBox) : ClippingBox(_ClippinBox){}

            gs_2dboxf ClippingBox; ///< next renderer clipping box
        };

        /**
         * @brief This struct encapsulates renderer rendering command.
         * @struct RenderingQueueCommand
         */
        struct RenderingQueueCommand final
        {
            /**
             * @brief Construct a new Rendering Queue Command object
             * 
             * @param _Command optional mesh rendering command
             * @param _ClearColor optional clear color renderer command
             * @param _ClippingBox optional clipping box renderer command
             */
            RenderingQueueCommand(
                const RenderingQueueRenderingCommand&           _Command,
                const RenderingQueueRendererCommandClearColor&  _ClearColor,
                const RenderingQueueRendererCommandClippingBox& _ClippingBox) :
                Command(_Command),
                ClearColor(_ClearColor),
                ClippingBox(_ClippingBox){}

            Frenchie::Core::Optional<RenderingQueueRenderingCommand>           Command;     ///< optional mesh rendering command
            Frenchie::Core::Optional<RenderingQueueRendererCommandClearColor>  ClearColor;  ///< optional clear color renderer command
            Frenchie::Core::Optional<RenderingQueueRendererCommandClippingBox> ClippingBox; ///< optional clipping box renderer command
        };

        /**
         * @brief  This struct encapsulates rendering queue metrcis.
         * @struct RenderingQueueMetrics
         */
        struct RenderingQueueMetrics final
        {
            double FrameRate              = 0.0; ///< estimated frame rate
            int    RenderedTrianglesCount = 0;   ///< rendered triangles count
            int    RenderingCommandsCount = 0;   ///< rendering queue commands count
        };

        /**
         * @brief This class implements rendering queue functionality.
         * @class RenderingQueue
         * @details Rendering queue has a queu of commands that are executed on render application loop stage.
         * Every command contains mesh, clear color and clipping box. All commands are sorted by their transform matrix Z-axis translation component
         * to provide correct color blending for semi-transparent objects.
         */
        class RenderingQueue : public Layer
        {
        public:

            /**
             * @brief Constructs a new rendering queue layer
             * 
             */
            RenderingQueue(const std::string& = STRINGIFY(RenderingQueue));

            // destructor
            virtual ~RenderingQueue();

            /**
             * @brief returns rendering queue estimated metrics
             * @return returns rendering queue estimated metrics
             */
            RenderingQueueMetrics get_rendering_queue_metrics() const;

            /**
             * @brief returns rendering queue minimum line width.
             * @return returns rendering queue minimum line width. 
             */
            float                 get_minimum_line_width() const;

            /**
             * @brief returns rendering queue cursor position to which all prjection, camera view matrixes are applied.
             * @return returns rendering queue cursor position to which all prjection, camera view matrixes are applied.
             */
            gs_vec3f              get_cursor_postion() const;

            /**
             * @brief returns rendering queue projection near plane.
             * @return returns rendering queue projection near plane.
             */
            float                 get_near_plane() const;

            /**
             * @brief returns rendering queue projection far plane.
             * @return returns rendering queue projection far plane.
             */
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

            /**
             * @brief This function pushes next applied clipping box into rendering queue commands queue.
             * @param _Value clipping box 
             * @param _Transform clipping box transform matrix 
             */
            void push_clip_box(const gs_2dboxf& _Value, const gs_mat4f& _Transform = gs_mat4f(1.f));

            /**
             * @brief This function pops clipping box out-of rendering queue commands queue.
             */
            void pop_clip_box();

            /**
             * @brief This function pushes next applied clear into rendering queue commands queue.
             * @param _Value clear color
             */
            void push_clear_color(const gs_color& _Value);

            /**
             * @brief This function pops clear color out-of rendering queue commands queue.
             */
            void pop_clear_color();
            
            /**
             * @brief This function returns current viewport bounding box.
             * @return returns current viewport bounding box.
             */
            gs_2dboxf current_viewport() const;

            /**
             * @brief This function returns clipping box.
             * @return  returns clipping box.
             */
            gs_2dboxf current_clipping_box() const;

            /**
             * @brief This function returns celar color.
             * @return  returns celar color.
             */
            gs_color  current_clear_color() const;

            /**
             * @brief This function calculates 2D transform matrix.
             * @param _Depth depth
             * @param _Position translate position
             * @param _Rotation 2D rotation XY vector
             * @param _Scale 2D scale XY vector
             * @return 
             */
            gs_mat4f calculate_transform_matrix(
                const float&    _Depth,
                const gs_vec2f& _Position = gs_vec2f(0.f, 0.f),
                const float&    _Rotation = 0.f,
                const gs_vec2f& _Scale    = gs_vec2f(1.f, 1.f));

            /**
             * @brief This function constructs and pushes rendering command into rendering queue.
             * @param _Transform mesh vertexes transform matrix
             * @param _RendererHints mesh rendering hints 
             * @details the command constructed from last built mesh and loaded texture.
             * _Transform, _RendererHints are apppied to last built mesh vertexes.
             */
            void push_rendering_command(
                const gs_mat4f&                                             _Transform,
                const ApplicationRenderingBackendGraphicsApiRenderingHints& _RendererHints = ApplicationRenderingBackendGraphicsApiRenderingHints_::ApplicationRenderingBackendGraphicsApiRenderingHints_Default);

            /**
             * @brief This function constructs and pushes rendering command into rendering queue.
             * @param _Texture mesh texture
             * @param _Color mesh vertexes color
             * @param _Transform mesh vertexes transform matrix
             * @param _RendererHints mesh rendering hints 
             * @details the command constructed from last built mesh and loaded texture.
             * _Transform,_Texture, _Color, _RendererHints are apppied to last built mesh vertexes.
             */
            void push_rendering_command(
                const ApplicationRenderingBackendTexture&                   _Texture,
                const gs_color&                                             _Color,
                const gs_mat4f&                                             _Transform,
                const ApplicationRenderingBackendGraphicsApiRenderingHints& _RendererHints = ApplicationRenderingBackendGraphicsApiRenderingHints_::ApplicationRenderingBackendGraphicsApiRenderingHints_Default);

            /**
             * @brief This function constructs and pushes rendering command into rendering queue.
             * @param _Mesh mesh
             * @param _Texture mesh texture
             * @param _Transform mesh vertexes transform matrix
             * @param _RendererHints mesh rendering hints
             * @param _ClearColor clear color
             * @param _ClippinBox clipping box
             * @details command loads mesh into rendering queue mesh handles (vertexes and indexes arrays) and creates clear color and clipping box.
             */
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

        /*! @} */
    }
}
