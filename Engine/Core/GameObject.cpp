#include "GameObject.h"
namespace SoulEngine
{
        GameObject::GameObject(const std::string& name) : name_(name) {}
    
    GameObject::~GameObject()
    {
        for (auto& component : components_)
        {
            if (component && component->m_state != ComponentState::Destroyed)
            {
                component->OnDestroy();
            }
        }
    }
    
    template<typename T, typename... Args>
    T* GameObject::AddComponent(Args&&... args)
    {
        static_assert(std::is_base_of_v<Component, T>, "T must inherit from Component");
        
        auto component = std::make_unique<T>(std::forward<Args>(args)...);
        T* componentPtr = component.get();
        
        // 设置关联
        component->SetGameObject(this);
        
        // 存储组件
        components_.push_back(std::move(component));
        componentMap_[std::type_index(typeid(T))] = componentPtr;
        
        // 立即调用Awake（如果GameObject已经Awake过）
        if (hasAwake_)
        {
            componentPtr->state_ = ComponentState::Awake;
            componentPtr->Awake();
            
            // 如果GameObject是激活状态，启用组件
            if (active_)
            {
                componentPtr->SetEnabled(true);
                
                // 如果已经Started，调用Start
                if (hasStarted_ && !componentPtr->hasStarted_)
                {
                    componentPtr->hasStarted_ = true;
                    componentPtr->Start();
                }
            }
        }
        
        return componentPtr;
    }
    
    template<typename T>
    T* GameObject::GetComponent() const
    {
        auto it = componentMap_.find(std::type_index(typeid(T)));
        if (it != componentMap_.end())
        {
            return static_cast<T*>(it->second);
        }
        return nullptr;
    }
    
    void GameObject::SetActive(bool active)
    {
        if (active_ == active) return;
        
        active_ = active;
        
        if (active)
        {
            InternalOnEnable();
        }
        else
        {
            InternalOnDisable();
        }
    }
    
    void GameObject::InternalAwake()
    {
        if (hasAwake_) return;
        
        hasAwake_ = true;
        
        for (auto& component : components_)
        {
            if (component->m_state == ComponentState::Created)
            {
                component->m_state = ComponentState::Awake;
                component->Awake();
                
                if (active_)
                {
                    component->SetEnable(true);
                }
            }
        }
    }
    
    void GameObject::InternalStart()
    {
        if (hasStarted_) return;
        
        hasStarted_ = true;
        
        for (auto& component : components_)
        {
            if (component->IsEnabled() && !component->hasStarted)
            {
                component->hasStarted = true;
                component->Start();
            }
        }
    }
    
    void GameObject::InternalUpdate(float deltaTime)
    {
        if (!active_) return;
        
        for (auto& component : components_)
        {
            if (component->IsEnabled())
            {
                component->Update(deltaTime);
            }
        }
    }
    
    void GameObject::InternalFixedUpdate(float fixedDeltaTime)
    {
        if (!active_) return;
        
        for (auto& component : components_)
        {
            if (component->IsEnabled())
            {
                component->FixedUpdate(fixedDeltaTime);
            }
        }
    }
    
    void GameObject::InternalLateUpdate(float deltaTime)
    {
        if (!active_) return;
        
        for (auto& component : components_)
        {
            if (component->IsEnabled())
            {
                component->LateUpdate(deltaTime);
            }
        }
    }
    
} // namespace SoulEngine

