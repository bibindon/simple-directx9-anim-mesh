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
    virtual void update() = 0;
    virtual void set_animation(const std::string&) { }
    virtual void set_default_animation(const std::string&) { }
    virtual void set_animation_config(const std::string&, const bool&, const float&) { }
    virtual ~animation_strategy() { }
    virtual bool is_playing() { return false; }
protected:
    bool is_playing_ { false };
    std::vector<LPD3DXANIMATIONSET> animation_sets_;
    LPD3DXANIMATIONCONTROLLER animation_controller_ { nullptr };
};

class no_animation : public animation_strategy
{
public:
    void update() override;
};

class normal_animation : public animation_strategy
{
public:
    normal_animation(LPD3DXANIMATIONCONTROLLER controller);

    void set_animation(const std::string& animation_set) override;
    void update() override;
    void set_default_animation(const std::string& animation_name) override;
    void set_animation_config(
        const std::string& animation_name,
        const bool& loop,
        const float& duration) override;
    virtual bool is_playing() override;

private:
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

