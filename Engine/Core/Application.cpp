#include "Application.h"
#include <spdlog/spdlog.h>

namespace SoulEngine {
    
    Application::Application(const std::string& name) 
        : m_name(name) {
        spdlog::info("Application '{}' created", m_name);
    }
    
} // namespace SoulEngine