#pragma once
#include <string>
#include "Log/Logger.h"
namespace SoulEngine
{
    /**
     * @brief 引擎系统接口 - 所有引擎子系统需继承此接口
     */
    class SystemInterface
    {
    public:
        virtual ~SystemInterface() = default;
        SystemInterface(const std::string &name) : name_(name) {}
        /**
         * @brief 获取系统名称
         */
        std::string GetName() const { return name_; }
        /**
         * @brief 获取系统初始化优先级，数值越小优先级越高
         */
        virtual int GetPriority() const { return 100; }

        /**
         * @brief 设置引擎实例
         */
        void SetEngine(class Engine *engine) { engine_ = engine; }

        /**
         * @brief 获取引擎实例
         */
        Engine *GetEngine() const { return engine_; }

        /**
         * @brief 初始化系统
         * @param engine 引擎实例
         * @return true if successful
         */
        virtual bool Initialize() = 0;
        /**
         * @brief 每帧更新系统
         * @param dt 与上一帧的时间差(秒)
         */
        virtual void Update(float dt) = 0;

        /**
         * @brief 关闭系统，释放资源
         */
        virtual void Shutdown() = 0;

    private:
        class Engine *engine_ = nullptr;
        std::string name_;
    };

}