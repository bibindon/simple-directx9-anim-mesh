#pragma once

#include <d3d9.h>

class SharedObj
{
public:
    static LPDIRECT3DDEVICE9 GetD3DDevice();
    static void SetD3DDevice(const LPDIRECT3DDEVICE9 D3DDevice);
    static void Finalize();

private:
    static LPDIRECT3DDEVICE9 m_D3DDevice;
};

