#pragma once

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

enum class eSequence
{
    TITLE,
    OPENING,
    BATTLE,
    ENDING
};

class Common
{
public:
    static std::vector<char> get_model_texture_resource(
        const std::string& model_name, const std::string& texture_name);
    static std::vector<char> get_model_resource(const std::string& model_name);
    static std::vector<char> get_sound_resource(const std::string& filename);
    static constexpr int KEY_DEQUE_MAX_SIZE { 300 };
    static constexpr int SIMULTANEOUS_ALLOW_FRAME { 3 };
    static constexpr float ANIMATION_SPEED { 1.0f / 60 };
};

