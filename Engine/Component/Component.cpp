#include "Component.h"
#include "Core/GameObject.h"

namespace SoulEngine
{
    void Component::SetEnable(bool enable)
    {
        
        if (m_state == ComponentState::Destroyed)
            return;
            
        if (enable && m_state == ComponentState::Disabled)
        {
            m_state = ComponentState::Enabled;
            OnEnable();
            
            // 如果还没有调用过Start，且现在是启用状态，调用Start
            if (!hasStarted)
            {
                hasStarted = true;
                Start();
            }
        }
        else if (!enable && m_state == ComponentState::Enabled)
        {
            m_state = ComponentState::Disabled;
            OnDisable();
        }
    }

    bool Component::IsEnabled() const
    {
        return m_state == ComponentState::Enabled;
    }

    bool Component::IsActive() const
    {
        return IsEnabled() && m_gameObject->IsActive();
    }

    GameObject* Component::GetGameObject() const
    {
        return m_gameObject;
    }
}
