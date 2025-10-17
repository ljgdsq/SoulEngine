#pragma once

#include <unordered_map>
#include <typeindex>

namespace SoulEngine {

class ServiceRegistry {
public:
    void Register(std::type_index key, void* instance) {
        services_[key] = instance;
    }
    void Unregister(std::type_index key, void* instance) {
        auto it = services_.find(key);
        if (it != services_.end() && it->second == instance) {
            services_.erase(it);
        }
    }
    template <class T>
    T* Get() const {
        auto it = services_.find(std::type_index(typeid(T)));
        if (it == services_.end()) return nullptr;
        return static_cast<T*>(it->second);
    }
private:
    std::unordered_map<std::type_index, void*> services_;
};

} // namespace SoulEngine
