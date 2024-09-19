
// TODO Ç‹ÇæçÌÇÍÇÈÇÕÇ∏
#pragma comment(lib, "d3d9.lib")
#ifdef _DEBUG
#pragma comment(lib, "d3dx9d.lib")
#else
#pragma comment(lib, "d3dx9.lib")
#endif

#include "Mesh.h"
#include "AnimMesh.h"

#include <windows.h>
#include <string>
#include <d3dx9.h>

const std::string TITLE = "simple anim mesh";
HWND m_hWnd;
LPDIRECT3D9 m_D3D;
Mesh* m_Mesh1 { nullptr };
AnimMesh* m_AnimMesh2 = { nullptr };

const D3DXVECTOR3 UPWARD { 0.0f, 1.0f, 0.0f };
D3DXVECTOR3 m_eyePos { 0.0f, 3.0f, -2.0f };
D3DXVECTOR3 m_lookAtPos { 0.0f, 0.0f, 0.0f };
float m_viewAngle { D3DX_PI / 4 };
float m_radian { D3DX_PI*3 / 4 };

D3DXMATRIX GetViewMatrix()
{
    D3DXMATRIX viewMatrix { };
    D3DXMatrixLookAtLH(&viewMatrix,
        &m_eyePos,
        &m_lookAtPos,
        &UPWARD);
    return viewMatrix;
}

D3DXMATRIX GetProjMatrix()
{
    D3DXMATRIX projection_matrix { };
    D3DXMatrixPerspectiveFovLH(
        &projection_matrix,
        m_viewAngle,
        static_cast<float>(1920) / 1080, /* TODO */
        0.1f,
        500.0f);
    return projection_matrix;
}

void Update()
{
    m_radian += 1/100.f;
    m_eyePos.x = m_lookAtPos.x + std::sin(m_radian)*10;
    m_eyePos.z = m_lookAtPos.z + std::cos(m_radian)*10;
}

LRESULT CALLBACK WndProc(HWND hWnd, UINT mes, WPARAM wParam, LPARAM lParam)
{
    if (mes == WM_DESTROY)
    {
        PostQuitMessage(0);
        return 0;
    }

    return DefWindowProc(hWnd, mes, wParam, lParam);
}

void Init(const HINSTANCE& hInstance)
{
    WNDCLASSEX wcex = {
        sizeof(WNDCLASSEX),
        CS_HREDRAW | CS_VREDRAW,
        WndProc,
        0,
        0,
        hInstance,
        NULL,
        NULL,
        (HBRUSH)(COLOR_WINDOW + 1),
        NULL,
        TITLE.c_str(), NULL
    };

    if (!RegisterClassEx(&wcex))
    {
        throw std::exception("");
    }

    if (!(m_hWnd = CreateWindow(TITLE.c_str(), TITLE.c_str(), WS_OVERLAPPEDWINDOW, CW_USEDEFAULT, 0,
        CW_USEDEFAULT, 0, NULL, NULL, hInstance, NULL)))
    {
        throw std::exception("");
    }

    if (!(m_D3D = Direct3DCreate9(D3D_SDK_VERSION)))
    {
        throw std::exception("");
    }

    D3DPRESENT_PARAMETERS d3dpp = {
        0,
        0,
        D3DFMT_UNKNOWN,
        0,
        D3DMULTISAMPLE_NONE,
        0,
        D3DSWAPEFFECT_DISCARD,
        NULL,
        TRUE,
        TRUE,
        D3DFMT_D24S8,
        0,
        D3DPRESENT_RATE_DEFAULT,
        D3DPRESENT_INTERVAL_DEFAULT
    };

    LPDIRECT3DDEVICE9 D3DDevice;
    if (FAILED(m_D3D->CreateDevice(D3DADAPTER_DEFAULT, D3DDEVTYPE_HAL, m_hWnd,
        D3DCREATE_HARDWARE_VERTEXPROCESSING, &d3dpp, &D3DDevice)))
    {
        if (FAILED(m_D3D->CreateDevice(D3DADAPTER_DEFAULT, D3DDEVTYPE_HAL, m_hWnd,
            D3DCREATE_SOFTWARE_VERTEXPROCESSING, &d3dpp, &D3DDevice)))
        {
            m_D3D->Release();
            throw std::exception("");
        }
    }

    Common::SetD3DDevice(D3DDevice);

    m_Mesh1 = new Mesh("tiger.x", D3DXVECTOR3(3, 1, 0), D3DXVECTOR3(0, 0, 0), 1.0f);
    m_AnimMesh2 = new AnimMesh("hoshiman.x", D3DXVECTOR3(0, 0, 0), D3DXVECTOR3(0, 0, 0), 0.5f);

    ShowWindow(m_hWnd, SW_SHOW);
}

void Finalize()
{
    Common::Finalize();
    m_D3D->Release();
}

int MainLoop()
{
    LPDIRECT3DDEVICE9 D3DDevice = Common::GetD3DDevice();
    MSG m_msg;

    do
    {
        Sleep(1);
        if (PeekMessage(&m_msg, NULL, 0, 0, PM_REMOVE))
        {
            DispatchMessage(&m_msg);
        }

        Update();

        D3DDevice->Clear(0, NULL, D3DCLEAR_TARGET | D3DCLEAR_ZBUFFER, D3DCOLOR_XRGB(40, 40, 80), 1.0f, 0);
        D3DDevice->BeginScene();

        m_Mesh1->Render(GetViewMatrix(), GetProjMatrix());
        m_AnimMesh2->Render(GetViewMatrix(), GetProjMatrix());

        D3DDevice->EndScene();
        D3DDevice->Present(NULL, NULL, NULL, NULL);

    } while (m_msg.message != WM_QUIT);

    return 0;
}

int APIENTRY WinMain(_In_ HINSTANCE hInstance, _In_opt_  HINSTANCE, _In_ LPSTR, _In_ int)
{
    Init(hInstance);
    MainLoop();
    Finalize();
    return 0;
}

