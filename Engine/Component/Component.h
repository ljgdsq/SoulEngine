#pragma once
#include <string>
namespace SoulEngine
{
    class GameObject;

    enum class ComponentState
    {
        Created, // 组件已创建但未初始化
        Awake, // 组件已初始化
        Enabled, // 组件已启用
        Disabled, // 组件已禁用
        Destroyed // 组件已销毁
    };
    
    class Component
    {
        friend class GameObject;
    public:
        Component() = default;
        virtual ~Component() = default;

    public:
        // 生命周期方法
        virtual void Awake() {}
        virtual void Start() {}
        virtual void OnEnable() {}
        virtual void OnDisable() {}
        virtual void OnDestroy() {}
        virtual void Update(float deltaTime) {}
        virtual void FixedUpdate(float fixedDeltaTime) {}
        virtual void LateUpdate(float deltaTime) {}

    public:
        void SetEnable(bool enable);
        bool IsEnabled() const;

        bool IsActive() const;
        GameObject* GetGameObject() const;
        virtual std::string GetComponentType() const = 0;
    protected:
        ComponentState m_state;
        GameObject * m_gameObject = nullptr;
        bool hasStarted = false;
        
    };

}
