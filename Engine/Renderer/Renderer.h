#pragma once

namespace SoulEngine {
    namespace Gfx {
        class IDevice;
        class IContext;
    }
    
    /**
     * @brief 渲染器基类 - 处理图形渲染
     */
    class Renderer {
    public:
        Renderer() = default;
        virtual ~Renderer() = default;
        
        /**
         * @brief 初始化渲染器
         */
        virtual bool Initialize(class IWindow * window) = 0;
        
        /**
         * @brief 开始新的一帧渲染
         */
        virtual void BeginFrame() = 0;
        
        /**
         * @brief 结束当前帧渲染
         */
        virtual void EndFrame() = 0;
        
        /**
         * @brief 清理渲染器
         */
        virtual void Shutdown() = 0;
        
        /**
         * @brief 清除屏幕
         */
        virtual void Clear() = 0;

        /**
         * @brief 交换前后缓冲区
         */
        virtual void SwapBuffers() = 0;

        // Optional: expose Gfx device/context if renderer provides them
        virtual Gfx::IDevice* GetGfxDevice() { return nullptr; }
        virtual Gfx::IContext* GetGfxContext() { return nullptr; }
    };
    
} // namespace SoulEngine