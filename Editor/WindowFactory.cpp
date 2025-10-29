#include "WindowFactory.h"

namespace SoulEditor
{
    std::shared_ptr<GuiWindow> WindowFactory::CreateWindow(const std::string& name)
    {
        auto it = creators_.find(name);
        if (it != creators_.end())
        {
            return it->second();
        }
        return nullptr;
    }

    void WindowFactory::RegisterWindow(const std::string& name, WindowCreator creator)
    {
        creators_[name] = creator;
    }

    bool WindowFactory::IsRegistered(const std::string& name) const
    {
        return creators_.find(name) != creators_.end();
    }

    std::vector<std::string> WindowFactory::GetRegisteredWindows() const
    {
        std::vector<std::string> names;
        for (const auto& pair : creators_)
        {
            names.push_back(pair.first);
        }
        return names;
    }
}
