#include <iostream>
#include <spdlog/spdlog.h>

int main()
{
    // Initialize logging
    spdlog::info("Starting SoulEngine Example1");
    
    std::cout << "Hello from SoulEngine Example1!" << std::endl;
    
    // Here you can add your engine initialization code
    // SoulEngine::Initialize();
    
    // Main loop example
    bool running = true;
    int frameCount = 0;
    
    while (running && frameCount < 10) {
        spdlog::debug("Frame {}", frameCount + 1);
        
        // Rendering logic
        // engine.Render();
        
        // Update logic  
        // engine.Update(deltaTime);
        
        frameCount++;
        
        // Simple exit condition
        if (frameCount >= 10) {
            running = false;
        }
    }
    
    spdlog::info("Example1 finished, ran {} frames", frameCount);
    
    // Cleanup resources
    // SoulEngine::Shutdown();
    
    return 0;
}