#include <SoulEngine.h>
#include "Input.h"
using namespace SoulEngine;
class MyApplication : public SoulEngine::Application
{
public:
    MyApplication() : Application("SoulEngine OpenGL Demo") {}


    bool Initialize() override
    {
        Logger::Log("MyApplication initialized");
        return true;
    }

    void Update(float deltaTime) override
    {
        if (Input::GetInstance().GetKeyDown(KeyCode::Escape))
        {
            m_window->SetShouldClose(true);
        }
            
    }

    void Render() override
    {

    }

    void Shutdown() override
    {
        SoulEngine::Logger::Log("MyApplication shutdown");
    }

    bool ShouldClose() const override
    {
        return m_window->ShouldClose();
    }
};

// 使用SoulEngine宏简化main函数
SOULENGINE_MAIN(MyApplication)