#include <iostream>
#include <Windows.h>
#include <cstdio>
#include <spdlog/spdlog.h>


// Allocate a console window for a WIN32 subsystem app so logs/printf are visible
static void SetupConsole()
{
    // If already has a console (debugger), skip
    if (GetConsoleWindow()) return;
    if (AllocConsole()) {
        FILE* fp = nullptr;
        // Redirect stdout/stderr to the new console
        freopen_s(&fp, "CONOUT$", "w", stdout);
        freopen_s(&fp, "CONOUT$", "w", stderr);
    }
}

int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nShowCmd)
{

        // 这些参数不使用
    UNREFERENCED_PARAMETER(hPrevInstance);
    UNREFERENCED_PARAMETER(lpCmdLine);
    UNREFERENCED_PARAMETER(nShowCmd);

AllocConsole(); 
freopen("CONOUT$", "w", stdout);

    // Show console so you can see output
    SetupConsole();

    // Initialize logging
    spdlog::set_level(spdlog::level::debug);
    spdlog::info("Starting SoulEngine DXDemo (WinMain)");
    
    std::cout << "Hello from SoulEngine DXDemo!" << std::endl;
    
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

    // Slow down so output is visible
    Sleep(250);
        
        // Simple exit condition
        if (frameCount >= 10) {
            running = false;
        }
    }
    
    spdlog::info("DXDemo finished, ran {} frames", frameCount);
    
    // Cleanup resources
    // SoulEngine::Shutdown();
    
    return 0;
}




// int main()
// {
//     // Initialize logging
//     spdlog::info("Starting SoulEngine DXDemo");
    
//     std::cout << "Hello from SoulEngine DXDemo!" << std::endl;
    
//     // Here you can add your engine initialization code
//     // SoulEngine::Initialize();
    
//     // Main loop example
//     bool running = true;
//     int frameCount = 0;
    
//     while (running && frameCount < 10) {
//         spdlog::debug("Frame {}", frameCount + 1);
        
//         // Rendering logic
//         // engine.Render();
        
//         // Update logic  
//         // engine.Update(deltaTime);
        
//         frameCount++;
        
//         // Simple exit condition
//         if (frameCount >= 10) {
//             running = false;
//         }
//     }
    
//     spdlog::info("DXDemo finished, ran {} frames", frameCount);
    
//     // Cleanup resources
//     // SoulEngine::Shutdown();
    
//     return 0;
// }