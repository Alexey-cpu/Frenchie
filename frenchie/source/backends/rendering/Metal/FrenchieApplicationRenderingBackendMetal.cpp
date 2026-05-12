// Metal implementation
#define NS_PRIVATE_IMPLEMENTATION
#define CA_PRIVATE_IMPLEMENTATION
#define MTL_PRIVATE_IMPLEMENTATION
#include <Foundation/Foundation.hpp>
#include <Metal/Metal.hpp>
#include <QuartzCore/QuartzCore.hpp>

// Application
#include <FrenchieApplicationPlatformBackend.hpp>
#include <FrenchieApplicationRenderingBackend.hpp>

#include <iostream>

// GLAD
using namespace Frenchie::Application;

namespace Frenchie
{
    namespace Application
    {
        struct ApplicationRenderingBackendMetal : public ApplicationRenderingBackendGraphicsApi
        {
            ApplicationRenderingBackendMetal(){}
            virtual ~ApplicationRenderingBackendMetal(){}

            MTL::Device*           Device         {nullptr};
            MTL::Buffer*           VertexBuffer   {nullptr};
            MTL::Buffer*           IndexBuffer    {nullptr};
            MTL::VertexDescriptor* VertexDesciptor{nullptr};
            MTL::Library*          ShaderProgramm {nullptr};
        };
    }
}

bool ApplicationRenderingBackend::awake(const std::any& _Stuff)
{
    // create API
    m_Api = std::make_shared<ApplicationRenderingBackendMetal>();

    auto Metal = graphics_api<ApplicationRenderingBackendMetal>();

    if(Metal == nullptr)
        return false;

    // create device
    Metal->Device = MTL::CreateSystemDefaultDevice();

    // create rendering pipeline
    NS::Error* error = nullptr;

    MTL::CompileOptions* compile_options = MTL::CompileOptions::alloc()->init();

    Metal->ShaderProgramm = Metal->Device->newLibrary(
        NS::String::string(
R"(
#include <metal_stdlib>
using namespace metal;
#include <simd/simd.h>

struct VertexIn
{
    float3 Position [[attribute(0)]];
    float3 Normal [[attribute(1)]];
    float3 UV [[attribute(2)]];
    float4 Color [[attribute(3)]];
};

struct VertexOut
{
    float4 Position [[position]];
    float3 Normal;
    float3 UV;
    float4 Color;
};

struct Uniforms
{
    float4x4 Projection;
};

vertex VertexOut vertex_main(
    const VertexIn _Input [[stage_in]],
    constant Uniforms& _Uniforms [[buffer(1)]])
{
    VertexOut out;
    out.Position = _Uniforms.Projection * float4(_Input.Position, 1.f);
    out.Normal   = _Input.Normal;
    out.UV       = _Input.UV;
    out.Color    = _Input.Color;
    return out;
}

fragment float4 fragment_main(VertexOut _Input [[stage_in]])
{
    return _Input.Color;
}    
)",
    NS::ASCIIStringEncoding),
    compile_options,
    &error);

    // TODO: uncomment on error
    // if (Metal->DefaultLibrary == nullptr)
    // {
    //     std::cout << error->localizedDescription()->utf8String() << std::endl;
    // }
    // else std::cout << "SHADER COMPILATION SUCCEEDED !!! \n";

    MTL::Function* vertexShader   = Metal->ShaderProgramm->newFunction(NS::String::string("vertex_main", NS::ASCIIStringEncoding));
    MTL::Function* fragmentShader = Metal->ShaderProgramm->newFunction(NS::String::string("fragment_main", NS::ASCIIStringEncoding));

    MTL::RenderPipelineDescriptor* renderPipelineDescriptor = MTL::RenderPipelineDescriptor::alloc()->init();
    renderPipelineDescriptor->setLabel(NS::String::string("Triangle Rendering Pipeline", NS::ASCIIStringEncoding));
    renderPipelineDescriptor->setVertexFunction(vertexShader);
    renderPipelineDescriptor->setFragmentFunction(fragmentShader);
    renderPipelineDescriptor->colorAttachments()->object(0)->setPixelFormat(MTL::PixelFormat::PixelFormatRGBA8Uint);

    if(renderPipelineDescriptor)
        renderPipelineDescriptor->release();

    return Metal->Device != nullptr;
}

void ApplicationRenderingBackend::quit()
{
    auto Metal = graphics_api<ApplicationRenderingBackendMetal>();

    if (Metal == nullptr)
        return;

    if(Metal->VertexBuffer != nullptr)
    {
        Metal->VertexBuffer->release();
        Metal->VertexBuffer = nullptr;
    }

    if(Metal->IndexBuffer != nullptr)
    {
        Metal->IndexBuffer->release();
        Metal->IndexBuffer = nullptr;
    }

    if(Metal->ShaderProgramm != nullptr)
    {
        Metal->ShaderProgramm->release();
        Metal->ShaderProgramm = nullptr;
    }

    if(Metal->Device != nullptr)
    {
        Metal->Device->release();
        Metal->Device = nullptr;
    }
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
    auto Metal = graphics_api<ApplicationRenderingBackendMetal>();

    if (Metal == nullptr)
        return false;

    // manage buffers
    if(
        Metal->VertexBuffer == nullptr ||
        Metal->IndexBuffer  == nullptr || 
        Metal->VertexBuffer->length() < _VertexesCount * sizeof(ApplicationRenderingBackendMeshVertex) ||
        Metal->IndexBuffer->length() < _IndexesCount * sizeof(ApplicationRenderingBackendMeshVertexIndex))
    {
        // release buffers
        if(Metal->VertexBuffer != nullptr)
        {
            Metal->VertexBuffer->release();
            Metal->VertexBuffer = nullptr;
        }

        if(Metal->IndexBuffer != nullptr)
        {
            Metal->IndexBuffer->release();
            Metal->IndexBuffer = nullptr;
        }

        // release vertex descriptor
        if(Metal->VertexDesciptor != nullptr)
        {
            Metal->VertexDesciptor->release();
            Metal->VertexDesciptor = nullptr;
        }

        // create new resized buffers
        Metal->Device->newBuffer(
            _Vertexes,
            _VertexesCount * sizeof(ApplicationRenderingBackendMeshVertex),
            MTL::ResourceStorageModeShared);

        Metal->Device->newBuffer(
            _Indexes,
            _IndexesCount * sizeof(ApplicationRenderingBackendMeshVertexIndex),
            MTL::ResourceStorageModeShared);

        // create vertex descriptor
        Metal->VertexDesciptor = MTL::VertexDescriptor::alloc()->init();

        // position
        Metal->VertexDesciptor->attributes()->object(0)->setFormat(MTL::VertexFormatFloat3);
        Metal->VertexDesciptor->attributes()->object(0)->setOffset(0);
        Metal->VertexDesciptor->attributes()->object(0)->setBufferIndex(0);

        // normal
        Metal->VertexDesciptor->attributes()->object(0)->setFormat(MTL::VertexFormatFloat3);
        Metal->VertexDesciptor->attributes()->object(0)->setOffset(sizeof(float) * 3);
        Metal->VertexDesciptor->attributes()->object(0)->setBufferIndex(1);

        // UV
        Metal->VertexDesciptor->attributes()->object(0)->setFormat(MTL::VertexFormatFloat2);
        Metal->VertexDesciptor->attributes()->object(0)->setOffset(sizeof(float) * 3 + sizeof(float) * 3);
        Metal->VertexDesciptor->attributes()->object(0)->setBufferIndex(2);

        // Color
        Metal->VertexDesciptor->attributes()->object(0)->setFormat(MTL::VertexFormatUInt);
        Metal->VertexDesciptor->attributes()->object(0)->setOffset(sizeof(float) * 3 + sizeof(float) * 3 + sizeof(float) * 2);
        Metal->VertexDesciptor->attributes()->object(0)->setBufferIndex(3);

        // configure vertex layout
        Metal->VertexDesciptor->layouts()->object(0)->setStride(sizeof(ApplicationRenderingBackendMeshVertex));
        Metal->VertexDesciptor->layouts()->object(0)->setStepFunction(MTL::VertexStepFunctionPerVertex);
        Metal->VertexDesciptor->layouts()->object(0)->setStepRate(1);
    }

    // wrtie vertex buffer
    ApplicationRenderingBackendMeshVertex* pVertexes =
        reinterpret_cast<ApplicationRenderingBackendMeshVertex*>(Metal->VertexBuffer->contents());

    for (ApplicationRenderingBackendMeshVertexIndex i = 0; i < _VertexesCount; i++)
        pVertexes[i] = _Vertexes[i];
    
    // write index buffer
    ApplicationRenderingBackendMeshVertexIndex* pIndexes =
        reinterpret_cast<ApplicationRenderingBackendMeshVertexIndex*>(Metal->IndexBuffer->contents());

    for (ApplicationRenderingBackendMeshVertexIndex i = 0; i < _IndexesCount; i++)
        pIndexes[i] = _Indexes[i];

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
    if(m_Api == nullptr || _Vertexes == nullptr || _Indexes == nullptr || _VertexesCount <= 0 || _IndexesCount <= 0 || _MeshVertexesCount <= 0 || _MeshIndexesCount <= 0)
        return;
}

void ApplicationRenderingBackend::end_render()
{
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