// Engine.inl - template implementations for Engine system management
#pragma once

#include <type_traits>
#include <algorithm>

namespace SoulEngine
{

template <class T, class... Args>
T* Engine::RegisterSystem(Args&&... args)
{
    static_assert(std::is_base_of<SystemInterface, T>::value, "T must derive from SystemInterface");

    auto system = std::make_unique<T>(std::forward<Args>(args)...);
    system->SetEngine(this);
    system->Initialize();
    T* raw = system.get();

    // If SystemInterface provides GetPriority(), insert by priority (higher priority first)
    // Otherwise push_back
    // We detect presence of GetPriority using SFINAE on a member function pointer
    using HasGetPriority = decltype(&SystemInterface::GetPriority);
    if constexpr (std::is_member_function_pointer_v<HasGetPriority>)
    {
        int pri = raw->GetPriority();
        auto it = std::find_if(m_systems.begin(), m_systems.end(),
            [pri](const std::unique_ptr<SystemInterface>& s) { return s->GetPriority() < pri; });
        m_systems.insert(it, std::move(system));
    }
    else
    {
        m_systems.push_back(std::move(system));
    }

    return raw;
}

template <class T>
void Engine::UnregisterSystem()
{
    for (auto it = m_systems.begin(); it != m_systems.end(); )
    {
        if (dynamic_cast<T*>(it->get()))
        {
            // Shutdown the system before erasing
            (*it)->Shutdown();
            it = m_systems.erase(it);
        }
        else
        {
            ++it;
        }
    }
}

template <class T>
T* Engine::GetSystem() const
{
    for (const auto& s : m_systems)
    {
        if (auto casted = dynamic_cast<T*>(s.get()))
            return casted;
    }
    return nullptr;
}

} // namespace SoulEngine
