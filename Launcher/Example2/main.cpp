#include <iostream>
#include <vector>
#include <memory>
#include <spdlog/spdlog.h>

// Simple scene object class
class SceneObject {
public:
    SceneObject(const std::string& name) : m_name(name) {
        spdlog::debug("Created scene object: {}", m_name);
    }
    
    virtual ~SceneObject() {
        spdlog::debug("Destroyed scene object: {}", m_name);
    }
    
    virtual void Update(float deltaTime) {
        // Update logic
        m_lifetime += deltaTime;
    }
    
    virtual void Render() const {
        spdlog::trace("Rendering object: {} (lifetime: {:.2f}s)", m_name, m_lifetime);
    }
    
    const std::string& GetName() const { return m_name; }
    float GetLifetime() const { return m_lifetime; }

private:
    std::string m_name;
    float m_lifetime = 0.0f;
};

// Simple scene manager
class SceneManager {
public:
    void AddObject(std::unique_ptr<SceneObject> obj) {
        spdlog::info("Adding scene object: {}", obj->GetName());
        m_objects.push_back(std::move(obj));
    }
    
    void UpdateAll(float deltaTime) {
        for (auto& obj : m_objects) {
            obj->Update(deltaTime);
        }
    }
    
    void RenderAll() const {
        spdlog::debug("Starting to render {} objects", m_objects.size());
        for (const auto& obj : m_objects) {
            obj->Render();
        }
    }
    
    size_t GetObjectCount() const { return m_objects.size(); }

private:
    std::vector<std::unique_ptr<SceneObject>> m_objects;
};

int main()
{
    // Initialize logging
    spdlog::set_level(spdlog::level::debug);
    spdlog::info("Starting SoulEngine Example2 - Scene Management Demo");
    
    // Create scene manager
    SceneManager sceneManager;
    
    // Add some scene objects
    sceneManager.AddObject(std::make_unique<SceneObject>("Cube"));
    sceneManager.AddObject(std::make_unique<SceneObject>("Sphere"));
    sceneManager.AddObject(std::make_unique<SceneObject>("Light"));
    
    spdlog::info("Scene initialization complete, total {} objects", sceneManager.GetObjectCount());
    
    // Simulate game loop
    const float targetFPS = 60.0f;
    const float deltaTime = 1.0f / targetFPS;
    const int maxFrames = 120; // Run for 2 seconds
    
    for (int frame = 0; frame < maxFrames; ++frame) {
        // Output status every 30 frames
        if (frame % 30 == 0) {
            spdlog::info("Frame {}: runtime {:.2f}s", frame, frame * deltaTime);
        }
        
        // Update scene
        sceneManager.UpdateAll(deltaTime);
        
        // Render scene (every 10 frames to reduce log output)
        if (frame % 10 == 0) {
            sceneManager.RenderAll();
        }
    }
    
    spdlog::info("Example2 completed, ran {} frames total", maxFrames);
    spdlog::info("Simulated runtime: {:.2f} seconds", maxFrames * deltaTime);
    
    return 0;
}