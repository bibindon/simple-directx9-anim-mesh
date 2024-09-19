#pragma once

#include <d3d9.h>
#include <d3dx9.h>
#include <string>
#include <vector>
#include <memory>
#include <unordered_map>
#include "Common.h"

class animation_strategy
{
public:
    animation_strategy(LPD3DXANIMATIONCONTROLLER controller);
    void set_animation(const std::string& animation_set);
    void update();
    void set_default_animation(const std::string& animation_name);
    void set_animation_config(
        const std::string& animation_name,
        const bool& loop,
        const float& duration);
    bool is_playing();

private:
    bool is_playing_ { false };
    std::vector<LPD3DXANIMATIONSET> animation_sets_;
    LPD3DXANIMATIONCONTROLLER animation_controller_ { nullptr };
    std::string default_animation_;
    float animation_time_;
    std::string playing_animation_;

    struct animation_config
    {
        bool loop_ { true };
        float duration_ { 1.0f };
    };
    std::unordered_map<std::string, animation_config> animation_configs_;
};


