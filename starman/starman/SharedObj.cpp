#include "SharedObj.h"
#include "Common.h"

LPDIRECT3DDEVICE9 SharedObj::m_D3DDevice { nullptr };

LPDIRECT3DDEVICE9 SharedObj::GetD3DDevice()
{
    return m_D3DDevice;
}

void SharedObj::Finalize()
{
    SAFE_RELEASE(m_D3DDevice);
}

void SharedObj::SetD3DDevice(const LPDIRECT3DDEVICE9 D3DDevice)
{
    m_D3DDevice = D3DDevice;
}
