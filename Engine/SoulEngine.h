#pragma once

// SoulEngine 主头文件 - 包含所有公共API
#include "Core/Engine.h"
#include "Core/Application.h"

// 引擎命名空间
namespace SoulEngine {
    
    /**
     * @brief 引擎版本信息
     */
    struct Version {
        static constexpr int MAJOR = 0;
        static constexpr int MINOR = 1;
        static constexpr int PATCH = 0;
        
        static std::string GetString() {
            return std::to_string(MAJOR) + "." + 
                   std::to_string(MINOR) + "." + 
                   std::to_string(PATCH);
        }
    };
    
} // namespace SoulEngine

// 便于用户使用的宏定义
#define SOULENGINE_VERSION SoulEngine::Version::GetString()

/**
 * @brief 应用程序入口点宏 - 简化用户代码
 * 
 * 使用示例:
 * class MyApp : public SoulEngine::Application {
 *     // 实现虚函数...
 * };
 * 
 * SOULENGINE_MAIN(MyApp)
 */
#define SOULENGINE_MAIN(AppClass) \
    int main() { \
        SoulEngine::Engine engine; \
        if (!engine.Initialize()) { \
            return -1; \
        } \
        auto app = std::make_unique<AppClass>(); \
        int result = engine.Run(std::move(app)); \
        engine.Shutdown(); \
        return result; \
    }