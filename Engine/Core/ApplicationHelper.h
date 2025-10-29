#pragma once

#include "Application.h"
#include "Engine.h"
#include "Renderer/RenderSystem.h"
#include "SoulEngine.h"
namespace SoulEngine {
    
    /**
     * @brief Application 基类的扩展，提供便捷的访问方法
     */
    class ApplicationBase : public Application {
    public:
        ApplicationBase(const std::string& name = "SoulEngine Application") 
            : Application(name) {}
        
    protected:
        /**
         * @brief 获取渲染设备
         */
        Gfx::IDevice* GetDevice() const {
            return GetEngine()->GetSystem<RenderSystem>()->GetDevice();
        }
        
        /**
         * @brief 获取渲染上下文
         */
        Gfx::IContext* GetContext() const {
            return GetEngine()->GetSystem<RenderSystem>()->GetContext();
        }
        
        /**
         * @brief 获取渲染器
         */
        Renderer* GetRenderer() const {
            return GetEngine()->GetSystem<RenderSystem>()->GetRenderer();
        }
        
        /**
         * @brief 获取渲染系统
         */
        RenderSystem* GetRenderSystem() const {
            return GetEngine()->GetSystem<RenderSystem>();
        }
        
        /**
         * @brief 获取窗口系统
         */
        template<typename T>
        T* GetSystem() const {
            return GetEngine()->GetSystem<T>();
        }

        virtual bool ShouldClose() const override{
            return m_window->ShouldClose();
        }
    };
    
} // namespace SoulEngine