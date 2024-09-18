#pragma once

#include <d3d9.h>
#include <vector>
#include <string>

template <typename T>
inline void SAFE_DELETE(T*& p)
{
    delete p;
    p = nullptr;
}
template <typename T>
inline void SAFE_DELETE_ARRAY(T*& p)
{
    delete[] p;
    p = nullptr;
}
template <typename T>
inline void SAFE_RELEASE(T*& p)
{
    if (p)
    {
        p->Release();
        p = nullptr;
    }
}

class Common
{
public:
    static constexpr int KEY_DEQUE_MAX_SIZE { 300 };
    static constexpr int SIMULTANEOUS_ALLOW_FRAME { 3 };
    static constexpr float ANIMATION_SPEED { 1.0f / 60 };

    static LPDIRECT3DDEVICE9 GetD3DDevice();
    static void SetD3DDevice(const LPDIRECT3DDEVICE9 D3DDevice);
    static void Finalize();

private:
    static LPDIRECT3DDEVICE9 m_D3DDevice;
};

