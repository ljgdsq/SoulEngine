#pragma once
#include <string>
#include "Define.h"
#include "nlohmann/json.hpp"

namespace SoulEditor
{
    class EditorPrefs
    {
        SINGLETON_CLASS(EditorPrefs)

    public:
        void Initialize();
        void Load();
        void Save() const;

    public:
        template <class T>
        T Get(const std::string& key, const T& defaultValue);

        template <class T>
        void Set(const std::string& key, const T& value);

        std::string GetString(const std::string& key, const std::string& defaultValue);
        void SetString(const std::string& key, const std::string& value);

        int GetInt(const std::string& key, int defaultValue);
        void SetInt(const std::string& key, int value);

        bool GetBool(const std::string& key, bool defaultValue);
        void SetBool(const std::string& key, bool value);

        double GetDouble(const std::string& key, double defaultValue);
        void SetDouble(const std::string& key, double value);

        bool HasKey(const std::string& key) const;
        bool DeleteKey(const std::string& key);

    private:
        static std::string GetSettingsFilePath();
        bool initialized_ = false;
        nlohmann::json settings_;
    };

}
