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
        static const NSUInteger kMaxInflightBuffers = 3;

        struct ApplicationRenderingBackendMetal : public ApplicationRenderingBackendGraphicsApi
        {
            ApplicationRenderingBackendMetal(){}
            virtual ~ApplicationRenderingBackendMetal(){}

            // device
            id<MTLDevice>               Device                    = nil;
            id<MTLCommandQueue>         CommandQueue              = nil;
            id<MTLCommandBuffer>        CommandBuffer             = nil;
            id<MTLRenderCommandEncoder> CommandEncoder            = nil;
            id<CAMetalDrawable>         DrawableSurface           = nil;

            // renderer
            id<MTLDepthStencilState>    RendererDepthState        = nil;
            id<MTLRenderPipelineState>  RendererPipeLineState     = nil;

            // client window vieport
            CAMetalLayer*               ClientWindowViewportLayer = nil;
            MTKView*                    ClientWindowViewport      = nil;

            // frame buffers and synchronization attributes
            dispatch_semaphore_t        FrameSynchronizationSemaphore;
            id <MTLBuffer>              CurrentFrameVertexBuffers[kMaxInflightBuffers];
            id <MTLBuffer>              CurretnFrameIndexBuffers [kMaxInflightBuffers];
            NSUInteger                  CurrentFrameIndex;

            // data
            gs_color                            ClearColor = gs_color_rgba(255, 255, 255, 255);
            Frenchie::Core::Optional<gs_2dboxf> Viewport;
        };

        struct ApplicationRenderingBackendMetalTextureData
        {
            id<MTLTexture>      Texture      = nil;
            id<MTLSamplerState> SamplerState = nil;
        };

        struct ApplicationRenderingBackendMetalShaderUniforms
        {
            matrix_float4x4 Projection;
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
        std::dynamic_pointer_cast<ApplicationRenderingBackendMetal>(
            m_Api = std::make_shared<ApplicationRenderingBackendMetal>());

    if(Metal == nullptr)
        return false;

    // create device and command queue
    Metal->Device   = MTLCreateSystemDefaultDevice();
    Metal->CommandQueue = [Metal->Device newCommandQueue];

    // create layer (swapchain)
    Metal->ClientWindowViewportLayer = [CAMetalLayer layer];
    Metal->ClientWindowViewportLayer.device = Metal->Device;
    Metal->ClientWindowViewportLayer.opaque = YES;

    // configure view
    Metal->ClientWindowViewport            = [window contentView];
    Metal->ClientWindowViewport.layer      = Metal->ClientWindowViewportLayer;
    Metal->ClientWindowViewport.wantsLayer = YES;

    // create rendering pipeline state
    {
        // compile shaders
        const char* shaderSource =
R"(
#include <metal_stdlib>
#include <simd/simd.h>
using namespace metal;

struct ApplicationRenderingBackendMetalShaderVertexIn
{
    float3 Position [[attribute(0)]];
    float3 Normal [[attribute(1)]];
    float2 UV [[attribute(2)]];
    uint   Color [[attribute(3)]];
};

struct ApplicationRenderingBackendMetalShaderVertexOut
{
    float4 Position [[position]];
    float3 Normal;
    float2 UV;
    float4 Color;
};

struct ApplicationRenderingBackendMetalShaderUniforms
{
    float4x4 Projection;
};

vertex ApplicationRenderingBackendMetalShaderVertexOut vertex_main(
    const    ApplicationRenderingBackendMetalShaderVertexIn  _Input    [[stage_in]],
    constant ApplicationRenderingBackendMetalShaderUniforms& _Uniforms [[buffer(1)]])
{
    ApplicationRenderingBackendMetalShaderVertexOut out;
    out.Position = _Uniforms.Projection * float4(_Input.Position, 1.f);
    out.Normal   = _Input.Normal;
    out.UV       = _Input.UV;
    out.Color    = unpack_unorm4x8_to_float(_Input.Color);
    return out;
}

fragment float4 fragment_main(
    ApplicationRenderingBackendMetalShaderVertexOut _Input   [[stage_in]],
    texture2d<float>                                _Texture [[texture(0)]],
    sampler                                         _Sampler [[sampler(0)]])
{
    return _Input.Color * _Texture.sample(_Sampler, _Input.UV);
}    
)";

        NSError *error = nil;
        id<MTLLibrary> library = [Metal->Device newLibraryWithSource:
            [NSString stringWithUTF8String:shaderSource]
            options:nil
            error:&error];

        if(error != nil)
        {
            NSLog(@"%@",[error localizedDescription]);
            return false;
        }

        id<MTLFunction> vertexShader = [library newFunctionWithName:@"vertex_main"];     
        id<MTLFunction> pixelShader  = [library newFunctionWithName:@"fragment_main"];     

        // create vertex descriptor
        MTLVertexDescriptor* vertexDescriptor = [MTLVertexDescriptor vertexDescriptor];

        // position
        vertexDescriptor.attributes[0].format      = MTLVertexFormatFloat3;
        vertexDescriptor.attributes[0].offset      = 0;
        vertexDescriptor.attributes[0].bufferIndex = 0;

        // normal
        vertexDescriptor.attributes[1].format      = MTLVertexFormatFloat3;
        vertexDescriptor.attributes[1].offset      = sizeof(Frenchie::Application::ApplicationRenderingBackendMeshVertex::Position);
        vertexDescriptor.attributes[1].bufferIndex = 0;

        // UV
        vertexDescriptor.attributes[2].format = MTLVertexFormatFloat2;
        vertexDescriptor.attributes[2].offset =
            sizeof(Frenchie::Application::ApplicationRenderingBackendMeshVertex::Position) +
            sizeof(Frenchie::Application::ApplicationRenderingBackendMeshVertex::Normal);
        vertexDescriptor.attributes[2].bufferIndex = 0;

        // Color
        vertexDescriptor.attributes[3].format = MTLVertexFormatUInt;
        vertexDescriptor.attributes[3].offset =
            sizeof(Frenchie::Application::ApplicationRenderingBackendMeshVertex::Position) +
            sizeof(Frenchie::Application::ApplicationRenderingBackendMeshVertex::Normal)   +
            sizeof(Frenchie::Application::ApplicationRenderingBackendMeshVertex::UV);
        vertexDescriptor.attributes[3].bufferIndex = 0;

        // Layout
        vertexDescriptor.layouts[0].stride       = sizeof(ApplicationRenderingBackendMeshVertex);
        vertexDescriptor.layouts[0].stepFunction = MTLVertexStepFunctionPerVertex;
        vertexDescriptor.layouts[0].stepRate     = 1;

        // render pipeline descriptor
        MTLRenderPipelineDescriptor* pipelineDescriptor        = [MTLRenderPipelineDescriptor new];
        pipelineDescriptor.label                               = @"Indexed mesh rendering pipeline";
        pipelineDescriptor.vertexFunction                      = vertexShader;
        pipelineDescriptor.fragmentFunction                    = pixelShader;
        pipelineDescriptor.vertexDescriptor                    = vertexDescriptor;
        pipelineDescriptor.depthAttachmentPixelFormat          = MTLPixelFormatDepth32Float;

        MTLRenderPipelineColorAttachmentDescriptor* colorAttachment = pipelineDescriptor.colorAttachments[0];
        colorAttachment.pixelFormat                 = MTLPixelFormatBGRA8Unorm;
        colorAttachment.blendingEnabled             = YES;
        colorAttachment.rgbBlendOperation           = MTLBlendOperationAdd;
        colorAttachment.alphaBlendOperation         = MTLBlendOperationAdd;
        colorAttachment.sourceRGBBlendFactor        = MTLBlendFactorSourceAlpha;
        colorAttachment.sourceAlphaBlendFactor      = MTLBlendFactorSourceAlpha;
        colorAttachment.destinationRGBBlendFactor   = MTLBlendFactorOneMinusSourceAlpha;
        colorAttachment.destinationAlphaBlendFactor = MTLBlendFactorOneMinusSourceAlpha;

        // depth descriptor
        MTLDepthStencilDescriptor* depthDescriptor         = [MTLDepthStencilDescriptor new];
        depthDescriptor.label                              = @"Indexed mesh rendering pipeline depth buffer state";
        depthDescriptor.depthCompareFunction               = MTLCompareFunctionLess;
        depthDescriptor.depthWriteEnabled                  = YES;

        Metal->RendererPipeLineState = [Metal->Device newRenderPipelineStateWithDescriptor:pipelineDescriptor error:&error];
        Metal->RendererDepthState    = [Metal->Device newDepthStencilStateWithDescriptor:depthDescriptor];

        if(error != nil)
        {
            NSLog(@"%@",[error localizedDescription]);
            return false;
        }

        // clean-up
        [vertexShader release];
        [pixelShader release];
        [vertexDescriptor release];
        [pipelineDescriptor release];
        [depthDescriptor release];
    }

    // sync layer
    {
        Metal->FrameSynchronizationSemaphore = dispatch_semaphore_create(kMaxInflightBuffers);
        Metal->CurrentFrameIndex = 0;

        for(int i = 0; i < kMaxInflightBuffers; i++)
        {
            Metal->CurrentFrameVertexBuffers[i] = nil;
            Metal->CurretnFrameIndexBuffers [i] = nil;
        }
    }

    return true;
}

void ApplicationRenderingBackend::quit()
{
    std::shared_ptr<ApplicationRenderingBackendMetal> Metal = graphics_api<ApplicationRenderingBackendMetal>();

    if(Metal == nullptr)
        return;

    if(Metal->Device != nil)
    {
        [Metal->Device release];
        Metal->Device = nil;
    }

    if(Metal->CommandQueue != nil)
    {
        [Metal->CommandQueue release];
        Metal->CommandQueue = nil;
    }

    if(Metal->DrawableSurface != nil)
    {
        [Metal->DrawableSurface release];
        Metal->DrawableSurface = nil;
    }

    if(Metal->CommandBuffer != nil)
    {
        [Metal->CommandBuffer release];
        Metal->CommandBuffer = nil;
    }

    if(Metal->CommandEncoder != nil)
    {
        [Metal->CommandEncoder release];
        Metal->CommandEncoder = nil;
    }

    if(Metal->RendererPipeLineState != nil)
    {
        [Metal->RendererPipeLineState release];
        Metal->RendererPipeLineState = nil;
    }

    if(Metal->RendererDepthState != nil)
    {
        [Metal->RendererPipeLineState release];
        Metal->RendererDepthState = nil;
    }

    // clean-up buffers
    for(int i = 0; i < kMaxInflightBuffers; i++)
    {
        if(Metal->CurrentFrameVertexBuffers[i] != nil)
        {
            [Metal->CurrentFrameVertexBuffers[i] release];
            Metal->CurrentFrameVertexBuffers[i] = nil;
        }

        if(Metal->CurretnFrameIndexBuffers[i] != nil)
        {
            [Metal->CurretnFrameIndexBuffers [i] release];
            Metal->CurretnFrameIndexBuffers [i] = nil;
        }
    }
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
    std::shared_ptr<ApplicationRenderingBackendMetal> Metal = graphics_api<ApplicationRenderingBackendMetal>();

    if(Metal == nullptr)
        return ApplicationRenderingBackendTexture();

    // create texture descriptor
    MTLTextureDescriptor* textureDescriptor = [[MTLTextureDescriptor alloc]init];

    textureDescriptor.width       = _Width;
    textureDescriptor.height      = _Height;
    textureDescriptor.pixelFormat = MTLPixelFormatRGBA8Unorm;//MTLPixelFormatBGRA8Unorm;

    // create texture
    id<MTLTexture> texture = [Metal->Device newTextureWithDescriptor:textureDescriptor];

    MTLRegion region = {{ 0, 0, 0 }, {NSUInteger(_Width), NSUInteger(_Height), 1}};

    NSUInteger bytesPerRow = 0;

    switch(_Format)
    {
        case ApplicationRenderingBackendTextureFormat_::ApplicationRenderingBackendTextureFormat_ALPHA:
        bytesPerRow = 1 * NSUInteger(_Width);
            break;

        case ApplicationRenderingBackendTextureFormat_::ApplicationRenderingBackendTextureFormat_RGB:
        bytesPerRow = 3 * NSUInteger(_Width);
            break;

        case ApplicationRenderingBackendTextureFormat_::ApplicationRenderingBackendTextureFormat_RGBA:
        bytesPerRow = 4 * NSUInteger(_Width);
            break;
    }

    [texture replaceRegion:region mipmapLevel:0 withBytes:_RawBuffer bytesPerRow:bytesPerRow];

    // create texture sampler
    MTLSamplerDescriptor* samplerDescriptor = [MTLSamplerDescriptor new];
    samplerDescriptor.normalizedCoordinates = YES; // Use 0.0 - 1.0 range

    // set the texture wrapping parameters
    switch (_Wrap)
    {
    case ApplicationRenderingBackendTextureWrapMode_::ApplicationRenderingBackendTextureWrapMode_Repeat:
        samplerDescriptor.sAddressMode = MTLSamplerAddressModeRepeat;
        samplerDescriptor.tAddressMode = MTLSamplerAddressModeRepeat;
        break;
    
    case ApplicationRenderingBackendTextureWrapMode_::ApplicationRenderingBackendTextureWrapMode_Mirrored:
        samplerDescriptor.sAddressMode = MTLSamplerAddressModeMirrorRepeat;
        samplerDescriptor.tAddressMode = MTLSamplerAddressModeMirrorRepeat;
        break;

    case ApplicationRenderingBackendTextureWrapMode_::ApplicationRenderingBackendTextureWrapMode_ClampToEdge:
        samplerDescriptor.sAddressMode = MTLSamplerAddressModeClampToEdge;
        samplerDescriptor.tAddressMode = MTLSamplerAddressModeClampToEdge;
        break;

    case ApplicationRenderingBackendTextureWrapMode_::ApplicationRenderingBackendTextureWrapMode_ClampToBorder:
        samplerDescriptor.sAddressMode = MTLSamplerAddressModeClampToBorderColor;
        samplerDescriptor.tAddressMode = MTLSamplerAddressModeClampToBorderColor;
        break;
    default:
        samplerDescriptor.sAddressMode = MTLSamplerAddressModeRepeat;
        samplerDescriptor.tAddressMode = MTLSamplerAddressModeRepeat;
        break;
    }
    
    // set minifying filter
    switch (_MinFilter)
    {
    case ApplicationRenderingBackendTextureMinFilter_::ApplicationRenderingBackendTextureMinFilter_Linear:
        samplerDescriptor.minFilter = MTLSamplerMinMagFilterLinear;
        break;
    
    case ApplicationRenderingBackendTextureMinFilter_::ApplicationRenderingBackendTextureMinFilter_Nearest:
        samplerDescriptor.minFilter = MTLSamplerMinMagFilterNearest;
        break;

    case ApplicationRenderingBackendTextureMinFilter_::ApplicationRenderingBackendTextureMinFilter_LinearMipMapLinear:
        samplerDescriptor.minFilter = MTLSamplerMinMagFilterLinear;
        samplerDescriptor.mipFilter = MTLSamplerMipFilterLinear;
        break;

    case ApplicationRenderingBackendTextureMinFilter_::ApplicationRenderingBackendTextureMinFilter_LinearMipMapNearest:
        samplerDescriptor.minFilter = MTLSamplerMinMagFilterLinear;
        samplerDescriptor.mipFilter = MTLSamplerMipFilterNearest;
        break;

    case ApplicationRenderingBackendTextureMinFilter_::ApplicationRenderingBackendTextureMinFilter_NearestMipMapLinear:
        samplerDescriptor.minFilter = MTLSamplerMinMagFilterNearest;
        samplerDescriptor.mipFilter = MTLSamplerMipFilterLinear;
        break;

    case ApplicationRenderingBackendTextureMinFilter_::ApplicationRenderingBackendTextureMinFilter_NearestMipMapNearest:
        samplerDescriptor.minFilter = MTLSamplerMinMagFilterNearest;
        samplerDescriptor.mipFilter = MTLSamplerMipFilterNearest;
        break;

    default:
        samplerDescriptor.minFilter = MTLSamplerMinMagFilterLinear;
        samplerDescriptor.mipFilter = MTLSamplerMipFilterLinear;
        break;
    }

    // set magnifying filter
    switch (_MaxFilter)
    {
    case ApplicationRenderingBackendTextureMaxFilter_::ApplicationRenderingBackendTextureMaxFilter_Linear:
        samplerDescriptor.magFilter = MTLSamplerMinMagFilterLinear;
        break;
    
    case ApplicationRenderingBackendTextureMaxFilter_::ApplicationRenderingBackendTextureMaxFilter_Nearest:
        samplerDescriptor.magFilter = MTLSamplerMinMagFilterNearest;
        break;

    default:
        samplerDescriptor.magFilter = MTLSamplerMinMagFilterLinear;
        break;
    }

    id<MTLSamplerState> samplerState  = [Metal->Device newSamplerStateWithDescriptor:samplerDescriptor];

    // clean-up
    [textureDescriptor release];

    ApplicationRenderingBackendMetalTextureData* textureData =
        new ApplicationRenderingBackendMetalTextureData();

    textureData->Texture      = texture;
    textureData->SamplerState = samplerState;

    return ApplicationRenderingBackendTexture(
        reinterpret_cast<uintptr_t>(textureData),
        _Width,
        _Height,
        gs_color_rgba(255, 255, 255, 255),
        _Format,
        _Wrap,
        _MinFilter,
        _MaxFilter);
}

void ApplicationRenderingBackend::destroy_texture(const ApplicationRenderingBackendTexture& _Texture)
{
    ApplicationRenderingBackendMetalTextureData* textureData =
        !_Texture.is_null() ?
            reinterpret_cast<ApplicationRenderingBackendMetalTextureData*>(_Texture.Ptr) :
                nil;

    if(textureData == nullptr)
        return;

    if(textureData->Texture != nil)
        [textureData->Texture release];
    
    if(textureData->SamplerState != nil)
        [textureData->SamplerState release];
    
    delete textureData;
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

    dispatch_semaphore_wait(Metal->FrameSynchronizationSemaphore, DISPATCH_TIME_FOREVER);

    // update current buffer index
    Metal->CurrentFrameIndex = (Metal->CurrentFrameIndex + 1) % kMaxInflightBuffers;

    // manage buffers
    if(
        Metal->CurrentFrameVertexBuffers[Metal->CurrentFrameIndex] == nil  ||
        Metal->CurretnFrameIndexBuffers[Metal->CurrentFrameIndex]  == nil  ||
        [Metal->CurrentFrameVertexBuffers[Metal->CurrentFrameIndex] length] < _VertexesCount * sizeof(Frenchie::Application::ApplicationRenderingBackendMeshVertex) ||
        [Metal->CurretnFrameIndexBuffers[Metal->CurrentFrameIndex] length]  < _IndexesCount * sizeof(Frenchie::Application::ApplicationRenderingBackendMeshVertexIndex))
    {
        if(Metal->CurrentFrameVertexBuffers[Metal->CurrentFrameIndex] != nil)
        {
            [Metal->CurrentFrameVertexBuffers[Metal->CurrentFrameIndex] release];
            Metal->CurrentFrameVertexBuffers[Metal->CurrentFrameIndex] = nil;
        }

        if(Metal->CurretnFrameIndexBuffers[Metal->CurrentFrameIndex] != nil)
        {
            [Metal->CurretnFrameIndexBuffers[Metal->CurrentFrameIndex] release];
            Metal->CurretnFrameIndexBuffers[Metal->CurrentFrameIndex] = nil;
        }

        Metal->CurrentFrameVertexBuffers[Metal->CurrentFrameIndex] = [Metal->Device newBufferWithBytes:
            _Vertexes
            length:_VertexesCount * sizeof(Frenchie::Application::ApplicationRenderingBackendMeshVertex)
            options:MTLResourceStorageModeShared];

        Metal->CurretnFrameIndexBuffers[Metal->CurrentFrameIndex] = [Metal->Device newBufferWithBytes:
            _Indexes
            length:_IndexesCount * sizeof(Frenchie::Application::ApplicationRenderingBackendMeshVertexIndex)
            options:MTLResourceStorageModeShared];
    }
    else
    {
        memcpy(
            Metal->CurrentFrameVertexBuffers[Metal->CurrentFrameIndex].contents,
            _Vertexes,
            _VertexesCount * sizeof(Frenchie::Application::ApplicationRenderingBackendMeshVertex));
        
        memcpy(
            Metal->CurretnFrameIndexBuffers[Metal->CurrentFrameIndex].contents,
            _Indexes,
            _IndexesCount * sizeof(Frenchie::Application::ApplicationRenderingBackendMeshVertexIndex));
    }

    [Metal->CurrentFrameVertexBuffers[Metal->CurrentFrameIndex]
        didModifyRange:NSMakeRange(
            0,
            _VertexesCount * sizeof(Frenchie::Application::ApplicationRenderingBackendMeshVertex))];

    [Metal->CurretnFrameIndexBuffers[Metal->CurrentFrameIndex]
        didModifyRange:NSMakeRange(
            0,
            _IndexesCount * sizeof(Frenchie::Application::ApplicationRenderingBackendMeshVertexIndex))];

    // retrieve surface from view layer
    Metal->DrawableSurface = [Metal->ClientWindowViewportLayer nextDrawable];

    // create render pass descriptor
    MTLRenderPassDescriptor* pass = [MTLRenderPassDescriptor renderPassDescriptor];

    // clear color
    gs_vec4f color = gs_vec4f(
        gs_color_rgba_get_r(Metal->ClearColor),
        gs_color_rgba_get_g(Metal->ClearColor),
        gs_color_rgba_get_b(Metal->ClearColor),
        gs_color_rgba_get_a(Metal->ClearColor)) / 255.f;

    pass.colorAttachments[0].clearColor  = MTLClearColorMake(color[0], color[1], color[2], color[3]);
    pass.colorAttachments[0].loadAction  = MTLLoadActionClear;
    pass.colorAttachments[0].storeAction = MTLStoreActionStore;
    pass.colorAttachments[0].texture     = Metal->DrawableSurface.texture;

    Metal->CommandBuffer  = [Metal->CommandQueue commandBuffer];
    Metal->CommandEncoder = [Metal->CommandBuffer renderCommandEncoderWithDescriptor:pass];


    [Metal->CommandEncoder setDepthStencilState:Metal->RendererDepthState];
    [Metal->CommandEncoder setRenderPipelineState:Metal->RendererPipeLineState];
    [Metal->CommandEncoder setVertexBuffer:Metal->CurrentFrameVertexBuffers[Metal->CurrentFrameIndex] offset:0 atIndex:0];

    // reset optional values
    Metal->Viewport.reset();

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
    std::shared_ptr<ApplicationRenderingBackendMetal> Metal = graphics_api<ApplicationRenderingBackendMetal>();

    if(Metal == nullptr || _Vertexes == nullptr || _Indexes == nullptr || _VertexesCount <= 0 || _IndexesCount <= 0)
        return;

    // setup shader uniforms
    {
        ApplicationRenderingBackendMetalShaderUniforms uniforms;

        for(int i = 0; i < _MeshProjectionMatrix.columns(); i++)
        {
            for(int j = 0; j < _MeshProjectionMatrix.rows(); j++)
                uniforms.Projection.columns[i][j] = _MeshProjectionMatrix[i][j];
        }

        [Metal->CommandEncoder setVertexBytes:
            &uniforms
            length:sizeof(ApplicationRenderingBackendMetalShaderUniforms)
            atIndex:1];
    }

    // bind texture
    {
        ApplicationRenderingBackendMetalTextureData* textureData =
            !_Texture.is_null() ?
                reinterpret_cast<ApplicationRenderingBackendMetalTextureData*>(_Texture.Ptr) : 
                    nullptr;

        if(textureData != nullptr)
        {
            [Metal->CommandEncoder setFragmentTexture:textureData->Texture atIndex:0];
            [Metal->CommandEncoder setFragmentSamplerState:textureData->SamplerState atIndex:0];
        }
    }

    // render primitives
    [Metal->CommandEncoder drawIndexedPrimitives:
        MTLPrimitiveTypeTriangle
        indexCount:_MeshIndexesCount - _MeshIndexesOffset
        indexType:sizeof(Frenchie::Application::ApplicationRenderingBackendMeshVertexIndex) == 2 ?
            MTLIndexTypeUInt16 :
                MTLIndexTypeUInt32
        indexBuffer:Metal->CurretnFrameIndexBuffers[Metal->CurrentFrameIndex]
        indexBufferOffset:_MeshIndexesOffset * sizeof(ApplicationRenderingBackendMeshVertexIndex)
        instanceCount:1
        baseVertex:0
        baseInstance:0];
}

void ApplicationRenderingBackend::end_render()
{
    std::shared_ptr<ApplicationRenderingBackendMetal> Metal = graphics_api<ApplicationRenderingBackendMetal>();

    if(Metal == nullptr)
        return;

    [Metal->CommandEncoder endEncoding];
    [Metal->CommandBuffer presentDrawable:Metal->DrawableSurface];
    [Metal->CommandBuffer addCompletedHandler:^(id<MTLCommandBuffer> commandBuffer) {dispatch_semaphore_signal(Metal->FrameSynchronizationSemaphore);}];
    [Metal->CommandBuffer commit];
}

void ApplicationRenderingBackend::set_viewport(const gs_vec2f& _Position, const gs_vec2f& _Size)
{
    std::shared_ptr<ApplicationRenderingBackendMetal> Metal = graphics_api<ApplicationRenderingBackendMetal>();

    if(Metal != nullptr)
        Metal->Viewport = gs_2dboxf(_Position, _Size);
}

void ApplicationRenderingBackend::clear_color(const gs_color& _Color)
{
    std::shared_ptr<ApplicationRenderingBackendMetal> Metal = graphics_api<ApplicationRenderingBackendMetal>();

    if(Metal != nullptr)
        Metal->ClearColor = _Color;
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
    gs_vec3f cameraWorldFrontAxisDirection = gs_vec3f(0.f, 0.f, +1.f);
    gs_vec3f cameraLocalFrontAxisDirection = gs_vector_normalize(cameraWorldFrontAxisDirection);
    gs_vec3f cameraLocalRightAxisDirection = gs_vector_normalize(gs_vector_cross(cameraLocalFrontAxisDirection, cameraWorldUpAxisDirection));
    gs_vec3f cameraLocalUpAxisDirection    = gs_vector_normalize(gs_vector_cross(cameraLocalRightAxisDirection, cameraLocalFrontAxisDirection));

    gs_mat4f cameraview =
        gs_matrix_look_at(
            gs_vec3f(0.f, 0.f, 1),
            gs_vec3f(0.f, 0.f, 1) + cameraLocalFrontAxisDirection,
            cameraLocalUpAxisDirection,
            false);
    
    gs_mat4f projection =
        gs_matrix_ortho(
            left,
            right,
            bottom,
            top,
            _CameraNearPlanePosition,
            _CameraFarPlanePosition,
            false,
            false) * gs_matrix_rotate(gs_mat4f(1.f), gs_to_radians(_CameraRotationAngle), gs_vec3f(0.f, 0.f, 1.f));

    return {cameraview, projection};
}

gs_mat4f ApplicationRenderingBackend::calculate_2d_transform_matrix(const float& _Depth, const gs_vec2f& _Position, const float& _Rotation, const gs_vec2f& _Scale)
{
    gs_mat4f matrix(1.f);

    return gs_matrix_translate(matrix, gs_vec3f(_Position, -_Depth)) *
            gs_matrix_rotate(matrix, gs_to_radians(_Rotation), gs_vec3f(0.f, 0.f, 1.f)) * 
            gs_matrix_scale(matrix, gs_vec3f(_Scale, 1.f));
}

bool ApplicationRenderingBackend::compare_objects_depths(const float& _A, const float& _B)
{
    return _A > _B;
}

gs_vec2f ApplicationRenderingBackend::convert_to_NDC(const gs_vec2f& _Position, const gs_vec2f& _Screen)
{
    return gs_vec2f((2.0f * _Position.x) / _Screen.x - 1.0f, 1.0f - (2.0f * _Position.y) / _Screen.y);
}