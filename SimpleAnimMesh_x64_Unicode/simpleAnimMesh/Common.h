#pragma once
#include <codecvt>

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

std::string ConvertWstringToUTF8(const std::wstring& src);

