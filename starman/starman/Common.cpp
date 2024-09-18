#include "Common.h"
#include <d3dx9.h>

LPDIRECT3DDEVICE9 Common::m_D3DDevice { nullptr };

LPDIRECT3DDEVICE9 Common::GetD3DDevice()
{
    return m_D3DDevice;
}

void Common::Finalize()
{
    SAFE_RELEASE(m_D3DDevice);
}

void Common::SetD3DDevice(const LPDIRECT3DDEVICE9 D3DDevice)
{
    m_D3DDevice = D3DDevice;
}
