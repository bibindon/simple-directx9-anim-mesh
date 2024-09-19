#include "AnimationStrategy.h"

 AnimController::AnimController(LPD3DXANIMATIONCONTROLLER controller)
        : m_defaultAnim { "" },
        m_animTime { },
        m_currentAnim { "" }
{
    SAFE_RELEASE(m_animController);
    m_animController = controller;
    DWORD animation_count { m_animController->GetNumAnimationSets() };

    std::vector<LPD3DXANIMATIONSET> animation_sets(animation_count);

    m_animSets.swap(animation_sets);

    for (DWORD i { 0 }; i < animation_count; ++i)
    {
        LPD3DXANIMATIONSET temp_animation_set { nullptr };
        m_animController->GetAnimationSet(i, &temp_animation_set);
        SAFE_RELEASE(m_animSets.at(i));
        m_animSets.at(i) = temp_animation_set;
    }
}

void AnimController::SetAnim(const std::string& animation_set)
{
    std::vector<LPD3DXANIMATIONSET>::const_iterator kit;

    kit = std::find_if(
        m_animSets.cbegin(), m_animSets.cend(),
        [&](const LPD3DXANIMATIONSET& a)
        {
            return animation_set == a->GetName();
        });

    if (m_animSets.cend() == kit)
    {
        // TODO return error
    //    THROW_WITH_TRACE("An illegal animation set was sent.: " + animation_set);
    }

    m_animController->SetTrackAnimationSet(0, *kit);
    m_animController->SetTrackPosition(0, -1.001f / 60);
    m_animTime = 0.f;

    if (m_animConfigMap.find(animation_set) == m_animConfigMap.end())
    {
        return;
    }
    if (animation_set != m_defaultAnim &&
        !m_animConfigMap.at(animation_set).loop)
    {
        m_isPlaying = true;
        m_currentAnim = animation_set;
    }
};

void AnimController::Update()
{
    m_animTime += 1.f/60;
    m_animController->SetTrackPosition(0, 0.f);
    m_animController->AdvanceTime(m_animTime, nullptr);
    if (m_isPlaying)
    {
        float duration { m_animConfigMap.at(m_currentAnim).duration };
        if (m_animTime + 0.001f >= duration)
        {
            SetAnim(m_defaultAnim);
            m_isPlaying = false;
            m_animTime = 0;
        }
    }
};

void AnimController::SetDefaultAnim(const std::string& animation_name)
{
    m_defaultAnim = animation_name;
    SetAnim(m_defaultAnim);
}

void AnimController::SetAnimConfig(
    const std::string& animation_name, const bool& loop, const float& duration)
{
    m_animConfigMap.emplace(animation_name, AnimConfig { loop, duration });
}

bool AnimController::is_playing()
{
    return m_isPlaying;
}

