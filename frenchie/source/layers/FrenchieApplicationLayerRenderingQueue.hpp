#pragma once

// Core
#include <FrenchieCoreMath.hpp>
#include <FrenchieCoreClock.hpp>
#include <FrenchieCoreRingBuffer.hpp>
#include <FrenchieCoreStringUtilities.hpp>

// Application
#include <FrenchieApplicationLayer.hpp>
#include <FrenchieApplicationPlatformBackend.hpp>
#include <FrenchieApplicationRenderingBackend.hpp>

// STL
#include <optional>

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
             * @param _SourceMeshVertex starting index of a mesh vertex in mesh vertex indexes array
             * @param _TargetMeshVertex ending index of a mesh vertex in mesh vertex indexes array
             */
            RenderingQueueMesh(
                const int _SourceMeshVertex  = -1,
                const int _TargetMeshVertex = -1) :
                SourceMeshVertex(_SourceMeshVertex),
                TargetMeshVertex(_TargetMeshVertex){}

            int SourceMeshVertex; ///< starting index of a mesh vertex in mesh vertex indexes array
            int TargetMeshVertex; ///< ending index of a mesh vertex in mesh vertex indexes array
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
             */
            RenderingQueueRenderingCommand(
                const RenderingQueueMesh&                 _Mesh,
                const ApplicationRenderingBackendTexture& _Texture,
                const gs_mat4f&                           _Transform) :
                Mesh(_Mesh),
                Texture(_Texture),
                Transform(_Transform){}

            RenderingQueueMesh                 Mesh      {RenderingQueueMesh()};                 ///< mesh to render
            ApplicationRenderingBackendTexture Texture   {ApplicationRenderingBackendTexture()}; ///< mesh texture
            gs_mat4f                           Transform {gs_mat4f(1.f)};                        ///< mesh vertexes transform matrix
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
                const gs_2d_boxf& _ClippinBox) : ClippingBox(_ClippinBox){}

            gs_2d_boxf ClippingBox; ///< next renderer clipping box
        };

        /**
         * @brief This struct encapsulates mesh rendering hints rendering command.
         * @struct RenderingQueueRendererCommandMeshRenderingHints
         */
        struct RenderingQueueRendererCommandMeshRenderingHints final
        {
            /**
             * @brief Initializes mesh rendering hints renderer command
             * @param _Hints mesh rendering hints (tells how to draw mesh: as lines, as triangles e.t.c)
             */
            RenderingQueueRendererCommandMeshRenderingHints(
                const ApplicationRenderingBackendMeshRenderingHints& _Hints) : Hints(_Hints){}

            ApplicationRenderingBackendMeshRenderingHints Hints =
                ApplicationRenderingBackendMeshRenderingHints_::ApplicationRenderingBackendMeshRenderingHints_Triangles;
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
             * @param _MeshRenderingHints mesh rendering hints renderer command
             */
            RenderingQueueCommand(
                const RenderingQueueRenderingCommand&                 _Command,
                const RenderingQueueRendererCommandClearColor&        _ClearColor,
                const RenderingQueueRendererCommandClippingBox&       _ClippingBox,
                const RenderingQueueRendererCommandMeshRenderingHints _MeshRenderingHints) :
                Command(_Command),
                ClearColor(_ClearColor),
                ClippingBox(_ClippingBox),
                MeshRenderingHints(_MeshRenderingHints){}

            std::optional<RenderingQueueRenderingCommand>                  Command;        ///< optional mesh rendering command
            std::optional<RenderingQueueRendererCommandClearColor>         ClearColor;     ///< optional clear color renderer command
            std::optional<RenderingQueueRendererCommandClippingBox>        ClippingBox;    ///< optional clipping box renderer command
            std::optional<RenderingQueueRendererCommandMeshRenderingHints> MeshRenderingHints; ///< optional mesh rendering hints command
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
            void push_clip_box(const gs_2d_boxf& _Value, const gs_mat4f& _Transform = gs_mat4f(1.f));

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
             * @brief This function pushes next applied mesh rendering hints into rendering queue commands queue.
             * @param _Hints mesh rendering hints
             */
            void push_mesh_rendering_hints(const ApplicationRenderingBackendMeshRenderingHints& _Hints);

            /**
             * @brief This function pops mesh rendering hints out-of rendering queue commands queue.
             */
            void pop_mesh_rendering_hints();

            /**
             * @brief This function pushes next applied tesselation tolerance into rendering queue commands queue.
             * @param _Value tesselation tolerance in %, default value is 10%
             */
            void push_tesselation_tolerance(const float& _Value);

            /**
             * @brief This function pops tesselation tolerance out-of rendering queue commands queue.
             */
            void pop_tesselation_tolerance();

            /**
             * @brief This function returns current viewport bounding box.
             * @return returns current viewport bounding box.
             */
            gs_2d_boxf current_viewport() const;

            /**
             * @brief This function returns current clipping box.
             * @return  returns current clipping box.
             */
            gs_2d_boxf current_clipping_box() const;

            /**
             * @brief This function returns current clear color.
             * @return  returns current clear color.
             */
            gs_color  current_clear_color() const;

            /**
             * @brief This function returns current mesh rendering hints.
             * @return  current mesh rendering hints.
             */
            ApplicationRenderingBackendMeshRenderingHints current_mesh_rendering_hints() const;

            /**
             * @brief This function returns current tesselation tolerance.
             * @return  returns current tesselation tolerance.
             */
            float current_tesselation_tolerance() const;

            /**
             * @brief This function constructs and pushes rendering command into rendering queue.
             * @param _Transform mesh vertexes transform matrix
             * @details the command constructed from last built mesh and loaded texture.
             * _Transform, _RendererHints are apppied to last built mesh vertexes.
             */
            void push_rendering_command(const gs_mat4f& _Transform);

            /**
             * @brief This function constructs and pushes rendering command into rendering queue.
             * @param _Texture mesh texture
             * @param _Color mesh vertexes color
             * @param _Transform mesh vertexes transform matrix
             * @details the command constructed from last built mesh and loaded texture.
             * _Transform,_Texture, _Color, _RendererHints are apppied to last built mesh vertexes.
             */
            void push_rendering_command(
                const ApplicationRenderingBackendTexture& _Texture,
                const gs_color&                           _Color,
                const gs_mat4f&                           _Transform);

        protected:

            // rendering queue data
            gs_2d_boxf                                                           m_Viewport                           {gs_vec2f(-gs_huge<float>(), -gs_huge<float>()), gs_vec2f(+gs_huge<float>(), +gs_huge<float>())};
            std::vector<gs_color>                                                m_ClearColors                        {std::vector<gs_color>()};
            std::vector<gs_2d_boxf>                                              m_ClippingBoxes                      {std::vector<gs_2d_boxf>()};
            std::vector<ApplicationRenderingBackendMeshRenderingHints>           m_MeshRenderingHints                 {std::vector<ApplicationRenderingBackendMeshRenderingHints>()};
            std::vector<float>                                                   m_TesselationTolerance               {std::vector<float>()};

            // mesh data
            std::vector<ApplicationRenderingBackendMeshVertex>                   m_MeshVertexes                       {std::vector<ApplicationRenderingBackendMeshVertex>()};
            std::vector<ApplicationRenderingBackendMeshVertexIndex>              m_MeshVertexesIndexes                {std::vector<ApplicationRenderingBackendMeshVertexIndex>()};
            ApplicationRenderingBackendMeshVertexIndex                           m_MeshVertexesIndexesOffset          {0};
            std::optional<ApplicationRenderingBackendMeshVertexIndex> m_MeshVertexesStartingIndex          {0};
            float                                                                m_MeshLineMinimumWidth               {4.f};

            // rendering
            gs_mat4f                                                             m_ProjectionMatrix                   {gs_mat4f(1)};
            gs_mat4f                                                             m_CameraViewMatrix                   {gs_mat4f(1)};
            std::vector<RenderingQueueCommand>                                   m_Commands                           {std::vector<RenderingQueueCommand>()};

            // metrics measurement
            Frenchie::Core::Clock::TimePoint                                     m_FrameRateMeasurementStartTimePoint {Frenchie::Core::Clock::tic()};
            Frenchie::Core::RingBuffer<double, 64>                               m_FrameRateMeasurementFilterBuffer   {Frenchie::Core::RingBuffer<double, 64>(0.0)};
            RenderingQueueMetrics                                                m_Metrics                            {RenderingQueueMetrics()};

            double                                                               m_MeshDataCleanUpInterval            {30};
            bool                                                                 m_MeshDataWantsCleanUp               {false};
            Frenchie::Core::Clock::TimePoint                                     m_MeshDataCleanUpTimePoint           {Frenchie::Core::Clock::TimePoint()};

            // service methods
            void begin_mesh();
            void end_mesh();
        };

        /*! @} */
    }
}
