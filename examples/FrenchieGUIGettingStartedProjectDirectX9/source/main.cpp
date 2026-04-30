#include <FrenchieApplicationLayerRenderingQueue.hpp>

//-----------------------------------------------------------------------------
// File: Vertices.cpp
//
// Desc: In this tutorial, we are rendering some vertices. This introduces the
//       concept of the vertex buffer, a Direct3D object used to store
//       vertices. Vertices can be defined any way we want by defining a
//       custom structure and a custom FVF (flexible vertex format). In this
//       tutorial, we are using vertices that are transformed (meaning they
//       are already in 2D window coordinates) and lit (meaning we are not
//       using Direct3D lighting, but are supplying our own colors).
//
// Copyright (c) Microsoft Corporation. All rights reserved.
//-----------------------------------------------------------------------------
#include <d3d9.h>
#pragma warning( disable : 4996 ) // disable deprecated warning 
#include <strsafe.h>
#pragma warning( default : 4996 )

#include <tchar.h>

//-----------------------------------------------------------------------------
// Global variables
//-----------------------------------------------------------------------------
LPDIRECT3D9                  g_pD3D                  = NULL; // D3D interface
LPDIRECT3DDEVICE9            g_D3DDevice             = NULL; // rendering device
LPDIRECT3DVERTEXBUFFER9      g_D3DVertexBuffer       = NULL; // vertex buffer
LPDIRECT3DINDEXBUFFER9       g_D3DIndexBuffer        = NULL; // index buffer
IDirect3DVertexDeclaration9* g_D3DVertexDeclaration  = NULL; // vertex layout
D3DPRESENT_PARAMETERS        g_D3DPresentParameters;
HWND                         g_D3DContextWindow;

typedef Frenchie::Application::ApplicationRenderingBackendMeshVertex      CUSTOMVERTEX;
typedef Frenchie::Application::ApplicationRenderingBackendMeshVertexIndex CUSTOMINDEX;

std::vector<CUSTOMVERTEX> g_Vertices;
std::vector<CUSTOMINDEX>  g_Indexes;
std::vector<CUSTOMINDEX>  g_Offsets;
std::vector<CUSTOMINDEX>  g_Counts;

//-----------------------------------------------------------------------------
// Name: InitD3D()
// Desc: Initializes Direct3D
//-----------------------------------------------------------------------------
HRESULT InitD3D( HWND hWnd )
{
    // Create the D3D object.
    if(NULL == (g_pD3D = Direct3DCreate9(D3D_SDK_VERSION)))
        return E_FAIL;

    // Set up the structure used to create the D3DDevice
    ZeroMemory(&g_D3DPresentParameters, sizeof(g_D3DPresentParameters));
    g_D3DPresentParameters.Windowed         = TRUE;
    g_D3DPresentParameters.SwapEffect       = D3DSWAPEFFECT_DISCARD;
    g_D3DPresentParameters.BackBufferFormat = D3DFMT_A8R8G8B8;

    g_D3DPresentParameters.EnableAutoDepthStencil = TRUE;
    g_D3DPresentParameters.AutoDepthStencilFormat = D3DFMT_D24S8; 

    // Create the D3DDevice
    if( FAILED(g_pD3D->CreateDevice(
        D3DADAPTER_DEFAULT,
        D3DDEVTYPE_HAL,
        hWnd,
        D3DCREATE_SOFTWARE_VERTEXPROCESSING,
        &g_D3DPresentParameters,
        &g_D3DDevice)))
    {
        return E_FAIL;
    }

    // Device state would normally be set here

    return S_OK;
}




//-----------------------------------------------------------------------------
// Name: InitVB()
// Desc: Creates a vertex buffer and fills it with our vertices. The vertex
//       buffer is basically just a chuck of memory that holds vertices. After
//       creating it, we must Lock()/Unlock() it to fill it. For indices, D3D
//       also uses index buffers. The special thing about vertex and index
//       buffers is that they can be created in device memory, allowing some
//       cards to process them in hardware, resulting in a dramatic
//       performance gain.
//-----------------------------------------------------------------------------
HRESULT InitVB()
{
    D3DCOLOR color = D3DCOLOR_ARGB(255, 255, 0, 0);

    // Initialize three vertices for rendering a triangle
    g_Vertices =
    {
        // triangle 1
        { {150.0f,  50.0f, 0.0f}, {0.f, 0.f, 0.f}, {0.f, 0.f}, D3DCOLOR_ARGB(255, 255, 0, 0)},
        { {250.0f, 250.0f, 0.0f}, {0.f, 0.f, 0.f}, {0.f, 0.f}, D3DCOLOR_ARGB(255, 255, 0, 0)},
        { {50.0f, 250.0f, 0.0f}, {0.f, 0.f, 0.f}, {0.f, 0.f}, D3DCOLOR_ARGB(255, 255, 0, 0)},

        // triangle 2
        { {250.0f,  150.0f, 1.0f}, {0.f, 0.f, 0.f}, {0.f, 0.f}, D3DCOLOR_ARGB(255, 0, 255, 0)},
        { {250.0f, 350.0f, 1.0f}, {0.f, 0.f, 0.f}, {0.f, 0.f}, D3DCOLOR_ARGB(255, 0, 255, 0)},
        { {150.0f, 350.0f, 1.0f}, {0.f, 0.f, 0.f}, {0.f, 0.f}, D3DCOLOR_ARGB(255, 0, 255, 0)},
    };

    g_Indexes = 
    {
        // triangle 1
        0, 1, 2,
        
        // triangle 2
        3, 4, 5
    };

    g_Offsets =
    {
        0,
        3
    };

    g_Counts =
    {
        3,
        3
    };
    
    D3DVERTEXELEMENT9 VertexColElements[] =
    {
        {0, sizeof(float) * 0, D3DDECLTYPE_FLOAT3, D3DDECLMETHOD_DEFAULT, D3DDECLUSAGE_POSITION, 0},
        {0, sizeof(float) * 3, D3DDECLTYPE_FLOAT3, D3DDECLMETHOD_DEFAULT, D3DDECLUSAGE_NORMAL, 0},
        {0, sizeof(float) * 6, D3DDECLTYPE_FLOAT2, D3DDECLMETHOD_DEFAULT, D3DDECLUSAGE_TEXCOORD, 0},
        {0, sizeof(float) * 8, D3DDECLTYPE_D3DCOLOR, D3DDECLMETHOD_DEFAULT, D3DDECLUSAGE_COLOR, 0},
        D3DDECL_END(),
    };
    
    if(FAILED(g_D3DDevice->CreateVertexDeclaration(VertexColElements, &g_D3DVertexDeclaration)))
    {
        return E_FAIL;
    }

    // Create vertex buffer
    if(FAILED(g_D3DDevice->CreateVertexBuffer(
        sizeof(CUSTOMVERTEX) * g_Vertices.size(),
        0,
        0,
        D3DPOOL_MANAGED,
        &g_D3DVertexBuffer,
        NULL)))
    {
        return E_FAIL;
    }
    else
    {
        VOID* pVertices;
        if(FAILED(g_D3DVertexBuffer->Lock( 0, sizeof(CUSTOMVERTEX) * g_Vertices.size(), (void**)&pVertices, 0)))
            return E_FAIL;
        memcpy(pVertices, &g_Vertices[0], sizeof(CUSTOMVERTEX) * g_Vertices.size());
        g_D3DVertexBuffer->Unlock();
    }

    // Create index buffer
    if(FAILED(g_D3DDevice->CreateIndexBuffer(
        sizeof(CUSTOMVERTEX) * g_Indexes.size(),
        0,
        sizeof(CUSTOMVERTEX) == 2 ? D3DFMT_INDEX16 : D3DFMT_INDEX32,
        D3DPOOL_MANAGED,
        &g_D3DIndexBuffer,
        nullptr)))
    {
        return E_FAIL;
    }
    else
    {
        VOID* pIndexes;
        if(FAILED(g_D3DIndexBuffer->Lock( 0, sizeof(CUSTOMVERTEX) * g_Indexes.size(), (void**)&pIndexes, 0)))
            return E_FAIL;
        memcpy(pIndexes, &g_Indexes[0], sizeof(CUSTOMVERTEX) * g_Indexes.size());
        g_D3DIndexBuffer->Unlock();
    }

    return S_OK;
}




//-----------------------------------------------------------------------------
// Name: Cleanup()
// Desc: Releases all previously initialized objects
//-----------------------------------------------------------------------------
VOID Cleanup()
{
    if( g_D3DVertexBuffer != NULL )
        g_D3DVertexBuffer->Release();

    if( g_D3DDevice != NULL )
        g_D3DDevice->Release();

    if( g_pD3D != NULL )
        g_pD3D->Release();
}




//-----------------------------------------------------------------------------
// Name: Render()
// Desc: Draws the scene
//-----------------------------------------------------------------------------
VOID Render()
{
    // retrieve window client rect
    RECT clientRect;

    if (GetClientRect(g_D3DContextWindow, &clientRect))
    {
        // adjust backbuffer
        g_D3DPresentParameters.BackBufferWidth  = clientRect.right - clientRect.left;
        g_D3DPresentParameters.BackBufferHeight = clientRect.bottom - clientRect.top;
        g_D3DDevice->Reset(&g_D3DPresentParameters);

        // adjust viewport
        D3DVIEWPORT9 vp;
        vp.X      = 0;
        vp.Y      = 0;
        vp.Width  = clientRect.right - clientRect.left;
        vp.Height = clientRect.bottom - clientRect.top;
        vp.MinZ   = 0.0f;
        vp.MaxZ   = 1.0f;
        g_D3DDevice->SetViewport(&vp);

        // adjust projectrion matrix
        float L = 0 + 0.5f;
        float R = 0 + (clientRect.right - clientRect.left) + 0.5f;
        float T = 0 + 0.5f;
        float B = 0 + (clientRect.bottom - clientRect.top) + 0.5f;
        float W = clientRect.right - clientRect.left;
        float H = clientRect.bottom - clientRect.top;

        D3DMATRIX mat_identity   = {{{ 1.0f, 0.0f, 0.0f, 0.0f,  0.0f, 1.0f, 0.0f, 0.0f,  0.0f, 0.0f, 1.0f, 0.0f,  0.0f, 0.0f, 0.0f, 1.0f}}};

        D3DMATRIX mat_projection =
        {{{
            2.0f/(R-L),   0.0f,         0.0f,  0.0f,
            0.0f,         2.0f/(T-B),   0.0f,  0.0f,
            0.0f,         0.0f,         0.5f,  0.0f,
            (L+R)/(L-R),  (T+B)/(B-T),  0.5f,  1.0f
        }}};

        g_D3DDevice->SetTransform(D3DTS_WORLD, &mat_identity);
        g_D3DDevice->SetTransform(D3DTS_VIEW, &mat_identity);
        g_D3DDevice->SetTransform(D3DTS_PROJECTION, &mat_projection);
    }

    // Clear the backbuffer to a blue color
    g_D3DDevice->SetPixelShader(nullptr);
    g_D3DDevice->SetVertexShader(nullptr);
    g_D3DDevice->SetRenderState(D3DRS_FILLMODE, D3DFILL_SOLID);
    g_D3DDevice->SetRenderState(D3DRS_SHADEMODE, D3DSHADE_GOURAUD);
    g_D3DDevice->SetRenderState(D3DRS_ZWRITEENABLE, TRUE);
    // g_pd3dDevice->SetRenderState(D3DRS_ALPHATESTENABLE, FALSE);
    // g_pd3dDevice->SetRenderState(D3DRS_CULLMODE, D3DCULL_NONE);
    g_D3DDevice->SetRenderState(D3DRS_ZENABLE, TRUE);

    g_D3DDevice->SetRenderState(D3DRS_ALPHABLENDENABLE, TRUE);
    g_D3DDevice->SetRenderState(D3DRS_SRCBLEND, D3DBLEND_SRCALPHA);
    g_D3DDevice->SetRenderState(D3DRS_DESTBLEND, D3DBLEND_INVSRCALPHA);

    // g_pd3dDevice->SetRenderState(D3DRS_BLENDOP, D3DBLENDOP_ADD);
    // g_pd3dDevice->SetRenderState(D3DRS_SRCBLEND, D3DBLEND_SRCALPHA);
    // g_pd3dDevice->SetRenderState(D3DRS_DESTBLEND, D3DBLEND_INVSRCALPHA);
    // g_pd3dDevice->SetRenderState(D3DRS_SEPARATEALPHABLENDENABLE, TRUE);
    // g_pd3dDevice->SetRenderState(D3DRS_SRCBLENDALPHA, D3DBLEND_ONE);
    // g_pd3dDevice->SetRenderState(D3DRS_DESTBLENDALPHA, D3DBLEND_INVSRCALPHA);
    
    // g_pd3dDevice->SetRenderState(D3DRS_SCISSORTESTENABLE, TRUE);
    // g_pd3dDevice->SetRenderState(D3DRS_FOGENABLE, FALSE);
    // g_pd3dDevice->SetRenderState(D3DRS_RANGEFOGENABLE, FALSE);
    // g_pd3dDevice->SetRenderState(D3DRS_SPECULARENABLE, FALSE);
    // g_pd3dDevice->SetRenderState(D3DRS_STENCILENABLE, FALSE);
    // g_pd3dDevice->SetRenderState(D3DRS_CLIPPING, TRUE);
    g_D3DDevice->SetRenderState(D3DRS_LIGHTING, FALSE);
    // g_pd3dDevice->SetTextureStageState(0, D3DTSS_COLOROP, D3DTOP_MODULATE);
    // g_pd3dDevice->SetTextureStageState(0, D3DTSS_COLORARG1, D3DTA_TEXTURE);
    // g_pd3dDevice->SetTextureStageState(0, D3DTSS_COLORARG2, D3DTA_DIFFUSE);
    // g_pd3dDevice->SetTextureStageState(0, D3DTSS_ALPHAOP, D3DTOP_MODULATE);
    // g_pd3dDevice->SetTextureStageState(0, D3DTSS_ALPHAARG1, D3DTA_TEXTURE);
    // g_pd3dDevice->SetTextureStageState(0, D3DTSS_ALPHAARG2, D3DTA_DIFFUSE);
    // g_pd3dDevice->SetTextureStageState(1, D3DTSS_COLOROP, D3DTOP_DISABLE);
    // g_pd3dDevice->SetTextureStageState(1, D3DTSS_ALPHAOP, D3DTOP_DISABLE);
    // g_pd3dDevice->SetSamplerState(0, D3DSAMP_MINFILTER, D3DTEXF_LINEAR);
    // g_pd3dDevice->SetSamplerState(0, D3DSAMP_MAGFILTER, D3DTEXF_LINEAR);
    // g_pd3dDevice->SetSamplerState(0, D3DSAMP_ADDRESSU, D3DTADDRESS_CLAMP);
    // g_pd3dDevice->SetSamplerState(0, D3DSAMP_ADDRESSV, D3DTADDRESS_CLAMP);

    g_D3DDevice->Clear(0, NULL, D3DCLEAR_TARGET | D3DCLEAR_ZBUFFER, D3DCOLOR_XRGB(128, 128, 128), 1.0f, 0);

    // Render scene into a back buffer
    if(SUCCEEDED(g_D3DDevice->BeginScene()))
    {
        g_D3DDevice->SetStreamSource(0, g_D3DVertexBuffer, 0, sizeof( CUSTOMVERTEX ) );
        g_D3DDevice->SetIndices(g_D3DIndexBuffer);
        g_D3DDevice->SetVertexDeclaration(g_D3DVertexDeclaration);

        for (int i = 0; i < (int)g_Offsets.size(); i++)        
        {
            g_D3DDevice->DrawIndexedPrimitive(
                D3DPT_TRIANGLELIST,
                0,
                0,
                (UINT)g_Vertices.size(),
                g_Offsets[i],
                (UINT)g_Counts[i]);
        }

        g_D3DDevice->EndScene();
    }

    // Present the backbuffer contents to the display
    g_D3DDevice->Present(NULL, NULL, NULL, NULL);
}

//-----------------------------------------------------------------------------
// Name: MsgProc()
// Desc: The window's message handler
//-----------------------------------------------------------------------------
LRESULT WINAPI MsgProc(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam)
{
    switch(msg)
    {
        case WM_DESTROY:
            Cleanup();
            PostQuitMessage(0);
            return 0;
    }

    return DefWindowProc(hWnd, msg, wParam, lParam);
}


//-----------------------------------------------------------------------------
// Name: wWinMain()
// Desc: The application's entry point
//-----------------------------------------------------------------------------
INT main(int argc, char *argv[])
{
    // Register the window class
    WNDCLASSEX wc =
    {
        sizeof(WNDCLASSEX), CS_CLASSDC, MsgProc, 0L, 0L,
        GetModuleHandle(NULL), NULL, NULL, NULL, NULL,
        TEXT("D3D Tutorial"), NULL
    };
    RegisterClassEx( &wc );

    // Create the application's window
    g_D3DContextWindow = CreateWindow(
        TEXT("D3D Tutorial"),
        TEXT("D3D Tutorial 02: Vertices"),
        WS_OVERLAPPEDWINDOW,
        100,
        100,
        300,
        300,
        NULL,
        NULL,
        wc.hInstance,
        NULL);

    // Initialize Direct3D
    if(SUCCEEDED(InitD3D(g_D3DContextWindow)))
    {
        // Create the vertex buffer
        if(SUCCEEDED(InitVB()))
        {
            // Show the window
            ShowWindow(g_D3DContextWindow, SW_SHOWDEFAULT);
            UpdateWindow(g_D3DContextWindow);

            // Enter the message loop
            MSG msg;
            ZeroMemory(&msg, sizeof(msg));
            while( msg.message != WM_QUIT )
            {
                if(PeekMessage(&msg, NULL, 0U, 0U, PM_REMOVE))
                {
                    TranslateMessage(&msg);
                    DispatchMessage(&msg);
                }
                else
                    Render();
            }
        }
    }

    UnregisterClass(TEXT("D3D Tutorial"), wc.hInstance);
    return 0;
}