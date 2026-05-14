// Metal implementation
#import <CoreGraphics/CGGeometry.h>
#import <Foundation/Foundation.h>
#import <QuartzCore/QuartzCore.h>
#import <Metal/Metal.h>
#import <AppKit/AppKit.h>
#import <MetalKit/MetalKit.h>

// Application
#include <FrenchieApplicationPlatformBackend.hpp>
#include <FrenchieApplicationRenderingBackend.hpp>

#include <iostream>

namespace Frenchie
{
    namespace Application
    {
        struct ApplicationRenderingBackendMetal : public ApplicationRenderingBackendGraphicsApi
        {
            ApplicationRenderingBackendMetal(){}
            virtual ~ApplicationRenderingBackendMetal(){}

            id<MTLDevice>               gpu;
            id<MTLCommandQueue>         queue;
            id<CAMetalDrawable>         surface;
            id<MTLCommandBuffer>        buffer;
            id<MTLRenderCommandEncoder> encoder;
            CAMetalLayer*               layer{nullptr};
        };
    }
}

// GLAD
using namespace Frenchie::Application;

bool ApplicationRenderingBackend::awake(const std::any& _Stuff)
{
    NSWindow* window = nullptr;

    try
    {
        window = reinterpret_cast<NSWindow* >(std::any_cast<id>(_Stuff));
    }
    catch(...)
    {
        return false;
    }

    std::shared_ptr<ApplicationRenderingBackendMetal> Metal =
        std::dynamic_pointer_cast<ApplicationRenderingBackendMetal>(m_Api = std::make_shared<ApplicationRenderingBackendMetal>());

    if(Metal == nullptr)
        return false;

    // create device and command queue
    Metal->gpu   = MTLCreateSystemDefaultDevice();
    Metal->queue = [Metal->gpu newCommandQueue];

    // create layer (swapchain)
    Metal->layer = [CAMetalLayer layer];
    Metal->layer.device = Metal->gpu;
    Metal->layer.opaque = YES;

    // configure view
    MTKView* view = [window contentView];
    view.layer = Metal->layer;
    view.wantsLayer = YES;

    // create rendering pipeline state
    {
        // compile shaders
        // create vertex descriptor
    }

    return true;
}

void ApplicationRenderingBackend::quit()
{
}

void ApplicationRenderingBackend::set_viewport(const gs_vec2f& _Position, const gs_vec2f& _Size)
{
    (void)_Position;
    (void)_Size;
}

ApplicationRenderingBackendTexture ApplicationRenderingBackend::construct_texture(
    const unsigned char*                               _RawBuffer,
    const int&                                         _Width,
    const int&                                         _Height,
    const ApplicationRenderingBackendTextureFormat&    _Format,
    const ApplicationRenderingBackendTextureWrapMode&  _Wrap,
    const ApplicationRenderingBackendTextureMinFilter& _MinFilter,
    const ApplicationRenderingBackendTextureMaxFilter& _MaxFilter)
{
    if(_RawBuffer == nullptr)
        return ApplicationRenderingBackendTexture();
    return ApplicationRenderingBackendTexture();
}

void ApplicationRenderingBackend::destroy_texture(const ApplicationRenderingBackendTexture& _Texture)
{
}

bool ApplicationRenderingBackend::begin_render(
    const ApplicationRenderingBackendMeshVertex*      _Vertexes,
    const ApplicationRenderingBackendMeshVertexIndex& _VertexesCount,
    const ApplicationRenderingBackendMeshVertexIndex* _Indexes,
    const ApplicationRenderingBackendMeshVertexIndex& _IndexesCount)
{
    std::shared_ptr<ApplicationRenderingBackendMetal> Metal = graphics_api<ApplicationRenderingBackendMetal>();

    if(Metal == nullptr || _Vertexes == nullptr || _Indexes == nullptr || _VertexesCount <= 0 || _IndexesCount <= 0)
        return false;

    // retrieve surface from view layer
    Metal->surface = [Metal->layer nextDrawable];

    // create render pass descriptor
    MTLRenderPassDescriptor *pass = [MTLRenderPassDescriptor renderPassDescriptor];
    pass.colorAttachments[0].clearColor  = MTLClearColorMake(0., 0.2, 0., 1);
    pass.colorAttachments[0].loadAction  = MTLLoadActionClear;
    pass.colorAttachments[0].storeAction = MTLStoreActionStore;
    pass.colorAttachments[0].texture     = Metal->surface.texture;

    Metal->buffer  = [Metal->queue commandBuffer];
    Metal->encoder = [Metal->buffer renderCommandEncoderWithDescriptor:pass];

    return true;
}

void ApplicationRenderingBackend::render_mesh(
    const ApplicationRenderingBackendMeshVertex*                _Vertexes,
    const ApplicationRenderingBackendMeshVertexIndex&           _VertexesCount,
    const ApplicationRenderingBackendMeshVertexIndex&           _MeshVertexesCount,
    const ApplicationRenderingBackendMeshVertexIndex&           _MeshVertexesOffset,
    const ApplicationRenderingBackendMeshVertexIndex*           _Indexes,
    const ApplicationRenderingBackendMeshVertexIndex&           _IndexesCount,
    const ApplicationRenderingBackendMeshVertexIndex&           _MeshIndexesCount,
    const ApplicationRenderingBackendMeshVertexIndex&           _MeshIndexesOffset,
    const ApplicationRenderingBackendTexture&                   _Texture,
    const gs_mat4f&                                             _MeshProjectionMatrix,
    const ApplicationRenderingBackendGraphicsApiRenderingHints& _MeshRenderHints)
{
}

void ApplicationRenderingBackend::end_render()
{
    std::shared_ptr<ApplicationRenderingBackendMetal> Metal = graphics_api<ApplicationRenderingBackendMetal>();

    if(Metal == nullptr)
        return;

    [Metal->encoder endEncoding];
    [Metal->buffer presentDrawable:Metal->surface];
    [Metal->buffer commit];
}

void ApplicationRenderingBackend::clear_color(const gs_color& _Color)
{
}

void ApplicationRenderingBackend::scissor_box(const gs_2dboxf& _ClippingRect)
{
}

// camera and view projection API
ApplicationRenderingBackend::Projections ApplicationRenderingBackend::calculate_2d_camera_view_and_projection(
    const gs_vec2f& _CameraWorldPosition,
    const gs_vec2f& _CameraResolution,
    const float&    _CameraRotationAngle,
    const float&    _CameraNearPlanePosition,
    const float&    _CameraFarPlanePosition)
{
    // compute projection matrix
    float left   = -_CameraResolution.x * 0.5f + _CameraWorldPosition.x;
    float right  = +_CameraResolution.x * 0.5f + _CameraWorldPosition.x;
    float bottom = +_CameraResolution.y * 0.5f + _CameraWorldPosition.y;
    float top    = -_CameraResolution.y * 0.5f + _CameraWorldPosition.y;

    // camera orientation
    gs_vec3f cameraWorldUpAxisDirection    = gs_vec3f(0.f, 1.f, 0.f);
    gs_vec3f cameraWorldFrontAxisDirection = gs_vec3f(0.f, 0.f, -1.f);
    gs_vec3f cameraLocalFrontAxisDirection = gs_vector_normalize(cameraWorldFrontAxisDirection);
    gs_vec3f cameraLocalRightAxisDirection = gs_vector_normalize(gs_vector_cross(cameraLocalFrontAxisDirection, cameraWorldUpAxisDirection));
    gs_vec3f cameraLocalUpAxisDirection    = gs_vector_normalize(gs_vector_cross(cameraLocalRightAxisDirection, cameraLocalFrontAxisDirection));

    gs_mat4f cameraview =
        gs_matrix_look_at(
            gs_vec3f(0.f, 0.f, 1),
            gs_vec3f(0.f, 0.f, 1) + cameraLocalFrontAxisDirection, cameraLocalUpAxisDirection);
    
    gs_mat4f projection =
        gs_matrix_ortho(
            left,
            right,
            bottom,
            top,
            _CameraNearPlanePosition,
            _CameraFarPlanePosition) * gs_matrix_rotate(gs_mat4f(1.f), gs_to_radians(_CameraRotationAngle), gs_vec3f(0.f, 0.f, 1.f));

    return {cameraview, projection};
}

gs_mat4f ApplicationRenderingBackend::calculate_2d_transform_matrix(const float& _Depth, const gs_vec2f& _Position, const float& _Rotation, const gs_vec2f& _Scale)
{
    gs_mat4f matrix(1.f);

    return gs_matrix_translate(matrix, gs_vec3f(_Position, _Depth)) *
            gs_matrix_rotate(matrix, gs_to_radians(_Rotation), gs_vec3f(0.f, 0.f, 1.f)) * 
            gs_matrix_scale(matrix, gs_vec3f(_Scale, 1.f));
}

bool ApplicationRenderingBackend::compare_objects_depths(const float& _A, const float& _B)
{
    return _A < _B;
}

gs_vec2f ApplicationRenderingBackend::convert_to_NDC(const gs_vec2f& _Position, const gs_vec2f& _Screen)
{
    return gs_vec2f((2.0f * _Position.x) / _Screen.x - 1.0f, 1.0f - (2.0f * _Position.y) / _Screen.y);
}