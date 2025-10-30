#pragma once
#include "Define.h"
#include "nlohmann/json.hpp"
NS_ENGINE
    class Serializable
    {
    public:
        Serializable() = default;
        virtual ~Serializable() = default;
        virtual nlohmann::json Serialize() const = 0;
        virtual void Deserialize(const nlohmann::json& jsonData) = 0;
    };

END_NS_ENGINE
