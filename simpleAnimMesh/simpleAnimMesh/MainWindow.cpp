#include "MainWindow.h"
#include <exception>
#include "Common.h"
#include "Camera.h"

LRESULT CALLBACK WndProc(HWND hWnd, UINT mes, WPARAM wParam, LPARAM lParam)
{
    if (mes == WM_DESTROY)
    {
        PostQuitMessage(0);
        return 0;
    }

    return DefWindowProc(hWnd, mes, wParam, lParam);
}

MainWindow::MainWindow(const HINSTANCE& hInstance)
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

MainWindow::~MainWindow()
{
    Common::Finalize();
    m_D3D->Release();
}

int MainWindow::MainLoop()
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

        Camera::Update();

        D3DDevice->Clear(0, NULL, D3DCLEAR_TARGET | D3DCLEAR_ZBUFFER, D3DCOLOR_XRGB(40, 40, 80), 1.0f, 0);
        D3DDevice->BeginScene();

        m_Mesh1->Render();
        m_AnimMesh2->Render();

        D3DDevice->EndScene();
        D3DDevice->Present(NULL, NULL, NULL, NULL);

    } while (m_msg.message != WM_QUIT);

    return 0;
}
