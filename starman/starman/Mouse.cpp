#include "Mouse.h"
#include <exception>

LPDIRECTINPUTDEVICE8 Mouse::m_DIMouse;
DIMOUSESTATE Mouse::m_DIMouseState;

void Mouse::Init(LPDIRECTINPUT8 directInput, HWND hWnd)
{
    HRESULT ret = directInput->CreateDevice(GUID_SysMouse, &m_DIMouse, NULL);

    ret = m_DIMouse->SetDataFormat(&c_dfDIMouse);
    ret = m_DIMouse->SetCooperativeLevel(hWnd, DISCL_FOREGROUND | DISCL_NONEXCLUSIVE);

    // デバイスの設定
    DIPROPDWORD diprop;
    diprop.diph.dwSize = sizeof(diprop);
    diprop.diph.dwHeaderSize = sizeof(diprop.diph);
    diprop.diph.dwObj = 0;
    diprop.diph.dwHow = DIPH_DEVICE;
    diprop.dwData = DIPROPAXISMODE_REL;	// 相対値モードで設定（絶対値はDIPROPAXISMODE_ABS）

    ret = m_DIMouse->SetProperty(DIPROP_AXISMODE, &diprop.diph);

    if (FAILED(ret))
    {
        throw std::exception("Failed initialize mouse");
    }
    ret = m_DIMouse->Acquire();
}

void Mouse::Update()
{
    HRESULT ret = m_DIMouse->Acquire();

    // 読取前の値を保持します
    DIMOUSESTATE g_zdiMouseState_bak;	// マウス情報(変化検知用)
    memcpy(&g_zdiMouseState_bak, &m_DIMouseState, sizeof(g_zdiMouseState_bak));

    // ここから、DirectInputで必要なコード -->
        // マウスの状態を取得します
    HRESULT	hr = m_DIMouse->GetDeviceState(sizeof(DIMOUSESTATE), &m_DIMouseState);
    if (hr == DIERR_INPUTLOST)
    {
        m_DIMouse->Acquire();
        hr = m_DIMouse->GetDeviceState(sizeof(DIMOUSESTATE), &m_DIMouseState);
    }
    // --> ここまで、DirectInputで必要なコード

    if (memcmp(&g_zdiMouseState_bak, &m_DIMouseState, sizeof(g_zdiMouseState_bak)) != 0)
    {
        // 確認用の処理、ここから -->
                // 値が変わったら表示します
        char buf[128];
        wsprintf(buf, "(%5d, %5d, %5d) %s %s %s\n",
            m_DIMouseState.lX, m_DIMouseState.lY, m_DIMouseState.lZ,
            (m_DIMouseState.rgbButtons[0] & 0x80) ? "Left" : "--",
            (m_DIMouseState.rgbButtons[1] & 0x80) ? "Right" : "--",
            (m_DIMouseState.rgbButtons[2] & 0x80) ? "Center" : "--");
        OutputDebugString(buf);
        // --> ここまで、確認用の処理
    }
}

LONG Mouse::GetX()
{
    return m_DIMouseState.lX;
}

LONG Mouse::GetY()
{
    return m_DIMouseState.lY;
}

bool Mouse::IsDownLeft()
{
    if (m_DIMouseState.rgbButtons[0] & 0x80)
    {
        return true;
    }
    return false;
}

bool Mouse::IsDownRight()
{
    if (m_DIMouseState.rgbButtons[1] & 0x80)
    {
        return true;
    }
    return false;
}
