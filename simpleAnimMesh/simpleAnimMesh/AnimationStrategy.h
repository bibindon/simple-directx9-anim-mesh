#pragma once

#include <d3d9.h>
#include <d3dx9.h>
#include <string>
#include <vector>
#include <memory>
#include <unordered_map>
#include "Common.h"

class AnimController
{
public:
    AnimController(LPD3DXANIMATIONCONTROLLER controller);
    void SetAnim(const std::string& animation_set);
    void Update();
    void SetDefaultAnim(const std::string& animation_name);
    void SetAnimConfig(
        const std::string& animation_name,
        const bool& loop,
        const float& duration);
    bool is_playing();

private:
    bool m_isPlaying { false };
    std::vector<LPD3DXANIMATIONSET> m_animSets;
    LPD3DXANIMATIONCONTROLLER m_animController { nullptr };
    std::string m_defaultAnim;
    float m_animTime;
    std::string m_currentAnim;

    struct AnimConfig
    {
        bool loop { true };
        float duration { 1.0f };
    };
    std::unordered_map<std::string, AnimConfig> m_animConfigMap;
};


