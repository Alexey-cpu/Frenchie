// Application
#include <FrenchieApplicationPlatformBackend.hpp>
#include <FrenchieApplicationRenderingBackend.hpp>

// D3D9
#include <d3d9.h>
#pragma warning( disable : 4996 ) // disable deprecated warning 
#include <strsafe.h>
#pragma warning( default : 4996 )

#include <iostream>
#include <queue>

using namespace Frenchie::Application;

namespace Frenchie
{
    namespace Application
    {
        struct ApplicationRenderingBackendDirectX9 : public ApplicationRenderingBackendGraphicsApi
        {
            ApplicationRenderingBackendDirectX9(){}
            virtual ~ApplicationRenderingBackendDirectX9(){}

            // default rendering pipeline properties
            mutable LPDIRECT3D9                         m_D3D                  = NULL;                        // D3D interface
            mutable LPDIRECT3DDEVICE9                   m_Device               = NULL;                        // rendering device
            mutable LPDIRECT3DVERTEXBUFFER9             m_VertexBuffer         = NULL;                        // vertex buffer
            mutable UINT                                m_VertexBufferSize     = 0;                           // vertex buffer size
            mutable LPDIRECT3DINDEXBUFFER9              m_IndexBuffer          = NULL;                        // index buffer
            mutable UINT                                m_IndexBufferSize      = 0;                           // index buffer size
            mutable IDirect3DVertexDeclaration9*        m_VertexDeclaration    = NULL;                        // vertex layout
            mutable IDirect3DStateBlock9*               m_RendererState        = NULL;                        // D3D device renderer state
            mutable gs_color                            m_ClearColor           = gs_color_rgba(0, 0, 0, 0);   // D3D device renderer clear color
            mutable std::optional<gs_2d_boxf>           m_Viewport             = std::optional<gs_2d_boxf>(); // D3D device viewport 
            mutable bool                                m_DeviceLost           = false;                       // D3D device lost event boolean handler
            mutable D3DPRESENT_PARAMETERS               m_PresentParameters    = D3DPRESENT_PARAMETERS();     // D3D device scene present parameters

            // framebuffering rendering pipeline properties
            IDirect3DTexture9*                          m_RenderTargetTexture  = NULL; // D3D render target texture
            IDirect3DSurface9*                          m_RenderTargetSurface  = NULL; // D3D render target texture surface
            IDirect3DSurface9*                          m_DepthStencilSurface  = NULL; // D3D render stencil surface
            IDirect3DSurface9*                          m_PreviousRenderTarget = NULL; // D3D render previous target texture
            IDirect3DSurface9*                          m_PreviousDepthStencil = NULL; // D3D render previous stencil surface
        };

        D3DMATRIX gs_convert_transform_from_opengl_to_directx(const gs_mat4f& _Matrix)
        {
            D3DMATRIX result;

            for (int i = 0; i < _Matrix.columns(); i++)
            {
                for (int j = 0; j < _Matrix.rows(); j++)
                {
                    result.m[i][j] = _Matrix[i][j];
                }
            }

            return result;
        }

        typedef Frenchie::Application::ApplicationRenderingBackendMeshVertex      CUSTOMVERTEX;
        typedef Frenchie::Application::ApplicationRenderingBackendMeshVertexIndex CUSTOMINDEX;
    }
}

bool ApplicationRenderingBackend::awake(const std::any& _Stuff)
{
    HWND hWnd;

    try
    {
        hWnd = std::any_cast<HWND>(_Stuff);
    }
    catch(...)
    {
        return false;
    }
    
    std::shared_ptr<ApplicationRenderingBackendDirectX9> DirectX9 =
        std::dynamic_pointer_cast<ApplicationRenderingBackendDirectX9>(m_Api = std::make_shared<ApplicationRenderingBackendDirectX9>());

    // Create the D3D object.
    if(NULL == (DirectX9->m_D3D = Direct3DCreate9(D3D_SDK_VERSION)))
        return false;

    // Set up the structure used to create the D3DDevice
    ZeroMemory(&DirectX9->m_PresentParameters, sizeof(DirectX9->m_PresentParameters));
    DirectX9->m_PresentParameters.Windowed               = TRUE;
    DirectX9->m_PresentParameters.SwapEffect             = D3DSWAPEFFECT_DISCARD;
    DirectX9->m_PresentParameters.BackBufferFormat       = D3DFMT_A8R8G8B8;
    DirectX9->m_PresentParameters.EnableAutoDepthStencil = TRUE;
    DirectX9->m_PresentParameters.AutoDepthStencilFormat = D3DFMT_D24S8; 

    // Create the D3DDevice
    if(FAILED(DirectX9->m_D3D->CreateDevice(D3DADAPTER_DEFAULT, D3DDEVTYPE_HAL, hWnd, D3DCREATE_SOFTWARE_VERTEXPROCESSING, &DirectX9->m_PresentParameters, &DirectX9->m_Device)))
        return false;

    return true;
}

void ApplicationRenderingBackend::begin_render(ApplicationRenderingBackendRenderingTarget* _Target)
{
    std::shared_ptr<ApplicationRenderingBackendDirectX9> DirectX9 = graphics_api<ApplicationRenderingBackendDirectX9>();
    
    if(DirectX9 == nullptr || DirectX9->m_Device == nullptr)
        return;

    // handle lost D3D9 device
    if (DirectX9->m_DeviceLost)
    {
        // wait while device is lost
        HRESULT hr = DirectX9->m_Device->TestCooperativeLevel();

        if (hr == D3DERR_DEVICELOST)
        {
            ::Sleep(10);
            return;
        }

        // clean-up device if it has not been cleaned earlier
        if (hr == D3DERR_DEVICENOTRESET)
        {
            // release resources
            if(DirectX9->m_VertexBuffer != NULL)
            {
                DirectX9->m_VertexBuffer->Release();
                DirectX9->m_VertexBuffer = NULL;
            }

            // release index buffer
            if(DirectX9->m_IndexBuffer != NULL)
            {
                DirectX9->m_IndexBuffer->Release();
                DirectX9->m_IndexBuffer = NULL;
            }

            // release vertex declaration
            if(DirectX9->m_VertexDeclaration != NULL)
            {
                DirectX9->m_VertexDeclaration->Release();
                DirectX9->m_VertexDeclaration = NULL;
            }
        }

        DirectX9->m_DeviceLost = false;
    }

    // handle viewport resize
    if(DirectX9->m_Viewport.has_value())
    {
        // clean-up device

        // release resources
        if(DirectX9->m_VertexBuffer != NULL)
        {
            DirectX9->m_VertexBuffer->Release();
            DirectX9->m_VertexBuffer = NULL;
        }

        // release index buffer
        if(DirectX9->m_IndexBuffer != NULL)
        {
            DirectX9->m_IndexBuffer->Release();
            DirectX9->m_IndexBuffer = NULL;
        }

        // release vertex declaration
        if(DirectX9->m_VertexDeclaration != NULL)
        {
            DirectX9->m_VertexDeclaration->Release();
            DirectX9->m_VertexDeclaration = NULL;
        }

        // adjust backbuffer and reset device
        DirectX9->m_PresentParameters.BackBufferWidth  = DirectX9->m_Viewport.value().size().x;
        DirectX9->m_PresentParameters.BackBufferHeight = DirectX9->m_Viewport.value().size().y;
        HRESULT r = DirectX9->m_Device->Reset(&DirectX9->m_PresentParameters);

        // adjust viewport
        D3DVIEWPORT9 vp;
        vp.X      = DirectX9->m_Viewport.value().Min.x;
        vp.Y      = DirectX9->m_Viewport.value().Min.y;
        vp.Width  = DirectX9->m_Viewport.value().size().x;
        vp.Height = DirectX9->m_Viewport.value().size().y;
        vp.MinZ   = 0.0f;
        vp.MaxZ   = 1.0f;
        DirectX9->m_Device->SetViewport(&vp);

        DirectX9->m_Viewport.reset();
    }

    // backup the DX9 state
    if(DirectX9->m_RendererState != NULL)
    {
        DirectX9->m_RendererState->Release();
        DirectX9->m_RendererState = NULL;
    }

    if (FAILED(DirectX9->m_Device->CreateStateBlock(D3DSBT_ALL, &DirectX9->m_RendererState)) || FAILED(DirectX9->m_RendererState->Capture()))
        return;

    // manage vertex declaration
    if(DirectX9->m_VertexDeclaration == NULL)
    {
        // Declare mesh vertex
        D3DVERTEXELEMENT9 VertexColElements[] =
        {
            {0, sizeof(float) * 0, D3DDECLTYPE_FLOAT3, D3DDECLMETHOD_DEFAULT, D3DDECLUSAGE_POSITION, 0},
            {0, sizeof(float) * 3, D3DDECLTYPE_FLOAT2, D3DDECLMETHOD_DEFAULT, D3DDECLUSAGE_TEXCOORD, 0},
            {0, sizeof(float) * 5, D3DDECLTYPE_D3DCOLOR, D3DDECLMETHOD_DEFAULT, D3DDECLUSAGE_COLOR,  0},
            D3DDECL_END(),
        };

        if(FAILED(DirectX9->m_Device->CreateVertexDeclaration(VertexColElements, &DirectX9->m_VertexDeclaration)))
            return;

        DirectX9->m_Device->SetVertexDeclaration(DirectX9->m_VertexDeclaration);
        DirectX9->m_Device->SetPixelShader(nullptr);
        DirectX9->m_Device->SetVertexShader(nullptr);
    }

    // manage previous render targets
    DirectX9->m_Device->GetRenderTarget(0, &DirectX9->m_PreviousRenderTarget);
    DirectX9->m_Device->GetDepthStencilSurface(&DirectX9->m_PreviousDepthStencil);

    // setup new render targets
    if((DirectX9->m_RenderingTarget = _Target) != nullptr)
    {
        // retrieve framebuffer size
        int width  = gs_clamp<float>(ApplicationPlatformBackend::get_window_size().x, 800.f, 800.f);
        int height = gs_clamp<float>(ApplicationPlatformBackend::get_window_size().y, 600.f, 600.f);

        // create a render target texture in the default Pool
        if(FAILED(DirectX9->m_Device->CreateTexture(width, height, 1, D3DUSAGE_RENDERTARGET, D3DFMT_A8R8G8B8, D3DPOOL_DEFAULT, &DirectX9->m_RenderTargetTexture, NULL)))
            return;

        // get the top-level surface to use as the actual frame buffer target and create stencil render target
        DirectX9->m_RenderTargetTexture->GetSurfaceLevel(0, &DirectX9->m_RenderTargetSurface);
        if(FAILED(DirectX9->m_Device->CreateDepthStencilSurface(width, height, D3DFMT_D24S8, D3DMULTISAMPLE_NONE, 0, TRUE, &DirectX9->m_DepthStencilSurface, NULL)))
            return;

        DirectX9->m_Device->SetRenderTarget(0, DirectX9->m_RenderTargetSurface);
        DirectX9->m_Device->SetDepthStencilSurface(DirectX9->m_DepthStencilSurface);
    }

    // setup current render state
    DirectX9->m_Device->SetRenderState(D3DRS_SHADEMODE, D3DSHADE_GOURAUD);
    DirectX9->m_Device->SetRenderState(D3DRS_ZENABLE, D3DZB_TRUE);
    DirectX9->m_Device->SetRenderState(D3DRS_ZWRITEENABLE, TRUE);
    DirectX9->m_Device->SetRenderState(D3DRS_ZFUNC, D3DCMP_LESS);

    DirectX9->m_Device->SetRenderState(D3DRS_ALPHATESTENABLE, TRUE);
    DirectX9->m_Device->SetRenderState(D3DRS_CULLMODE, D3DCULL_NONE); // I DON'T KNOW HOW TO ENABLE CULLING HERE CORRECTLY ...
    
    // alpha blending
    DirectX9->m_Device->SetRenderState(D3DRS_ALPHABLENDENABLE, TRUE);
    DirectX9->m_Device->SetRenderState(D3DRS_SRCBLEND, D3DBLEND_SRCALPHA);
    DirectX9->m_Device->SetRenderState(D3DRS_DESTBLEND, D3DBLEND_INVSRCALPHA);
    DirectX9->m_Device->SetRenderState(D3DRS_BLENDOP, D3DBLENDOP_ADD);
    
    DirectX9->m_Device->SetRenderState(D3DRS_SCISSORTESTENABLE, TRUE);
    DirectX9->m_Device->SetRenderState(D3DRS_FOGENABLE, FALSE);
    DirectX9->m_Device->SetRenderState(D3DRS_RANGEFOGENABLE, FALSE);
    DirectX9->m_Device->SetRenderState(D3DRS_SPECULARENABLE, FALSE);
    DirectX9->m_Device->SetRenderState(D3DRS_STENCILENABLE, TRUE);
    DirectX9->m_Device->SetRenderState(D3DRS_CLIPPING, TRUE);
    DirectX9->m_Device->SetRenderState(D3DRS_LIGHTING, FALSE);

    DirectX9->m_Device->SetTextureStageState(0, D3DTSS_COLOROP, D3DTOP_MODULATE);
    DirectX9->m_Device->SetTextureStageState(0, D3DTSS_COLORARG1, D3DTA_TEXTURE);
    DirectX9->m_Device->SetTextureStageState(0, D3DTSS_COLORARG2, D3DTA_DIFFUSE);
    
    DirectX9->m_Device->SetTextureStageState(0, D3DTSS_ALPHAOP, D3DTOP_MODULATE);
    DirectX9->m_Device->SetTextureStageState(0, D3DTSS_ALPHAARG1, D3DTA_TEXTURE);
    DirectX9->m_Device->SetTextureStageState(0, D3DTSS_ALPHAARG2, D3DTA_DIFFUSE);

    // DirectX9->g_D3DDevice->SetTextureStageState(1, D3DTSS_COLOROP, D3DTOP_DISABLE);
    // DirectX9->g_D3DDevice->SetTextureStageState(1, D3DTSS_ALPHAOP, D3DTOP_DISABLE);

    // clear back buffer
    DirectX9->m_Device->Clear(
        0,
        NULL,
        D3DCLEAR_TARGET | D3DCLEAR_ZBUFFER | D3DCLEAR_STENCIL,
        D3DCOLOR_RGBA(
            gs_color_rgba_get_r(DirectX9->m_ClearColor),
            gs_color_rgba_get_g(DirectX9->m_ClearColor),
            gs_color_rgba_get_b(DirectX9->m_ClearColor),
            gs_color_rgba_get_a(DirectX9->m_ClearColor)),
        1.0f,
        0);
}

void ApplicationRenderingBackend::end_render()
{
    std::shared_ptr<ApplicationRenderingBackendDirectX9> DirectX9 = graphics_api<ApplicationRenderingBackendDirectX9>();

    if(DirectX9 == nullptr)
        return;

    DirectX9->m_Device->EndScene();

    // Present the backbuffer contents to the display if there is no any target texture
    if(DirectX9->m_RenderingTarget == NULL)
    {
        HRESULT result = DirectX9->m_Device->Present(NULL, NULL, NULL, NULL);
        if (result == D3DERR_DEVICELOST)
            DirectX9->m_DeviceLost = true;
    }

    // Restore the DX9 state
    if(DirectX9->m_RendererState != NULL)
    {
        DirectX9->m_RendererState->Apply();
        DirectX9->m_RendererState->Release();
        DirectX9->m_RendererState = NULL;
    }

    // create frame buffer texture
    if(DirectX9->m_RenderingTarget != NULL && DirectX9->m_RenderTargetSurface != NULL)
    {
        // create a system memory surface for the CPU copy
        IDirect3DSurface9* pSystemSurf = nullptr;
        D3DSURFACE_DESC desc;
        DirectX9->m_RenderTargetSurface->GetDesc(&desc);

        DirectX9->m_Device->CreateOffscreenPlainSurface(desc.Width, desc.Height, desc.Format, D3DPOOL_SYSTEMMEM, &pSystemSurf, NULL);

        // copy from GPU to CPU memory
        DirectX9->m_Device->GetRenderTargetData(DirectX9->m_RenderTargetSurface, pSystemSurf);

        // create the D3DPOOL_MANAGED destination texture
        LPDIRECT3DTEXTURE9 pManagedTexture = nullptr;
        DirectX9->m_Device->CreateTexture(desc.Width, desc.Height, 1, 0, desc.Format, D3DPOOL_MANAGED, &pManagedTexture, NULL);

        IDirect3DSurface9* pManagedSurf = nullptr;
        pManagedTexture->GetSurfaceLevel(0, &pManagedSurf);

        // lock and copy data
        D3DLOCKED_RECT lockedSysRect, lockedManagedRect;
        pSystemSurf->LockRect(&lockedSysRect, NULL, D3DLOCK_READONLY);
        pManagedSurf->LockRect(&lockedManagedRect, NULL, 0);

        // copy row-by-row to handle potential pitch differences
        BYTE* pSrc = (BYTE*)lockedSysRect.pBits;
        BYTE* pDst = (BYTE*)lockedManagedRect.pBits;
        DWORD pitch = min(lockedSysRect.Pitch, lockedManagedRect.Pitch);

        for (UINT i = 0; i < desc.Height; ++i)
        {
            memcpy(pDst, pSrc, pitch);
            pSrc += lockedSysRect.Pitch;
            pDst += lockedManagedRect.Pitch;
        }

        pManagedSurf->UnlockRect();
        pSystemSurf->UnlockRect();

        // destroy old frames
        if(DirectX9->m_RenderingTarget->Frames.size() > 2)
        {
            destroy_texture(DirectX9->m_RenderingTarget->Frames.front());
            DirectX9->m_RenderingTarget->Frames.pop_front();
        }

        // push current frame to the frame buffer
        DirectX9->m_RenderingTarget->Frames.push_back(
            ApplicationRenderingBackendTexture(
                reinterpret_cast<uintptr_t>(pManagedTexture),
                desc.Width,
                desc.Height,
                gs_color_rgba(255, 255, 255, 255)));

        // cleanup
        pSystemSurf->Release();
        pManagedSurf->Release();
    }

    // clean-up framebuffer rendering pipeline and restore previous rendering target
    if(DirectX9->m_RenderTargetTexture != NULL)
        DirectX9->m_RenderTargetTexture->Release();
    DirectX9->m_RenderTargetTexture = NULL;

    if(DirectX9->m_RenderTargetSurface != NULL)
        DirectX9->m_RenderTargetSurface->Release();
    DirectX9->m_RenderTargetSurface = NULL;

    if(DirectX9->m_DepthStencilSurface != NULL)
        DirectX9->m_DepthStencilSurface->Release();
    DirectX9->m_DepthStencilSurface = NULL;

    if(DirectX9->m_PreviousRenderTarget != NULL)
    {
        DirectX9->m_Device->SetRenderTarget(0, DirectX9->m_PreviousRenderTarget);
        DirectX9->m_PreviousRenderTarget->Release();
        DirectX9->m_PreviousRenderTarget = NULL;
    }

    if(DirectX9->m_PreviousDepthStencil != NULL)
    {
        DirectX9->m_Device->SetDepthStencilSurface(DirectX9->m_PreviousDepthStencil);
        DirectX9->m_PreviousDepthStencil->Release();
        DirectX9->m_PreviousDepthStencil = NULL;
    }
}

void ApplicationRenderingBackend::quit()
{
    std::shared_ptr<ApplicationRenderingBackendDirectX9> DirectX9 = graphics_api<ApplicationRenderingBackendDirectX9>();

    if(DirectX9 == nullptr)
        return;

    // clean-up famebuffering pipeline
    if(DirectX9->m_RenderTargetTexture != NULL)
        DirectX9->m_RenderTargetTexture->Release();

    if(DirectX9->m_RenderTargetSurface != NULL)
        DirectX9->m_RenderTargetSurface->Release();

    if(DirectX9->m_DepthStencilSurface != NULL)
        DirectX9->m_DepthStencilSurface->Release();

    // clean-up default rendering pipeline
    if(DirectX9->m_DefaultFont.has_value())
        destroy_font(DirectX9->m_DefaultFont.value());

    if(DirectX9->m_DefaultTexture.has_value())
        destroy_texture(DirectX9->m_DefaultTexture.value());

    if(DirectX9->m_VertexBuffer != NULL)
        DirectX9->m_VertexBuffer->Release();

    if(DirectX9->m_IndexBuffer != NULL)
        DirectX9->m_IndexBuffer->Release();

    if(DirectX9->m_Device != NULL)
        DirectX9->m_Device->Release();

    if(DirectX9->m_RendererState != NULL)
        DirectX9->m_RendererState->Release();

    if(DirectX9->m_D3D != NULL)
        DirectX9->m_D3D->Release();
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
    (void)_RawBuffer;
    (void)_Width;
    (void)_Height;
    (void)_Format;
    (void)_Wrap;
    (void)_MinFilter;
    (void)_MaxFilter;

    std::shared_ptr<ApplicationRenderingBackendDirectX9> DirectX9 = graphics_api<ApplicationRenderingBackendDirectX9>();

    if(DirectX9 == nullptr || DirectX9->m_Device == nullptr)
        return ApplicationRenderingBackendTexture();

    std::shared_ptr<unsigned char> image = std::shared_ptr<unsigned char>(new unsigned char[_Width * _Height * 4]);

    const int height   = _Height;
    const int width    = _Width;
    const int channels = _Format == ApplicationRenderingBackendTextureFormat_::ApplicationRenderingBackendTextureFormat_RGBA ? 4 : _Format == ApplicationRenderingBackendTextureFormat_::ApplicationRenderingBackendTextureFormat_RGB ? 3 : 1;

    for (int y = 0; y < height; y++)
    {
        for (int x = 0; x < width; x++)
        {
            for (int z = 0; z < channels; z++)
            {
                image.get()[channels * (y * width + x) + (channels - z - 1)] = _RawBuffer[channels * (y * width + x) + z];
            }
        }
    }

    // create texture
    LPDIRECT3DTEXTURE9 pTexture = nullptr;
    DirectX9->m_Device->CreateTexture(_Width, _Height, 1, 0, D3DFMT_A8R8G8B8, D3DPOOL_MANAGED, &pTexture, nullptr);

    // lock texture rect
    D3DLOCKED_RECT lockedRect;
    pTexture->LockRect(0, &lockedRect, nullptr, D3DLOCK_DISCARD);

    // copy Data (Row by Row to handle pitch)
    unsigned char* pDest = (unsigned char*)lockedRect.pBits;
    const unsigned char* pSrc = image.get();

    int stride = _Width * 4; // 4 bytes per pixel

    for (int row = 0; row < _Height; ++row)
    {
        memcpy(pDest, pSrc, stride);
        pDest += lockedRect.Pitch;
        pSrc += stride;
    }

    // unlock rect
    pTexture->UnlockRect(0);

    return ApplicationRenderingBackendTexture(reinterpret_cast<uintptr_t>(pTexture), _Width, _Height, gs_color_rgba(255, 255, 255, 255), _Format, _Wrap, _MinFilter, _MaxFilter);
}

void ApplicationRenderingBackend::destroy_texture(const ApplicationRenderingBackendTexture& _Texture)
{
    if(_Texture.is_null())
        return;

    reinterpret_cast<LPDIRECT3DTEXTURE9>(_Texture.Ptr)->Release();
    _Texture.Ptr = 0;
}

bool ApplicationRenderingBackend::load_mesh(
    const ApplicationRenderingBackendMeshVertex*      _Vertexes,
    const ApplicationRenderingBackendMeshVertexIndex& _VertexesCount,
    const ApplicationRenderingBackendMeshVertexIndex* _Indexes,
    const ApplicationRenderingBackendMeshVertexIndex& _IndexesCount)
{
    std::shared_ptr<ApplicationRenderingBackendDirectX9> DirectX9 = graphics_api<ApplicationRenderingBackendDirectX9>();
    
    if(DirectX9 == nullptr || DirectX9->m_Device == nullptr || _Vertexes == nullptr || _Indexes == nullptr || _VertexesCount <= 0 || _IndexesCount <= 0)
        return false;

    // manage buffers
    {
        if(DirectX9->m_VertexBuffer == nullptr || DirectX9->m_IndexBuffer == nullptr || DirectX9->m_VertexBufferSize < _VertexesCount || DirectX9->m_IndexBufferSize < _IndexesCount)
        {
            if(DirectX9->m_VertexBuffer != NULL)
            {
                DirectX9->m_VertexBuffer->Release();
                DirectX9->m_VertexBuffer = NULL;
            }

            if(DirectX9->m_IndexBuffer != NULL)
            {
                DirectX9->m_IndexBuffer->Release();
                DirectX9->m_IndexBuffer = NULL;
            }

            if(FAILED(DirectX9->m_Device->CreateVertexBuffer(sizeof(CUSTOMVERTEX) * _VertexesCount, D3DUSAGE_DYNAMIC | D3DUSAGE_WRITEONLY, 0, D3DPOOL_DEFAULT, &DirectX9->m_VertexBuffer, NULL)))
                return false;
            else
                DirectX9->m_VertexBufferSize = _VertexesCount;

            if(FAILED(DirectX9->m_Device->CreateIndexBuffer(sizeof(CUSTOMINDEX) * _IndexesCount, D3DUSAGE_DYNAMIC | D3DUSAGE_WRITEONLY, sizeof(CUSTOMINDEX) == 2 ? D3DFMT_INDEX16 : D3DFMT_INDEX32, D3DPOOL_DEFAULT, &DirectX9->m_IndexBuffer, NULL)))
                return false;
            else
                DirectX9->m_IndexBufferSize  = _IndexesCount;
        }
    }

    // write buffers
    {
        // write
        CUSTOMVERTEX* pVertices;
        if(FAILED(DirectX9->m_VertexBuffer->Lock(0, sizeof(CUSTOMVERTEX) * _VertexesCount, (void**)&pVertices, D3DLOCK_DISCARD)))
            return false;

        // adjust colors of mesh points
        for (int i = 0; i < _VertexesCount; i++)
        {
            gs_color color = _Vertexes[i].Color;

            pVertices[i].Position[0] = _Vertexes[i].Position[0];
            pVertices[i].Position[1] = _Vertexes[i].Position[1];
            pVertices[i].Position[2] = _Vertexes[i].Position[2];

            pVertices[i].UV[0] = _Vertexes[i].UV[0];
            pVertices[i].UV[1] = _Vertexes[i].UV[1];

            pVertices[i].Color = D3DCOLOR_ARGB(gs_color_rgba_get_a(color), gs_color_rgba_get_r(color), gs_color_rgba_get_g(color), gs_color_rgba_get_b(color));
        }

        DirectX9->m_VertexBuffer->Unlock();

        VOID* pIndexes;
        if(FAILED(DirectX9->m_IndexBuffer->Lock(0, sizeof(CUSTOMINDEX) * _IndexesCount, (void**)&pIndexes, D3DLOCK_DISCARD)))
            return false;

        memcpy(pIndexes, _Indexes, sizeof(CUSTOMINDEX) * _IndexesCount);
        DirectX9->m_IndexBuffer->Unlock();
    }

    if(DirectX9->m_VertexBuffer == nullptr || DirectX9->m_IndexBuffer == nullptr)
        return false;

    if(SUCCEEDED(DirectX9->m_Device->BeginScene()))
    {
        DirectX9->m_Device->SetStreamSource(0, DirectX9->m_VertexBuffer, 0, sizeof(CUSTOMVERTEX));
        DirectX9->m_Device->SetIndices(DirectX9->m_IndexBuffer);
        return true;
    }

    return false;
}

void ApplicationRenderingBackend::render_mesh(
    const ApplicationRenderingBackendMeshVertexIndex& _SourceMeshVertex,
    const ApplicationRenderingBackendMeshVertexIndex& _TargetMeshVertex,
    const ApplicationRenderingBackendTexture&         _Texture,
    const gs_mat4f&                                   _MeshProjectionMatrix)
{
    std::shared_ptr<ApplicationRenderingBackendDirectX9> DirectX9 = graphics_api<ApplicationRenderingBackendDirectX9>();
    
    if(DirectX9 == nullptr || _SourceMeshVertex < 0 || _TargetMeshVertex < 0 || (_TargetMeshVertex - _SourceMeshVertex) <= 0)
        return;

    D3DMATRIX mat_world      = {{{1.0f, 0.0f, 0.0f, 0.0f,  0.0f, 1.0f, 0.0f, 0.0f,  0.0f, 0.0f, 1.0f, 0.0f,  0.0f, 0.0f, 0.0f, 1.0f}}};
    D3DMATRIX mat_camera     = {{{1.0f, 0.0f, 0.0f, 0.0f,  0.0f, 1.0f, 0.0f, 0.0f,  0.0f, 0.0f, 1.0f, 0.0f,  0.0f, 0.0f, 0.0f, 1.0f}}};
    D3DMATRIX mat_projection = gs_convert_transform_from_opengl_to_directx(_MeshProjectionMatrix);

    if(!_Texture.is_null())
    {
        // setup texture wrap mode
        switch (_Texture.Wrap)
        {
            case ApplicationRenderingBackendTextureWrapMode_::ApplicationRenderingBackendTextureWrapMode_Repeat:
                DirectX9->m_Device->SetSamplerState(0, D3DSAMP_ADDRESSU, D3DTADDRESS_WRAP);
                DirectX9->m_Device->SetSamplerState(0, D3DSAMP_ADDRESSV, D3DTADDRESS_WRAP);
                break;
            
            case ApplicationRenderingBackendTextureWrapMode_::ApplicationRenderingBackendTextureWrapMode_Mirrored:
                DirectX9->m_Device->SetSamplerState(0, D3DSAMP_ADDRESSU, D3DTADDRESS_MIRROR);
                DirectX9->m_Device->SetSamplerState(0, D3DSAMP_ADDRESSV, D3DTADDRESS_MIRROR);
                break;

            case ApplicationRenderingBackendTextureWrapMode_::ApplicationRenderingBackendTextureWrapMode_ClampToEdge:
                DirectX9->m_Device->SetSamplerState(0, D3DSAMP_ADDRESSU, D3DTADDRESS_CLAMP);
                DirectX9->m_Device->SetSamplerState(0, D3DSAMP_ADDRESSV, D3DTADDRESS_CLAMP);
                break;

            case ApplicationRenderingBackendTextureWrapMode_::ApplicationRenderingBackendTextureWrapMode_ClampToBorder:
                DirectX9->m_Device->SetSamplerState(0, D3DSAMP_ADDRESSU, D3DTADDRESS_BORDER);
                DirectX9->m_Device->SetSamplerState(0, D3DSAMP_ADDRESSV, D3DTADDRESS_BORDER);
                break;
            default:
                break;
        }

        // set minifying filter
        switch (_Texture.MinFilter)
        {
        case ApplicationRenderingBackendTextureMinFilter_::ApplicationRenderingBackendTextureMinFilter_Linear:
            DirectX9->m_Device->SetSamplerState(0, D3DSAMP_MINFILTER, D3DTEXF_LINEAR);
            break;
        
        case ApplicationRenderingBackendTextureMinFilter_::ApplicationRenderingBackendTextureMinFilter_Nearest:
            DirectX9->m_Device->SetSamplerState(0, D3DSAMP_MINFILTER, D3DTEXF_POINT);
            break;

        case ApplicationRenderingBackendTextureMinFilter_::ApplicationRenderingBackendTextureMinFilter_LinearMipMapLinear:
            DirectX9->m_Device->SetSamplerState(0, D3DSAMP_MIPFILTER, D3DTEXF_LINEAR);
            DirectX9->m_Device->SetSamplerState(0, D3DSAMP_MINFILTER, D3DTEXF_LINEAR);
            break;

        case ApplicationRenderingBackendTextureMinFilter_::ApplicationRenderingBackendTextureMinFilter_LinearMipMapNearest:
            DirectX9->m_Device->SetSamplerState(0, D3DSAMP_MIPFILTER, D3DTEXF_LINEAR);
            DirectX9->m_Device->SetSamplerState(0, D3DSAMP_MINFILTER, D3DTEXF_POINT);
            break;

        case ApplicationRenderingBackendTextureMinFilter_::ApplicationRenderingBackendTextureMinFilter_NearestMipMapLinear:
            DirectX9->m_Device->SetSamplerState(0, D3DSAMP_MIPFILTER, D3DTEXF_POINT);
            DirectX9->m_Device->SetSamplerState(0, D3DSAMP_MINFILTER, D3DTEXF_LINEAR);
            break;

        case ApplicationRenderingBackendTextureMinFilter_::ApplicationRenderingBackendTextureMinFilter_NearestMipMapNearest:
            DirectX9->m_Device->SetSamplerState(0, D3DSAMP_MIPFILTER, D3DTEXF_POINT);
            DirectX9->m_Device->SetSamplerState(0, D3DSAMP_MINFILTER, D3DTEXF_POINT);
            break;

        default:
            DirectX9->m_Device->SetSamplerState(0, D3DSAMP_MINFILTER, D3DTEXF_LINEAR);
            break;
        }

        // set magnifying filter
        switch (_Texture.MaxFilter)
        {
        case ApplicationRenderingBackendTextureMaxFilter_::ApplicationRenderingBackendTextureMaxFilter_Linear:
            DirectX9->m_Device->SetSamplerState(0, D3DSAMP_MAGFILTER, D3DTEXF_LINEAR);
            break;
        
        case ApplicationRenderingBackendTextureMaxFilter_::ApplicationRenderingBackendTextureMaxFilter_Nearest:
            DirectX9->m_Device->SetSamplerState(0, D3DSAMP_MAGFILTER, D3DTEXF_POINT);
            break;

        default:
            DirectX9->m_Device->SetSamplerState(0, D3DSAMP_MAGFILTER, D3DTEXF_LINEAR);
            break;
        }

        DirectX9->m_Device->SetTexture(0, reinterpret_cast<LPDIRECT3DTEXTURE9>(_Texture.Ptr));
    }

    DirectX9->m_Device->SetTransform(D3DTS_WORLD, &mat_world);
    DirectX9->m_Device->SetTransform(D3DTS_VIEW, &mat_camera);
    DirectX9->m_Device->SetTransform(D3DTS_PROJECTION, &mat_projection);

    DirectX9->m_Device->DrawIndexedPrimitive(
        D3DPT_TRIANGLELIST,
        0,
        _SourceMeshVertex,
        _TargetMeshVertex - _SourceMeshVertex,
        _SourceMeshVertex,
        (_TargetMeshVertex - _SourceMeshVertex) / 3);

    if(!_Texture.is_null())
        DirectX9->m_Device->SetTexture(0, NULL); // Unbind to prevent leaks
}

void ApplicationRenderingBackend::set_viewport(const gs_vec2f& _Position, const gs_vec2f& _Size)
{
    std::shared_ptr<ApplicationRenderingBackendDirectX9> DirectX9 = graphics_api<ApplicationRenderingBackendDirectX9>();

    if(DirectX9 != nullptr)
        DirectX9->m_Viewport = gs_2d_boxf(_Position, _Position + _Size);
}

void ApplicationRenderingBackend::clear_color(const gs_color& _Color)
{
    std::shared_ptr<ApplicationRenderingBackendDirectX9> DirectX9 = graphics_api<ApplicationRenderingBackendDirectX9>();

    if(DirectX9 != nullptr)
        DirectX9->m_ClearColor = _Color;
}

void ApplicationRenderingBackend::scissor_box(const gs_2d_boxf& _ClippingRect)
{
    std::shared_ptr<ApplicationRenderingBackendDirectX9> DirectX9 = graphics_api<ApplicationRenderingBackendDirectX9>();

    if(DirectX9 == nullptr)
        return;

    gs_vec2f  displayScale = ApplicationPlatformBackend::get_window_framebuffer_size() / ApplicationPlatformBackend::get_window_size();
    gs_2d_boxf clippingBox  = gs_2d_boxf(_ClippingRect.Min * displayScale, _ClippingRect.Max * displayScale);

    RECT scissorRect;
    SetRect(
        &scissorRect,
        (int)clippingBox.Min.x,
        (int)clippingBox.Min.y,
        (int)(clippingBox.Min.x + clippingBox.width()),
        (int)(clippingBox.Min.y + clippingBox.height()));

    DirectX9->m_Device->SetScissorRect(&scissorRect);
}

void ApplicationRenderingBackend::mesh_rendering_hints(const ApplicationRenderingBackendMeshRenderingHints& _Hints)
{
    std::shared_ptr<ApplicationRenderingBackendDirectX9> DirectX9 = graphics_api<ApplicationRenderingBackendDirectX9>();

    if(DirectX9 == nullptr)
        return;

    if(_Hints & ApplicationRenderingBackendMeshRenderingHints_::ApplicationRenderingBackendMeshRenderingHints_Lines)
        DirectX9->m_Device->SetRenderState(D3DRS_FILLMODE, D3DFILL_WIREFRAME);
    else if(_Hints & ApplicationRenderingBackendMeshRenderingHints_::ApplicationRenderingBackendMeshRenderingHints_Triangles)
        DirectX9->m_Device->SetRenderState(D3DRS_FILLMODE, D3DFILL_SOLID);
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
            gs_vec3f(0.f, 0.f, 1) + cameraLocalFrontAxisDirection, cameraLocalUpAxisDirection, false);
    
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