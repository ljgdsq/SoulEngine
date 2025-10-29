#pragma once
#include "WindowFactory.h"

namespace SoulEditor
{
    template <class T>
    class WindowRegister
    {
    public:
        WindowRegister(const std::string name)
        {
            WindowFactory::GetInstance().RegisterWindow(name, []()
            {
                return std::make_shared<T>();
            });
        }
    };
}

#define REGISTER_WINDOW(WindowClass)  \
    namespace {   \
    static WindowRegister<WindowClass> g_window_register_##WindowClass(#WindowClass); \
    }

#define REGISTER_WINDOW_NAMED(NAME,WindowClass)  \
    namespace { \
    static WindowRegister<WindowClass> g_window_register_##WindowClass(NAME); \
    }
