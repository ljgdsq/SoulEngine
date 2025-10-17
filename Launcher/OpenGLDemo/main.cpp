#include <SoulEngine.h>
#include <iostream>

class MyApplication : public SoulEngine::Application {
public:
    MyApplication() : Application("SoulEngine Framework Demo") {}
    
    bool Initialize() override {
        spdlog::info("Initializing {}", GetName());
        m_frameCount = 0;
        m_maxFrames = 100; // Run for 100 frames
        
        return true;
    }
    
    bool Update(float deltaTime) override {
        m_frameCount++;
        m_totalTime += deltaTime;
        
        // Log every 20 frames
        if (m_frameCount % 20 == 0) {
            spdlog::info("Frame {}: DeltaTime={:.3f}s, TotalTime={:.2f}s", 
                        m_frameCount, deltaTime, m_totalTime);
        }
        
        // Simulate some game logic
        UpdateGameLogic(deltaTime);
        
        // Continue running until max frames
        return m_frameCount < m_maxFrames;
    }
    
    void Render() override {
        // Simulate rendering
        if (m_frameCount % 30 == 0) {
            spdlog::debug("Rendering frame {}", m_frameCount);
        }
    }
    
    void Shutdown() override {
        spdlog::info("Shutting down {}. Ran {} frames in {:.2f} seconds", 
                    GetName(), m_frameCount, m_totalTime);
    }
    
    bool ShouldClose() const override {
        return m_shouldClose || m_frameCount >= m_maxFrames;
    }
    
private:
    void UpdateGameLogic(float deltaTime) {
        // Simulate some game entities
        static float rotationAngle = 0.0f;
        rotationAngle += deltaTime * 90.0f; // 90 degrees per second
        
        if (rotationAngle >= 360.0f) {
            rotationAngle -= 360.0f;
            spdlog::trace("Object completed full rotation");
        }
    }
    
    int m_frameCount = 0;
    int m_maxFrames = 100;
    float m_totalTime = 0.0f;
};

// 使用SoulEngine宏简化main函数
SOULENGINE_MAIN(MyApplication)