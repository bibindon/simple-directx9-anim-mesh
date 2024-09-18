#pragma once

#ifndef DIRECTINPUT_VERSION
#define DIRECTINPUT_VERSION 0x0800
#endif

#include <dinput.h>

class KeyBoard
{
public:
    static void Init(LPDIRECTINPUT8 directInput, HWND hWnd);
    static void Update();
    static bool IsDown(int keyCode);
    static bool IsHold(int keyCode);
private:
    static LPDIRECTINPUTDEVICE8 m_keyboard;
    static BYTE m_key[256];
    static BYTE m_keyPrev[256];
};

