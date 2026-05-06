// Application
#include <FrenchieApplicationPlatformBackend.hpp>
#include <FrenchieApplicationRenderingBackend.hpp>

// D3D9
#include <d3d9.h>
#pragma warning( disable : 4996 ) // disable deprecated warning 
#include <strsafe.h>
#pragma warning( default : 4996 )

#include <iostream>

using namespace Frenchie::Application;

namespace Frenchie
{
    namespace Application
    {
        struct ApplicationRenderingBackendDirectX9 : public ApplicationRenderingBackendGraphicsApi
        {
            ApplicationRenderingBackendDirectX9(){}
            virtual ~ApplicationRenderingBackendDirectX9(){}

            LPDIRECT3D9                  D3D                = NULL;                      // D3D interface
            LPDIRECT3DDEVICE9            Device             = NULL;                      // rendering device
            LPDIRECT3DVERTEXBUFFER9      VertexBuffer       = NULL;                      // vertex buffer
            UINT                         VertexBufferSize   = 0;                         // vertex buffer size
            LPDIRECT3DINDEXBUFFER9       IndexBuffer        = NULL;                      // index buffer
            UINT                         IndexBufferSize    = 0;                         // index buffer size
            IDirect3DVertexDeclaration9* VertexDeclaration  = NULL;                      // vertex layout
            IDirect3DStateBlock9*        RendererState      = NULL;                      // D3D device renderer state
            gs_color                     ClearColor         = gs_color_rgba(0, 0, 0, 0); // D3D device renderer clear color
            bool                         DeviceLost         = false;                     // D3D device lost event boolean handler
            D3DPRESENT_PARAMETERS        PresentParameters;                              // D3D device scene present parameters

            void reset()
            {
                if(VertexBuffer != NULL)
                    VertexBuffer->Release();
                VertexBuffer = NULL;

                if(IndexBuffer != NULL)
                    IndexBuffer->Release();
                IndexBuffer = NULL;
            }
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
    
    m_Api = std::make_shared<ApplicationRenderingBackendDirectX9>();
    
    std::shared_ptr<ApplicationRenderingBackendDirectX9> g_DirectX9 = graphics_api<ApplicationRenderingBackendDirectX9>();

    // Create the D3D object.
    if(NULL == (g_DirectX9->D3D = Direct3DCreate9(D3D_SDK_VERSION)))
        return false;

    // Set up the structure used to create the D3DDevice
    ZeroMemory(&g_DirectX9->PresentParameters, sizeof(g_DirectX9->PresentParameters));
    g_DirectX9->PresentParameters.Windowed               = TRUE;
    g_DirectX9->PresentParameters.SwapEffect             = D3DSWAPEFFECT_DISCARD;
    g_DirectX9->PresentParameters.BackBufferFormat       = D3DFMT_A8R8G8B8;
    g_DirectX9->PresentParameters.EnableAutoDepthStencil = TRUE;
    g_DirectX9->PresentParameters.AutoDepthStencilFormat = D3DFMT_D24S8; 

    // Create the D3DDevice
    if(FAILED(g_DirectX9->D3D->CreateDevice(D3DADAPTER_DEFAULT, D3DDEVTYPE_HAL, hWnd, D3DCREATE_SOFTWARE_VERTEXPROCESSING, &g_DirectX9->PresentParameters, &g_DirectX9->Device)))
        return false;

    // Declare mesh vertex
    D3DVERTEXELEMENT9 VertexColElements[] =
    {
        {0, sizeof(float) * 0, D3DDECLTYPE_FLOAT3, D3DDECLMETHOD_DEFAULT, D3DDECLUSAGE_POSITION, 0},
        {0, sizeof(float) * 3, D3DDECLTYPE_FLOAT3, D3DDECLMETHOD_DEFAULT, D3DDECLUSAGE_NORMAL,   0},
        {0, sizeof(float) * 6, D3DDECLTYPE_FLOAT2, D3DDECLMETHOD_DEFAULT, D3DDECLUSAGE_TEXCOORD, 0},
        {0, sizeof(float) * 8, D3DDECLTYPE_D3DCOLOR, D3DDECLMETHOD_DEFAULT, D3DDECLUSAGE_COLOR,  0},
        D3DDECL_END(),
    };
    
    if(FAILED(g_DirectX9->Device->CreateVertexDeclaration(VertexColElements, &g_DirectX9->VertexDeclaration)))
        return false;

    g_DirectX9->Device->SetVertexDeclaration(g_DirectX9->VertexDeclaration);
    g_DirectX9->Device->SetPixelShader(nullptr);
    g_DirectX9->Device->SetVertexShader(nullptr);

    return true;
}

void ApplicationRenderingBackend::quit()
{
    std::shared_ptr<ApplicationRenderingBackendDirectX9> g_DirectX9 = graphics_api<ApplicationRenderingBackendDirectX9>();

    if(g_DirectX9 == nullptr)
        return;

    if(g_DirectX9->VertexBuffer != NULL)
        g_DirectX9->VertexBuffer->Release();

    if(g_DirectX9->IndexBuffer != NULL)
        g_DirectX9->IndexBuffer->Release();

    if(g_DirectX9->Device != NULL)
        g_DirectX9->Device->Release();

    if(g_DirectX9->RendererState != NULL)
        g_DirectX9->RendererState->Release();

    if(g_DirectX9->D3D != NULL)
        g_DirectX9->D3D->Release();
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

    std::shared_ptr<ApplicationRenderingBackendDirectX9> g_DirectX9 = graphics_api<ApplicationRenderingBackendDirectX9>();

    if(g_DirectX9 == nullptr || g_DirectX9->Device == nullptr)
        return ApplicationRenderingBackendTexture();

    std::shared_ptr<unsigned char> image = std::shared_ptr<unsigned char>(new unsigned char[_Width * _Height * 4]);

    const int height   = _Height;
    const int width    = _Width;
    const int channels = 4;
    const int red      = 0;
    const int green    = 1;
    const int blue     = 2;
    const int alpha    = 3;

    for (int y = 0; y < height; y++)
    {
        for (int x = 0; x < width; x++)
        {
            unsigned char r = _RawBuffer[channels * (y * width + x) + red  ];
            unsigned char g = _RawBuffer[channels * (y * width + x) + green];
            unsigned char b = _RawBuffer[channels * (y * width + x) + blue ];
            unsigned char a = _RawBuffer[channels * (y * width + x) + alpha];

            image.get()[channels * (y * width + x) + red  ] = b;
            image.get()[channels * (y * width + x) + green] = g;
            image.get()[channels * (y * width + x) + blue ] = r;
            image.get()[channels * (y * width + x) + alpha] = a;
        }
    }

    // create texture
    LPDIRECT3DTEXTURE9 pTexture = nullptr;
    g_DirectX9->Device->CreateTexture(_Width, _Height, 1, 0, D3DFMT_A8R8G8B8, D3DPOOL_MANAGED, &pTexture, nullptr);

    // lock texture rect
    D3DLOCKED_RECT lockedRect;
    pTexture->LockRect(0, &lockedRect, nullptr, D3DLOCK_DISCARD);

    // 3. Copy Data (Row by Row to handle pitch)
    unsigned char* pDest = (unsigned char*)lockedRect.pBits;
    const unsigned char* pSrc = image.get();

    int stride = _Width * 4; // 4 bytes per pixel

    for (int row = 0; row < _Height; ++row)
    {
        memcpy(pDest, pSrc, stride);
        pDest += lockedRect.Pitch;
        pSrc += stride;
    }

    // 4. Unlock
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

bool ApplicationRenderingBackend::begin_render(
    const ApplicationRenderingBackendMeshVertex*      _Vertexes,
    const ApplicationRenderingBackendMeshVertexIndex& _VertexesCount,
    const ApplicationRenderingBackendMeshVertexIndex* _Indexes,
    const ApplicationRenderingBackendMeshVertexIndex& _IndexesCount)
{
    std::shared_ptr<ApplicationRenderingBackendDirectX9> g_DirectX9 = graphics_api<ApplicationRenderingBackendDirectX9>();
    
    if(g_DirectX9 == nullptr || g_DirectX9->Device == nullptr || _Vertexes == nullptr || _Indexes == nullptr || _VertexesCount <= 0 || _IndexesCount <= 0)
        return false;

    // handle lost D3D9 device
    if (g_DirectX9->DeviceLost)
    {
        HRESULT hr = g_DirectX9->Device->TestCooperativeLevel();

        if (hr == D3DERR_DEVICELOST)
        {
            ::Sleep(10);
            return false;
        }

        if (hr == D3DERR_DEVICENOTRESET)
            g_DirectX9->reset();
        g_DirectX9->Device = false;
    }

    // backup the DX9 state
    if(g_DirectX9->RendererState != NULL)
    {
        g_DirectX9->RendererState->Release();
        g_DirectX9->RendererState = NULL;
    }

    if (g_DirectX9->Device->CreateStateBlock(D3DSBT_ALL, &g_DirectX9->RendererState) < 0)
        return false;

    if (FAILED(g_DirectX9->RendererState->Capture()))
        return false;

    // manage buffers
    {
        if(g_DirectX9->VertexBuffer == nullptr || g_DirectX9->IndexBuffer == nullptr || g_DirectX9->VertexBufferSize < _VertexesCount || g_DirectX9->IndexBufferSize < _IndexesCount)
        {
            if(g_DirectX9->VertexBuffer != NULL)
            {
                g_DirectX9->VertexBuffer->Release();
                g_DirectX9->VertexBuffer = NULL;
            }

            if(g_DirectX9->IndexBuffer != NULL)
            {
                g_DirectX9->IndexBuffer->Release();
                g_DirectX9->IndexBuffer = NULL;
            }

            if(FAILED(g_DirectX9->Device->CreateVertexBuffer(sizeof(CUSTOMVERTEX) * _VertexesCount, D3DUSAGE_DYNAMIC | D3DUSAGE_WRITEONLY, 0, D3DPOOL_DEFAULT, &g_DirectX9->VertexBuffer, NULL)))
                return false;
            else
                g_DirectX9->VertexBufferSize = _VertexesCount;

            if(FAILED(g_DirectX9->Device->CreateIndexBuffer(sizeof(CUSTOMINDEX) * _IndexesCount, D3DUSAGE_DYNAMIC | D3DUSAGE_WRITEONLY, sizeof(CUSTOMINDEX) == 2 ? D3DFMT_INDEX16 : D3DFMT_INDEX32, D3DPOOL_DEFAULT, &g_DirectX9->IndexBuffer, NULL)))
                return false;
            else
                g_DirectX9->IndexBufferSize  = _IndexesCount;
        }
    }

    // write buffers
    {
        // write
        CUSTOMVERTEX* pVertices;
        if(FAILED(g_DirectX9->VertexBuffer->Lock(0, sizeof(CUSTOMVERTEX) * _VertexesCount, (void**)&pVertices, D3DLOCK_DISCARD)))
            return false;

        // adjust colors of mesh points
        for (int i = 0; i < _VertexesCount; i++)
        {
            gs_color color = _Vertexes[i].Color;

            pVertices[i].Normal[0] = _Vertexes[i].Normal[0];
            pVertices[i].Normal[1] = _Vertexes[i].Normal[1];
            pVertices[i].Normal[2] = _Vertexes[i].Normal[2];

            pVertices[i].Position[0] = _Vertexes[i].Position[0];
            pVertices[i].Position[1] = _Vertexes[i].Position[1];
            pVertices[i].Position[2] = _Vertexes[i].Position[2];

            pVertices[i].UV[0] = _Vertexes[i].UV[0];
            pVertices[i].UV[1] = _Vertexes[i].UV[1];

            pVertices[i].Color = D3DCOLOR_ARGB(gs_color_rgba_get_a(color), gs_color_rgba_get_r(color), gs_color_rgba_get_g(color), gs_color_rgba_get_b(color));
        }

        g_DirectX9->VertexBuffer->Unlock();

        VOID* pIndexes;
        if(FAILED(g_DirectX9->IndexBuffer->Lock(0, sizeof(CUSTOMINDEX) * _IndexesCount, (void**)&pIndexes, D3DLOCK_DISCARD)))
            return false;
        memcpy(pIndexes, _Indexes, sizeof(CUSTOMINDEX) * _IndexesCount);
        g_DirectX9->IndexBuffer->Unlock();
    }

    if(g_DirectX9->VertexBuffer == nullptr || g_DirectX9->IndexBuffer == nullptr)
        return false;

    g_DirectX9->Device->SetRenderState(D3DRS_FILLMODE, D3DFILL_SOLID);
    g_DirectX9->Device->SetRenderState(D3DRS_SHADEMODE, D3DSHADE_GOURAUD);

    g_DirectX9->Device->SetRenderState(D3DRS_ZENABLE, D3DZB_TRUE);
    g_DirectX9->Device->SetRenderState(D3DRS_ZWRITEENABLE, TRUE);
    g_DirectX9->Device->SetRenderState(D3DRS_ZFUNC, D3DCMP_LESS);

    g_DirectX9->Device->SetRenderState(D3DRS_ALPHATESTENABLE, TRUE);

    g_DirectX9->Device->SetRenderState(D3DRS_CULLMODE, D3DCULL_NONE); // I DON'T KNOW HOW TO ENABLE CULLING HERE CORRECTLY ...
    
    // alpha blending
    g_DirectX9->Device->SetRenderState(D3DRS_ALPHABLENDENABLE, TRUE);
    g_DirectX9->Device->SetRenderState(D3DRS_SRCBLEND, D3DBLEND_SRCALPHA);
    g_DirectX9->Device->SetRenderState(D3DRS_DESTBLEND, D3DBLEND_INVSRCALPHA);
    g_DirectX9->Device->SetRenderState(D3DRS_BLENDOP, D3DBLENDOP_ADD);
    
    g_DirectX9->Device->SetRenderState(D3DRS_SCISSORTESTENABLE, TRUE);
    // g_DirectX9->g_D3DDevice->SetRenderState(D3DRS_FOGENABLE, FALSE);
    // g_DirectX9->g_D3DDevice->SetRenderState(D3DRS_RANGEFOGENABLE, FALSE);
    // g_DirectX9->g_D3DDevice->SetRenderState(D3DRS_SPECULARENABLE, FALSE);
    // g_DirectX9->g_D3DDevice->SetRenderState(D3DRS_STENCILENABLE, TRUE);
    // g_DirectX9->g_D3DDevice->SetRenderState(D3DRS_CLIPPING, TRUE);
    g_DirectX9->Device->SetRenderState(D3DRS_LIGHTING, FALSE);

    g_DirectX9->Device->SetTextureStageState(0, D3DTSS_COLOROP, D3DTOP_MODULATE);
    g_DirectX9->Device->SetTextureStageState(0, D3DTSS_COLORARG1, D3DTA_TEXTURE);
    g_DirectX9->Device->SetTextureStageState(0, D3DTSS_COLORARG2, D3DTA_DIFFUSE);
    
    g_DirectX9->Device->SetTextureStageState(0, D3DTSS_ALPHAOP, D3DTOP_MODULATE);
    g_DirectX9->Device->SetTextureStageState(0, D3DTSS_ALPHAARG1, D3DTA_TEXTURE);
    g_DirectX9->Device->SetTextureStageState(0, D3DTSS_ALPHAARG2, D3DTA_DIFFUSE);

    // g_DirectX9->g_D3DDevice->SetTextureStageState(1, D3DTSS_COLOROP, D3DTOP_DISABLE);
    // g_DirectX9->g_D3DDevice->SetTextureStageState(1, D3DTSS_ALPHAOP, D3DTOP_DISABLE);

    // g_DirectX9->g_D3DDevice->SetSamplerState(0, D3DSAMP_MINFILTER, D3DTEXF_LINEAR);
    // g_DirectX9->g_D3DDevice->SetSamplerState(0, D3DSAMP_MAGFILTER, D3DTEXF_LINEAR);
    // g_DirectX9->g_D3DDevice->SetSamplerState(0, D3DSAMP_ADDRESSU, D3DTADDRESS_CLAMP);
    // g_DirectX9->g_D3DDevice->SetSamplerState(0, D3DSAMP_ADDRESSV, D3DTADDRESS_CLAMP);

    // clear back buffer
    g_DirectX9->Device->Clear(
        0,
        NULL,
        D3DCLEAR_TARGET | D3DCLEAR_ZBUFFER | D3DCLEAR_STENCIL,
        D3DCOLOR_RGBA(
            gs_color_rgba_get_r(g_DirectX9->ClearColor),
            gs_color_rgba_get_g(g_DirectX9->ClearColor),
            gs_color_rgba_get_b(g_DirectX9->ClearColor),
            gs_color_rgba_get_a(g_DirectX9->ClearColor)),
        1.0f,
        0);

    if(SUCCEEDED(g_DirectX9->Device->BeginScene()))
    {
        g_DirectX9->Device->SetStreamSource(0, g_DirectX9->VertexBuffer, 0, sizeof(CUSTOMVERTEX));
        g_DirectX9->Device->SetIndices(g_DirectX9->IndexBuffer);
        return true;
    }

    return false;
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
    std::shared_ptr<ApplicationRenderingBackendDirectX9> g_DirectX9 = graphics_api<ApplicationRenderingBackendDirectX9>();
    
    if(g_DirectX9 == nullptr || g_DirectX9->Device == nullptr || _Vertexes == nullptr || _Indexes == nullptr || _VertexesCount <= 0 || _IndexesCount <= 0 || _MeshVertexesCount <= 0 || _MeshIndexesCount <= 0)
        return;

    D3DMATRIX mat_world      = {{{1.0f, 0.0f, 0.0f, 0.0f,  0.0f, 1.0f, 0.0f, 0.0f,  0.0f, 0.0f, 1.0f, 0.0f,  0.0f, 0.0f, 0.0f, 1.0f}}};
    D3DMATRIX mat_camera     = {{{1.0f, 0.0f, 0.0f, 0.0f,  0.0f, 1.0f, 0.0f, 0.0f,  0.0f, 0.0f, 1.0f, 0.0f,  0.0f, 0.0f, 0.0f, 1.0f}}};
    D3DMATRIX mat_projection = gs_convert_transform_from_opengl_to_directx(_MeshProjectionMatrix);

    if(!_Texture.is_null())
        g_DirectX9->Device->SetTexture(0, reinterpret_cast<LPDIRECT3DTEXTURE9>(_Texture.Ptr));

    g_DirectX9->Device->SetTransform(D3DTS_WORLD, &mat_world);
    g_DirectX9->Device->SetTransform(D3DTS_VIEW, &mat_camera);
    g_DirectX9->Device->SetTransform(D3DTS_PROJECTION, &mat_projection);

    g_DirectX9->Device->DrawIndexedPrimitive(
        D3DPT_TRIANGLELIST,
        0,
        _MeshIndexesOffset,
        _MeshIndexesCount - _MeshIndexesOffset,
        _MeshIndexesOffset,
        (_MeshIndexesCount - _MeshIndexesOffset) / 3);

    if(!_Texture.is_null())
        g_DirectX9->Device->SetTexture(0, NULL);    // Unbind to prevent leaks
}

void ApplicationRenderingBackend::end_render()
{
    std::shared_ptr<ApplicationRenderingBackendDirectX9> g_DirectX9 = graphics_api<ApplicationRenderingBackendDirectX9>();

    if(g_DirectX9 == nullptr)
        return;

    g_DirectX9->Device->EndScene();

    // Present the backbuffer contents to the display
    HRESULT result = g_DirectX9->Device->Present(NULL, NULL, NULL, NULL);

    if (result == D3DERR_DEVICELOST)
        g_DirectX9->DeviceLost = true;

    // Restore the DX9 state
    g_DirectX9->RendererState->Apply();
    g_DirectX9->RendererState->Release();
    g_DirectX9->RendererState = NULL;
}

void ApplicationRenderingBackend::set_viewport(const gs_vec2f& _Position, const gs_vec2f& _Size)
{
    // std::shared_ptr<ApplicationRenderingBackendDirectX9> g_DirectX9 = graphics_api<ApplicationRenderingBackendDirectX9>();

    // if(g_DirectX9 == nullptr)
    //     return;

    // // adjust backbuffer
    // g_DirectX9->g_D3DPresentParameters.BackBufferWidth  = _Size.x;
    // g_DirectX9->g_D3DPresentParameters.BackBufferHeight = _Size.y;
    // g_DirectX9->g_D3DDevice->Reset(&g_DirectX9->g_D3DPresentParameters);

    // // adjust viewport
    // D3DVIEWPORT9 vp;
    // vp.X      = _Position.x;
    // vp.Y      = _Position.y;
    // vp.Width  = _Size.x;
    // vp.Height = _Size.y;
    // vp.MinZ   = 0.0f;
    // vp.MaxZ   = 1.0f;
    // g_DirectX9->g_D3DDevice->SetViewport(&vp);

    // // reset rendering device
    // g_DirectX9->reset();
}

void ApplicationRenderingBackend::clear_color(const gs_color& _Color)
{
    std::shared_ptr<ApplicationRenderingBackendDirectX9> g_DirectX9 = graphics_api<ApplicationRenderingBackendDirectX9>();

    if(g_DirectX9 == nullptr)
        return;

    g_DirectX9->ClearColor = _Color;
}

void ApplicationRenderingBackend::scissor_box(const gs_2dboxf& _ClippingRect)
{
    std::shared_ptr<ApplicationRenderingBackendDirectX9> g_DirectX9 = graphics_api<ApplicationRenderingBackendDirectX9>();

    if(g_DirectX9 == nullptr)
        return;

    RECT scissorRect;
    SetRect(
        &scissorRect,
        (int)_ClippingRect.Min.x,
        (int)_ClippingRect.Min.y,
        (int)(_ClippingRect.Min.x + _ClippingRect.width()),
        (int)(_ClippingRect.Min.y + _ClippingRect.height())); // 300x300 area

    g_DirectX9->Device->SetScissorRect(&scissorRect);
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