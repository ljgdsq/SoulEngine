#pragma once

namespace SoulEngine
{

    /**
     * @brief 引擎系统接口 - 所有引擎子系统需继承此接口
     */
    class IEngineSystem
    {
    public:
        virtual ~IEngineSystem() = default;
        /**
         * @brief 获取系统名称
         */
        virtual const char *GetName() const = 0;
        /**
         * @brief 获取系统初始化优先级（数值越小优先级越高）
         */
        virtual int GetPriority() const = 0;

        virtual bool Initialize(class Engine &engine) = 0;
        virtual void Update(float dt) = 0;
        virtual void Shutdown() = 0;
    };

}