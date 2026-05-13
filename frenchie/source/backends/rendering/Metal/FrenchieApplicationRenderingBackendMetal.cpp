// Metal implementation
#define NS_PRIVATE_IMPLEMENTATION
#define CA_PRIVATE_IMPLEMENTATION
#define MTL_PRIVATE_IMPLEMENTATION
#include <Foundation/Foundation.hpp>
#include <Metal/Metal.hpp>
#include <QuartzCore/QuartzCore.hpp>

#include <CoreGraphics/CGGeometry.h>

// Application
#include <FrenchieApplicationPlatformBackend.hpp>
#include <FrenchieApplicationRenderingBackend.hpp>

#include <iostream>

// GLAD
using namespace Frenchie::Application;

namespace NS
{
	_NS_OPTIONS( NS::UInteger, WindowStyleMask )
	{
		WindowStyleMaskBorderless	   = 0,
		WindowStyleMaskTitled		   = ( 1 << 0 ),
		WindowStyleMaskClosable		 = ( 1 << 1 ),
		WindowStyleMaskMiniaturizable   = ( 1 << 2 ),
		WindowStyleMaskResizable		= ( 1 << 3 ),
		WindowStyleMaskTexturedBackground = ( 1 << 8 ),
		WindowStyleMaskUnifiedTitleAndToolbar = ( 1 << 12 ),
		WindowStyleMaskFullScreen	   = ( 1 << 14 ),
		WindowStyleMaskFullSizeContentView = ( 1 << 15 ),
		WindowStyleMaskUtilityWindow	= ( 1 << 4 ),
		WindowStyleMaskDocModalWindow   = ( 1 << 6 ),
		WindowStyleMaskNonactivatingPanel   = ( 1 << 7 ),
		WindowStyleMaskHUDWindow		= ( 1 << 13 )
	};

	_NS_ENUM( NS::UInteger, BackingStoreType )
	{
		BackingStoreRetained = 0,
		BackingStoreNonretained = 1,
		BackingStoreBuffered = 2
	};

	_NS_ENUM( NS::UInteger, ActivationPolicy )
	{
		ActivationPolicyRegular,
		ActivationPolicyAccessory,
		ActivationPolicyProhibited
	};
}

namespace NS::Private::Class {

_NS_PRIVATE_DEF_CLS(NSView);
_NS_PRIVATE_DEF_CLS(NSWindow);

} // Class

namespace NS::Private::Selector
{

_NS_PRIVATE_DEF_SEL( addItem_,
						"addItem:" );

_NS_PRIVATE_DEF_SEL( addItemWithTitle_action_keyEquivalent_,
						"addItemWithTitle:action:keyEquivalent:" );

_NS_PRIVATE_DEF_SEL( applicationDidFinishLaunching_,
						"applicationDidFinishLaunching:" );

_NS_PRIVATE_DEF_SEL( applicationShouldTerminateAfterLastWindowClosed_,
						"applicationShouldTerminateAfterLastWindowClosed:" );

_NS_PRIVATE_DEF_SEL( applicationWillFinishLaunching_,
						"applicationWillFinishLaunching:" );

_NS_PRIVATE_DEF_SEL( close,
						"close" );

_NS_PRIVATE_DEF_SEL( currentApplication,
						"currentApplication" );

_NS_PRIVATE_DEF_SEL( keyEquivalentModifierMask,
 						"keyEquivalentModifierMask" );

_NS_PRIVATE_DEF_SEL( localizedName,
						"localizedName" );

_NS_PRIVATE_DEF_SEL( sharedApplication,
						"sharedApplication" );

_NS_PRIVATE_DEF_SEL( setDelegate_,
						"setDelegate:" );

_NS_PRIVATE_DEF_SEL( setActivationPolicy_,
						"setActivationPolicy:" );

_NS_PRIVATE_DEF_SEL( activateIgnoringOtherApps_,
						"activateIgnoringOtherApps:" );

_NS_PRIVATE_DEF_SEL( run,
						"run" );

_NS_PRIVATE_DEF_SEL( terminate_,
						"terminate:" );

_NS_PRIVATE_DEF_SEL( initWithContentRect_styleMask_backing_defer_,
						"initWithContentRect:styleMask:backing:defer:" );

_NS_PRIVATE_DEF_SEL( initWithFrame_,
						"initWithFrame:" );

_NS_PRIVATE_DEF_SEL( initWithTitle_,
						"initWithTitle:" );

_NS_PRIVATE_DEF_SEL( setLayer_,
						"setLayer:" );
	
_NS_PRIVATE_DEF_SEL( setOpaque_,
						"setOpaque:" );

_NS_PRIVATE_DEF_SEL( setWantsLayer_,
						"setWantsLayer:" );
	
_NS_PRIVATE_DEF_SEL( contentView,
						"contentView" );
	
_NS_PRIVATE_DEF_SEL( setContentView_,
						"setContentView:" );

_NS_PRIVATE_DEF_SEL( makeKeyAndOrderFront_,
						"makeKeyAndOrderFront:" );

_NS_PRIVATE_DEF_SEL( setKeyEquivalentModifierMask_,
						"setKeyEquivalentModifierMask:" );

_NS_PRIVATE_DEF_SEL( setMainMenu_,
						"setMainMenu:" );

_NS_PRIVATE_DEF_SEL( setSubmenu_,
						"setSubmenu:" );

_NS_PRIVATE_DEF_SEL( setTitle_,
						"setTitle:" );

_NS_PRIVATE_DEF_SEL( windows,
						"windows" );

}

namespace NS
{
	class View : public NS::Referencing<View>
	{
    public:
        View* init(CGRect frame);
        void setLayer (CA::MetalLayer* layer);
        void setOpaque (bool opaque);
        void setWantsLayer (bool wantsLayer);
	};

    _NS_INLINE NS::View* NS::View::init( CGRect frame )
    {
        return Object::sendMessage<View*>( _NS_PRIVATE_CLS( NSView ), _NS_PRIVATE_SEL( initWithFrame_ ), frame );
    }

    _NS_INLINE void NS::View::setLayer( CA::MetalLayer* layer )
    {
        return Object::sendMessage< void >( this, _NS_PRIVATE_SEL( setLayer_ ), layer );
    }

    _NS_INLINE void NS::View::setOpaque( bool opaque )
    {
        return Object::sendMessage< void >( this, _NS_PRIVATE_SEL( setOpaque_ ), opaque );
    }

    _NS_INLINE void NS::View::setWantsLayer( bool wantsLayer )
    {
        return Object::sendMessage< void >( this, _NS_PRIVATE_SEL( setWantsLayer_ ), wantsLayer );
    }
};

namespace NS
{
	class Window : public Referencing< Window >
	{
		public:
			static Window*		alloc();
			Window*				init( CGRect contentRect, WindowStyleMask styleMask, BackingStoreType backing, bool defer );

			View*				contentView() const;
			void				setContentView( const View* pContentView );
			void				makeKeyAndOrderFront( const Object* pSender );
			void				setTitle( const String* pTitle );
			void				close();
	};

    _NS_INLINE NS::Window* NS::Window::alloc()
    {
        return Object::sendMessage< Window* >( _NS_PRIVATE_CLS( NSWindow ), _NS_PRIVATE_SEL( alloc ) );
    }

    _NS_INLINE NS::Window* NS::Window::init( CGRect contentRect, WindowStyleMask styleMask, BackingStoreType backing, bool defer )
    {
        return Object::sendMessage< Window* >( this, _NS_PRIVATE_SEL( initWithContentRect_styleMask_backing_defer_ ), contentRect, styleMask, backing, defer );
    }

    _NS_INLINE NS::View* NS::Window::contentView() const
    {
        return Object::sendMessage< View* >( this, _NS_PRIVATE_SEL( contentView ));
    }

    _NS_INLINE void NS::Window::setContentView( const NS::View* pContentView )
    {
        Object::sendMessage< void >( this, _NS_PRIVATE_SEL( setContentView_ ), pContentView );
    }

    _NS_INLINE void NS::Window::makeKeyAndOrderFront( const Object* pSender )
    {
        Object::sendMessage< void >( this, _NS_PRIVATE_SEL( makeKeyAndOrderFront_ ), pSender );
    }

    _NS_INLINE void NS::Window::setTitle( const String* pTitle )
    {
        Object::sendMessage< void >( this, _NS_PRIVATE_SEL( setTitle_), pTitle );
    }

    _NS_INLINE void NS::Window::close()
    {
        Object::sendMessage< void >( this, _NS_PRIVATE_SEL( close ) );
    }
}

namespace Frenchie
{
    namespace Application
    {
        struct ApplicationRenderingBackendMetal : public ApplicationRenderingBackendGraphicsApi
        {
            ApplicationRenderingBackendMetal(){}
            virtual ~ApplicationRenderingBackendMetal(){}

            MTL::Device*              Device           {nullptr};
            MTL::Buffer*              VertexBuffer     {nullptr};
            MTL::Buffer*              IndexBuffer      {nullptr};
            MTL::CommandQueue*        CommandQueue     {nullptr};
            MTL::RenderPipelineState* RenderingPipeLine{nullptr};
            
            NS::AutoreleasePool*      Pool{nullptr};
            CA::MetalLayer* Layer{nullptr};
            CA::MetalDrawable* Sufrace{nullptr};

            MTL::CommandBuffer*        CommandBuffer {nullptr};
            MTL::RenderPassDescriptor* RenderPass{nullptr};
            MTL::RenderCommandEncoder* Encoder{nullptr};
        };
    }
}

bool ApplicationRenderingBackend::awake(const std::any& _Stuff)
{
    // retrieve window
    NS::Window* nswindow = nullptr;
    NS::View*   nsview   = nullptr;

    try
    {
        nswindow = reinterpret_cast<NS::Window*>(std::any_cast<id>(_Stuff));
        nsview   = nswindow->contentView();
    }
    catch(const std::exception& e)
    {
        return false;
    }

    std::cout << "successfully retrieved window !!! \n";

    // create API
    m_Api = std::make_shared<ApplicationRenderingBackendMetal>();

    auto Metal = graphics_api<ApplicationRenderingBackendMetal>();

    if(Metal == nullptr)
        return false;

    // create device
    Metal->Device       = MTL::CreateSystemDefaultDevice();
    Metal->Layer        = CA::MetalLayer::layer();
    Metal->CommandQueue = Metal->Device->newCommandQueue();

    Metal->Layer->setDevice(Metal->Device);

    nsview->setLayer(Metal->Layer);
    nsview->setWantsLayer(true);
    nsview->setOpaque(true);

    // compile shaders
    NS::Error* error = nullptr;

    MTL::Library* shaderProgramm = Metal->Device->newLibrary(
        NS::String::string(
R"(
#include <metal_stdlib>
#include <simd/simd.h>
using namespace metal;

struct VertexIn
{
    float3 Position [[attribute(0)]];
    float3 Normal [[attribute(1)]];
    float3 UV [[attribute(2)]];
    uint   Color [[attribute(3)]];
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

// vertex VertexOut vertex_main(
//     const VertexIn _Input [[stage_in]],
//     constant Uniforms& _Uniforms [[buffer(1)]])
// {
//     VertexOut out;
//     out.Position = float4(_Input.Position, 1.f);
//     out.Normal   = _Input.Normal;
//     out.UV       = _Input.UV;
//     out.Color    = unpack_unorm4x8_to_float(_Input.Color);
//     return out;
// }

vertex VertexOut vertex_main(const VertexIn _Input [[stage_in]])
{
    VertexOut out;
    out.Position = float4(_Input.Position, 1.f);
    out.Normal   = _Input.Normal;
    out.UV       = _Input.UV;
    out.Color    = unpack_unorm4x8_to_float(_Input.Color);
    return out;
}

fragment uint4 fragment_main(VertexOut _Input [[stage_in]])
{
    return uint4(_Input.Color * 255.0);
}    
)",
    NS::ASCIIStringEncoding),
    nullptr,
    &error);

    if (error != nullptr)
    {
       std::cout << error->localizedDescription()->utf8String() << std::endl;
    }

    if (shaderProgramm == nullptr)
    {
        return false;
    }

    std::cout << "SHADERS COMPILATION SUCCEEDED !!! \n";

    MTL::Function* vertexShader = shaderProgramm->newFunction(NS::String::string("vertex_main", NS::ASCIIStringEncoding));
    MTL::Function* pixelShader  = shaderProgramm->newFunction(NS::String::string("fragment_main", NS::ASCIIStringEncoding));

    // create vertex descriptor
    MTL::VertexDescriptor* vertexDescriptor = MTL::VertexDescriptor::alloc()->init();

    // position
    vertexDescriptor->attributes()->object(0)->setFormat(MTL::VertexFormatFloat3);
    vertexDescriptor->attributes()->object(0)->setOffset(0);
    vertexDescriptor->attributes()->object(0)->setBufferIndex(0);

    // normal
    vertexDescriptor->attributes()->object(1)->setFormat(MTL::VertexFormatFloat3);
    vertexDescriptor->attributes()->object(1)->setOffset(sizeof(float) * 3);
    vertexDescriptor->attributes()->object(1)->setBufferIndex(0);

    // UV
    vertexDescriptor->attributes()->object(2)->setFormat(MTL::VertexFormatFloat2);
    vertexDescriptor->attributes()->object(2)->setOffset(sizeof(float) * 3 + sizeof(float) * 3);
    vertexDescriptor->attributes()->object(2)->setBufferIndex(0);

    // Color
    vertexDescriptor->attributes()->object(3)->setFormat(MTL::VertexFormatUInt);
    vertexDescriptor->attributes()->object(3)->setOffset(sizeof(float) * 3 + sizeof(float) * 3 + sizeof(float) * 2);
    vertexDescriptor->attributes()->object(3)->setBufferIndex(0);

    std::cout << sizeof(float) * 3 + sizeof(float) * 3 + sizeof(float) * 2 + sizeof(gs_color) << "\t" << sizeof(ApplicationRenderingBackendMeshVertex) << "\n";

    // Layout
    vertexDescriptor->layouts()->object(0)->setStride(sizeof(ApplicationRenderingBackendMeshVertex));
    vertexDescriptor->layouts()->object(0)->setStepFunction(MTL::VertexStepFunctionPerVertex);
    vertexDescriptor->layouts()->object(0)->setStepRate(1);

    // create renderer pipeline
    MTL::RenderPipelineDescriptor* renderPipelineDescriptor = MTL::RenderPipelineDescriptor::alloc()->init();
    renderPipelineDescriptor->setLabel(NS::String::string("Indexed mesh rendering pipeline", NS::ASCIIStringEncoding));
    renderPipelineDescriptor->setVertexFunction(vertexShader);
    renderPipelineDescriptor->setFragmentFunction(pixelShader);
    renderPipelineDescriptor->setVertexDescriptor(vertexDescriptor);
    renderPipelineDescriptor->setDepthAttachmentPixelFormat(MTL::PixelFormat::PixelFormatDepth32Float);
    renderPipelineDescriptor->colorAttachments()->object(0)->setPixelFormat(MTL::PixelFormat::PixelFormatRGBA8Uint);

    Metal->RenderingPipeLine = Metal->Device->newRenderPipelineState(renderPipelineDescriptor, &error);

    if (error != nullptr)
    {
       std::cout << "pipeline errors:\n";
       std::cout << error->localizedDescription()->utf8String() << std::endl;
    }

    if(renderPipelineDescriptor)
        renderPipelineDescriptor->release();

    if(vertexShader)
        vertexShader->release();

    if(pixelShader)
        pixelShader->release();

    if(vertexDescriptor)
        vertexDescriptor->release();

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

    if(Metal->RenderingPipeLine != nullptr)
    {
        Metal->RenderingPipeLine->release();
        Metal->RenderingPipeLine = nullptr;
    }

    if(Metal->CommandQueue != nullptr)
    {
        Metal->CommandQueue->release();
        Metal->CommandQueue = nullptr;
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

        // create new resized buffers
        Metal->VertexBuffer = Metal->Device->newBuffer(
            _Vertexes,
            _VertexesCount * sizeof(ApplicationRenderingBackendMeshVertex),
            MTL::ResourceStorageModeShared);

        Metal->IndexBuffer = Metal->Device->newBuffer(
            _Indexes,
            _IndexesCount * sizeof(ApplicationRenderingBackendMeshVertexIndex),
            MTL::ResourceStorageModeShared);
    }
    else
    {
        memcpy(Metal->VertexBuffer->contents(), _Vertexes, _VertexesCount * sizeof(ApplicationRenderingBackendMeshVertex));
        memcpy(Metal->IndexBuffer->contents(), _Indexes, _IndexesCount * sizeof(ApplicationRenderingBackendMeshVertexIndex));        
    }
    

    Metal->Pool       = NS::AutoreleasePool::alloc()->init();
    Metal->Sufrace    = Metal->Layer->nextDrawable();
    Metal->RenderPass = MTL::RenderPassDescriptor::renderPassDescriptor();

    auto passColorAttachment0 = Metal->RenderPass->colorAttachments()->object(0);
    passColorAttachment0->setClearColor(MTL::ClearColor(0.5, 0.5, 0.5, 1));
    passColorAttachment0->setLoadAction(MTL::LoadActionClear);
    passColorAttachment0->setStoreAction(MTL::StoreActionStore);
    passColorAttachment0->setTexture(Metal->Sufrace->texture());

    Metal->CommandBuffer = Metal->CommandQueue->commandBuffer();
    Metal->Encoder       = Metal->CommandBuffer->renderCommandEncoder(Metal->RenderPass);

    Metal->Encoder->setRenderPipelineState(Metal->RenderingPipeLine);
    Metal->Encoder->setVertexBuffer(Metal->VertexBuffer, NS::UInteger(0), NS::UInteger(0));

    GS_ASSERT(Metal->Encoder);

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

    if(Metal == nullptr || _Vertexes == nullptr || _Indexes == nullptr || _VertexesCount <= 0 || _IndexesCount <= 0 || _MeshVertexesCount <= 0 || _MeshIndexesCount <= 0)
        return;

    Metal->Encoder->drawIndexedPrimitives(
        MTL::PrimitiveType::PrimitiveTypeTriangle,
        _MeshIndexesCount,
        sizeof(ApplicationRenderingBackendMeshVertexIndex) == 2 ? 
            MTL::IndexType::IndexTypeUInt16 :
                MTL::IndexType::IndexTypeUInt32,
        Metal->IndexBuffer,
        _MeshIndexesOffset);
}

void ApplicationRenderingBackend::end_render()
{
    std::shared_ptr<ApplicationRenderingBackendMetal> Metal = graphics_api<ApplicationRenderingBackendMetal>();

    if (Metal == nullptr)
        return;

    Metal->Encoder->endEncoding();
    Metal->CommandBuffer->presentDrawable(Metal->Sufrace);
    Metal->CommandBuffer->commit();
    Metal->Pool->release();
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