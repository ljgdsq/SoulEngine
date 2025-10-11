#pragma once

namespace SoulEngine {
    
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
        virtual bool Initialize() = 0;
        
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
    };
    
} // namespace SoulEngine