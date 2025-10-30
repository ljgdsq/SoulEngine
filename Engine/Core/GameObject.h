#pragma once
#include <string>
#include <vector>
#include "Component/Component.h"
#include <unordered_map>
#include <memory>
#include <typeindex>

namespace SoulEngine
{
class GameObject
    {
    public:
        GameObject(const std::string& name = "GameObject");
        ~GameObject();
        
        // 组件管理
        template<typename T, typename... Args>
        T* AddComponent(Args&&... args);
        
        template<typename T>
        T* GetComponent() const;
        
        template<typename T>
        std::vector<T*> GetComponents() const;
        
        template<typename T>
        bool RemoveComponent();
        
        // 对象状态
        void SetActive(bool active);
        bool IsActive() const { return active_; }
        
        const std::string& GetName() const { return name_; }
        void SetName(const std::string& name) { name_ = name; }
        
    private:
        friend class Scene;  // Scene负责调用生命周期
        
        // 生命周期管理（由Scene调用）
        void InternalAwake();
        void InternalStart();
        void InternalUpdate(float deltaTime);
        void InternalFixedUpdate(float fixedDeltaTime);
        void InternalLateUpdate(float deltaTime);
        void InternalOnEnable();
        void InternalOnDisable();
        void InternalDestroy();
        
        std::string name_;
        bool active_ = true;
        bool hasAwake_ = false;
        bool hasStarted_ = false;
        
        std::vector<std::unique_ptr<Component>> components_;
        std::unordered_map<std::type_index, Component*> componentMap_;
    };

}

