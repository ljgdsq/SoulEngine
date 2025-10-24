#include "GLFW/glfw3.h"
#include "Core/Input.h"
#include "Log/Logger.h"

namespace SoulEngine
{
    namespace
    {
        static int KeyCodeToGLFWKey(KeyCode key)
        {
            switch (key)
            {
            // 功能键
            case KeyCode::Escape: return GLFW_KEY_ESCAPE;
            case KeyCode::F1: return GLFW_KEY_F1;
            case KeyCode::F2: return GLFW_KEY_F2;
            case KeyCode::F3: return GLFW_KEY_F3;
            case KeyCode::F4: return GLFW_KEY_F4;
            case KeyCode::F5: return GLFW_KEY_F5;
            case KeyCode::F6: return GLFW_KEY_F6;
            case KeyCode::F7: return GLFW_KEY_F7;
            case KeyCode::F8: return GLFW_KEY_F8;
            case KeyCode::F9: return GLFW_KEY_F9;
            case KeyCode::F10: return GLFW_KEY_F10;
            case KeyCode::F11: return GLFW_KEY_F11;
            case KeyCode::F12: return GLFW_KEY_F12;
            
            // 数字键 0-9
            case KeyCode::Key0: return GLFW_KEY_0;
            case KeyCode::Key1: return GLFW_KEY_1;
            case KeyCode::Key2: return GLFW_KEY_2;
            case KeyCode::Key3: return GLFW_KEY_3;
            case KeyCode::Key4: return GLFW_KEY_4;
            case KeyCode::Key5: return GLFW_KEY_5;
            case KeyCode::Key6: return GLFW_KEY_6;
            case KeyCode::Key7: return GLFW_KEY_7;
            case KeyCode::Key8: return GLFW_KEY_8;
            case KeyCode::Key9: return GLFW_KEY_9;
            
            // 字母键 A-Z
            case KeyCode::A: return GLFW_KEY_A;
            case KeyCode::B: return GLFW_KEY_B;
            case KeyCode::C: return GLFW_KEY_C;
            case KeyCode::D: return GLFW_KEY_D;
            case KeyCode::E: return GLFW_KEY_E;
            case KeyCode::F: return GLFW_KEY_F;
            case KeyCode::G: return GLFW_KEY_G;
            case KeyCode::H: return GLFW_KEY_H;
            case KeyCode::I: return GLFW_KEY_I;
            case KeyCode::J: return GLFW_KEY_J;
            case KeyCode::K: return GLFW_KEY_K;
            case KeyCode::L: return GLFW_KEY_L;
            case KeyCode::M: return GLFW_KEY_M;
            case KeyCode::N: return GLFW_KEY_N;
            case KeyCode::O: return GLFW_KEY_O;
            case KeyCode::P: return GLFW_KEY_P;
            case KeyCode::Q: return GLFW_KEY_Q;
            case KeyCode::R: return GLFW_KEY_R;
            case KeyCode::S: return GLFW_KEY_S;
            case KeyCode::T: return GLFW_KEY_T;
            case KeyCode::U: return GLFW_KEY_U;
            case KeyCode::V: return GLFW_KEY_V;
            case KeyCode::W: return GLFW_KEY_W;
            case KeyCode::X: return GLFW_KEY_X;
            case KeyCode::Y: return GLFW_KEY_Y;
            case KeyCode::Z: return GLFW_KEY_Z;
            
            // 箭头键
            case KeyCode::Up: return GLFW_KEY_UP;
            case KeyCode::Down: return GLFW_KEY_DOWN;
            case KeyCode::Left: return GLFW_KEY_LEFT;
            case KeyCode::Right: return GLFW_KEY_RIGHT;
            
            // 常用功能键
            case KeyCode::Space: return GLFW_KEY_SPACE;
            case KeyCode::Enter: return GLFW_KEY_ENTER;
            case KeyCode::LeftShift: return GLFW_KEY_LEFT_SHIFT;
            case KeyCode::RightShift: return GLFW_KEY_RIGHT_SHIFT;
            case KeyCode::LeftCtrl: return GLFW_KEY_LEFT_CONTROL;
            case KeyCode::RightCtrl: return GLFW_KEY_RIGHT_CONTROL;
            case KeyCode::LeftAlt: return GLFW_KEY_LEFT_ALT;
            case KeyCode::RightAlt: return GLFW_KEY_RIGHT_ALT;
            case KeyCode::Tab: return GLFW_KEY_TAB;
            case KeyCode::Backspace: return GLFW_KEY_BACKSPACE;
            case KeyCode::Delete: return GLFW_KEY_DELETE;
                
            default:
                Logger::Error("Unsupported KeyCode: {}", static_cast<int>(key));
                return -1; // 未知按键
            }
        }
    }

    /**
     * @brief 输入系统的实现类，使用Pimpl模式隐藏实现细节
     */
    class Input::InputImpl
    {
    public:
        InputImpl() = default;
        ~InputImpl() = default;

        GLFWwindow *window = nullptr;

        std::array<KeyState, GLFW_KEY_LAST + 1> keyStates{};
        std::array<KeyState, GLFW_KEY_LAST + 1> prevKeyStates{};

        std::array<KeyState, static_cast<size_t>(MouseButton::Count)> mouseButtonStates{};
        std::array<KeyState, static_cast<size_t>(MouseButton::Count)> prevMouseButtonStates{};

        double mouseX = 0.0;
        double mouseY = 0.0;
        double prevMouseX = 0.0;
        double prevMouseY = 0.0;
        double mouseDeltaX = 0.0;
        double mouseDeltaY = 0.0;

        double scrollDelta = 0.0;

        std::unordered_map<std::string, AxisType> customAxes;
        std::unordered_map<std::string, int> customKeyBindings;

        double axisValues[5] = {0.0};

        /**
         * @brief 更新键盘状态，处理按键的按下、持续按下和释放状态
         */
        void UpdateKeyboardState()
        {
            prevKeyStates = keyStates;

            for (int key = 0; key <= GLFW_KEY_LAST; ++key)
            {
                int state = glfwGetKey(window, key);

                if (state == GLFW_PRESS)
                {
                    if (keyStates[key] == KeyState::None || keyStates[key] == KeyState::Up)
                    {
                        keyStates[key] = KeyState::Down;
                    }
                    else if (keyStates[key] == KeyState::Down)
                    {
                        keyStates[key] = KeyState::Held;
                    }
                }
                else
                {
                    if (keyStates[key] == KeyState::Down || keyStates[key] == KeyState::Held)
                    {
                        keyStates[key] = KeyState::Up;
                    }
                    else if (keyStates[key] == KeyState::Up)
                    {
                        keyStates[key] = KeyState::None;
                    }
                }
            }
        }

        /**
         * @brief 更新鼠标状态，包括位置、按钮和滚轮
         */
        void UpdateMouseState()
        {
            prevMouseButtonStates = mouseButtonStates;

            prevMouseX = mouseX;
            prevMouseY = mouseY;
            glfwGetCursorPos(window, &mouseX, &mouseY);

            mouseDeltaX = mouseX - prevMouseX;
            mouseDeltaY = mouseY - prevMouseY;

            for (int button = 0; button < static_cast<int>(MouseButton::Count); ++button)
            {
                int state = glfwGetMouseButton(window, button);

                if (state == GLFW_PRESS)
                {
                    if (mouseButtonStates[button] == KeyState::None || mouseButtonStates[button] == KeyState::Up)
                    {
                        mouseButtonStates[button] = KeyState::Down;
                    }
                    else if (mouseButtonStates[button] == KeyState::Down)
                    {
                        mouseButtonStates[button] = KeyState::Held;
                    }
                }
                else
                {
                    if (mouseButtonStates[button] == KeyState::Down || mouseButtonStates[button] == KeyState::Held)
                    {
                        mouseButtonStates[button] = KeyState::Up;
                    }
                    else if (mouseButtonStates[button] == KeyState::Up)
                    {
                        mouseButtonStates[button] = KeyState::None;
                    }
                }
            }

            scrollDelta = 0.0;
        }

        /**
         * @brief 更新轴值，包括键盘轴（WASD/方向键）和鼠标轴
         */
        void UpdateAxisValues()
        {
            float horizontal = 0.0f;
            if (keyStates[GLFW_KEY_A] == KeyState::Down || keyStates[GLFW_KEY_A] == KeyState::Held)
                horizontal -= 1.0f;
            if (keyStates[GLFW_KEY_D] == KeyState::Down || keyStates[GLFW_KEY_D] == KeyState::Held)
                horizontal += 1.0f;
            if (keyStates[GLFW_KEY_LEFT] == KeyState::Down || keyStates[GLFW_KEY_LEFT] == KeyState::Held)
                horizontal -= 1.0f;
            if (keyStates[GLFW_KEY_RIGHT] == KeyState::Down || keyStates[GLFW_KEY_RIGHT] == KeyState::Held)
                horizontal += 1.0f;
            horizontal = horizontal > 1.0f ? 1.0f : (horizontal < -1.0f ? -1.0f : horizontal);
            axisValues[static_cast<int>(AxisType::KeyboardHorizontal)] = horizontal;

            float vertical = 0.0f;
            if (keyStates[GLFW_KEY_W] == KeyState::Down || keyStates[GLFW_KEY_W] == KeyState::Held)
                vertical += 1.0f;
            if (keyStates[GLFW_KEY_S] == KeyState::Down || keyStates[GLFW_KEY_S] == KeyState::Held)
                vertical -= 1.0f;
            if (keyStates[GLFW_KEY_UP] == KeyState::Down || keyStates[GLFW_KEY_UP] == KeyState::Held)
                vertical += 1.0f;
            if (keyStates[GLFW_KEY_DOWN] == KeyState::Down || keyStates[GLFW_KEY_DOWN] == KeyState::Held)
                vertical -= 1.0f;
            vertical = vertical > 1.0f ? 1.0f : (vertical < -1.0f ? -1.0f : vertical);
            axisValues[static_cast<int>(AxisType::KeyboardVertical)] = vertical;

            axisValues[static_cast<int>(AxisType::MouseX)] = mouseDeltaX / 10.0;
            axisValues[static_cast<int>(AxisType::MouseY)] = mouseDeltaY / 10.0;

            axisValues[static_cast<int>(AxisType::MouseScrollWheel)] = scrollDelta;
        }
    };

    /**
     * @brief 鼠标滚轮回调函数，由GLFW调用
     * @param window GLFW窗口指针
     * @param xoffset 水平滚动值
     * @param yoffset 垂直滚动值
     */
    static void ScrollCallback(GLFWwindow *window, double xoffset, double yoffset)
    {
        Input::GetInstance().SetScrollDelta(yoffset);
    }

    std::unique_ptr<Input> Input::s_instance = nullptr;

    /**
     * @brief 构造函数，初始化Pimpl对象
     */
    Input::Input() : m_impl(std::make_unique<InputImpl>())
    {
    }

    Input::~Input() = default;

    /**
     * @brief 获取Input单例实例
     * @return Input& 输入系统的单例引用
     */
    Input &Input::GetInstance()
    {
        if (!s_instance)
        {
            s_instance = std::unique_ptr<Input>(new Input());
        }
        return *s_instance;
    }

    /**
     * @brief 初始化输入系统
     * @param window GLFW窗口指针
     */
    void Input::Initialize(IWindow *window)
    {
        m_impl->window = static_cast<GLFWwindow *>(window->GetNativeWindowHandle());
        glfwSetScrollCallback(m_impl->window, ScrollCallback);
        Logger::Log("Input system initialized");
    }

    /**
     * @brief 更新输入系统状态，应每帧调用一次
     */
    void Input::Update()
    {
        if (!m_impl->window)
        {
            Logger::Warn("Input system not initialized with a window");
            return;
        }

        m_impl->UpdateKeyboardState();
        m_impl->UpdateMouseState();
        m_impl->UpdateAxisValues();
    }

    /***
     * @brief 检查按键是否被按下或持续按下
     * @param code KeyCode 枚举值
     * @return bool 如果按键处于Down或Held状态返回true
     */
    bool Input::GetKey(KeyCode code)
    {
        auto &instance = GetInstance();
        if (!instance.m_impl->window)
            return false;
        int keyCode = KeyCodeToGLFWKey(code);
        if (keyCode < 0 || keyCode >= static_cast<int>(instance.m_impl->keyStates.size()))
        {
            Logger::Error("Invalid KeyCode: {}", static_cast<int>(code));
            return false;
        }
        return instance.m_impl->keyStates[keyCode] == KeyState::Down ||
               instance.m_impl->keyStates[keyCode] == KeyState::Held;
    }

    /**
     * @brief 检查按键是否被按下或持续按下
     * @param keyCode GLFW键码
     * @return bool 如果按键处于Down或Held状态返回true
     */
    bool Input::GetKey(int keyCode)
    {
        auto &instance = GetInstance();
        if (!instance.m_impl->window)
            return false;

        return instance.m_impl->keyStates[keyCode] == KeyState::Down ||
               instance.m_impl->keyStates[keyCode] == KeyState::Held;
    }

    /**
     * @brief 检查按键是否刚刚被按下
     * @param key 按键枚举
     * @return bool 如果按键处于Down状态返回true
     */
    bool Input::GetKeyDown(KeyCode key)
    {
        auto &instance = GetInstance();
        if (!instance.m_impl->window)
            return false;

        int keyCode = KeyCodeToGLFWKey(key);
        if (keyCode < 0 || keyCode >= static_cast<int>(instance.m_impl->keyStates.size()))
        {
            Logger::Error("Invalid KeyCode: {}", static_cast<int>(key));
            return false;
        }
        return instance.m_impl->keyStates[keyCode] == KeyState::Down;
    }

    /**
     * @brief 检查按键是否刚刚被按下
     * @param keyCode GLFW键码
     * @return bool 如果按键处于Down状态返回true
     */
    bool Input::GetKeyDown(int keyCode)
    {
        auto &instance = GetInstance();
        if (!instance.m_impl->window)
            return false;

        return instance.m_impl->keyStates[keyCode] == KeyState::Down;
    }

    /**
     * @brief 检查按键是否刚刚被释放
     * @param key 按键枚举
     * @return bool 如果按键处于Up状态返回true
     */
    bool Input::GetKeyUp(KeyCode key)
    {
        auto &instance = GetInstance();
        if (!instance.m_impl->window)
            return false;

        int keyCode = KeyCodeToGLFWKey(key);
        if (keyCode < 0 || keyCode >= static_cast<int>(instance.m_impl->keyStates.size()))
        {
            Logger::Error("Invalid KeyCode: {}", static_cast<int>(key));
            return false;
        }
        return instance.m_impl->keyStates[keyCode] == KeyState::Up;
    }

    /**
     * @brief 检查按键是否刚刚被释放
     * @param keyCode GLFW键码
     * @return bool 如果按键处于Up状态返回true
     */
    bool Input::GetKeyUp(int keyCode)
    {
        auto &instance = GetInstance();
        if (!instance.m_impl->window)
            return false;

        return instance.m_impl->keyStates[keyCode] == KeyState::Up;
    }

    /**
     * @brief 检查鼠标按钮是否被按下或持续按下
     * @param button 鼠标按钮枚举
     * @return bool 如果按钮处于Down或Held状态返回true
     */
    bool Input::GetMouseButton(MouseButton button)
    {
        auto &instance = GetInstance();
        if (!instance.m_impl->window)
            return false;

        return instance.m_impl->mouseButtonStates[static_cast<size_t>(button)] == KeyState::Down ||
               instance.m_impl->mouseButtonStates[static_cast<size_t>(button)] == KeyState::Held;
    }

    /**
     * @brief 检查鼠标按钮是否刚刚被按下
     * @param button 鼠标按钮枚举
     * @return bool 如果按钮处于Down状态返回true
     */
    bool Input::GetMouseButtonDown(MouseButton button)
    {
        auto &instance = GetInstance();
        if (!instance.m_impl->window)
            return false;

        return instance.m_impl->mouseButtonStates[static_cast<size_t>(button)] == KeyState::Down;
    }

    /**
     * @brief 检查鼠标按钮是否刚刚被释放
     * @param button 鼠标按钮枚举
     * @return bool 如果按钮处于Up状态返回true
     */
    bool Input::GetMouseButtonUp(MouseButton button)
    {
        auto &instance = GetInstance();
        if (!instance.m_impl->window)
            return false;

        return instance.m_impl->mouseButtonStates[static_cast<size_t>(button)] == KeyState::Up;
    }

    /**
     * @brief 获取鼠标X坐标
     * @return float 鼠标当前X坐标
     */
    float Input::GetMouseX()
    {
        auto &instance = GetInstance();
        if (!instance.m_impl->window)
            return 0.0f;

        return static_cast<float>(instance.m_impl->mouseX);
    }

    /**
     * @brief 获取鼠标Y坐标
     * @return float 鼠标当前Y坐标
     */
    float Input::GetMouseY()
    {
        auto &instance = GetInstance();
        if (!instance.m_impl->window)
            return 0.0f;

        return static_cast<float>(instance.m_impl->mouseY);
    }

    /**
     * @brief 获取鼠标位置
     * @param x 用于存储X坐标的引用
     * @param y 用于存储Y坐标的引用
     */
    void Input::GetMousePosition(float &x, float &y)
    {
        auto &instance = GetInstance();
        if (!instance.m_impl->window)
        {
            x = 0.0f;
            y = 0.0f;
            return;
        }

        x = static_cast<float>(instance.m_impl->mouseX);
        y = static_cast<float>(instance.m_impl->mouseY);
    }

    /**
     * @brief 获取鼠标滚轮增量
     * @return float 当前帧的滚轮增量
     */
    float Input::GetMouseScrollDelta()
    {
        auto &instance = GetInstance();
        if (!instance.m_impl->window)
            return 0.0f;

        return static_cast<float>(instance.m_impl->scrollDelta);
    }

    /**
     * @brief 获取输入轴的值
     * @param axis 轴类型枚举
     * @return float 轴值，范围从-1到1
     */
    float Input::GetAxis(AxisType axis)
    {
        auto &instance = GetInstance();
        if (!instance.m_impl->window)
            return 0.0f;

        return static_cast<float>(instance.m_impl->axisValues[static_cast<int>(axis)]);
    }

    /**
     * @brief 获取输入轴的原始值，不使用平滑处理
     * @param axis 轴类型枚举
     * @return float 对于键盘轴返回-1、0或1，对于其他轴返回原始值
     */
    float Input::GetAxisRaw(AxisType axis)
    {
        auto &instance = GetInstance();
        if (!instance.m_impl->window)
            return 0.0f;

        float value = static_cast<float>(instance.m_impl->axisValues[static_cast<int>(axis)]);
        if (axis == AxisType::KeyboardHorizontal || axis == AxisType::KeyboardVertical)
        {
            if (value > 0.1f)
                return 1.0f;
            if (value < -0.1f)
                return -1.0f;
            return 0.0f;
        }
        return value;
    }

    /**
     * @brief 注册自定义轴映射
     * @param name 轴的名称
     * @param axisType 轴类型枚举
     */
    void Input::RegisterAxis(const std::string &name, AxisType axisType)
    {
        auto &instance = GetInstance();
        instance.m_impl->customAxes[name] = axisType;
    }

    /**
     * @brief 注册自定义按键映射
     * @param name 按键的名称
     * @param keyCode GLFW键码
     */
    void Input::RegisterKeyBinding(const std::string &name, int keyCode)
    {
        auto &instance = GetInstance();
        instance.m_impl->customKeyBindings[name] = keyCode;
    }

    /**
     * @brief 根据名称获取轴的值
     * @param name 自定义轴的名称
     * @return float 轴值，范围从-1到1，若未找到则返回0
     */
    float Input::GetAxis(const std::string &name)
    {
        auto &instance = GetInstance();
        if (!instance.m_impl->window)
            return 0.0f;

        auto it = instance.m_impl->customAxes.find(name);
        if (it != instance.m_impl->customAxes.end())
        {
            return GetAxis(it->second);
        }

        Logger::Warn("Axis not found: {}", name);
        return 0.0f;
    }

    /**
     * @brief 根据名称检查按键是否被按下
     * @param name 自定义按键的名称
     * @return bool 如果按键被按下返回true，若未找到则返回false
     */
    bool Input::GetButton(const std::string &name)
    {
        auto &instance = GetInstance();
        if (!instance.m_impl->window)
            return false;

        auto it = instance.m_impl->customKeyBindings.find(name);
        if (it != instance.m_impl->customKeyBindings.end())
        {
            return GetKey(it->second);
        }

        Logger::Warn("Button not found: {}", name);

        return false;
    }

    /**
     * @brief 根据名称检查按键是否刚刚被按下
     * @param name 自定义按键的名称
     * @return bool 如果按键刚刚被按下返回true，若未找到则返回false
     */
    bool Input::GetButtonDown(const std::string &name)
    {
        auto &instance = GetInstance();
        if (!instance.m_impl->window)
            return false;

        auto it = instance.m_impl->customKeyBindings.find(name);
        if (it != instance.m_impl->customKeyBindings.end())
        {
            return GetKeyDown(it->second);
        }

        Logger::Warn("Button not found: {}", name);
        return false;
    }

    /**
     * @brief 根据名称检查按键是否刚刚被释放
     * @param name 自定义按键的名称
     * @return bool 如果按键刚刚被释放返回true，若未找到则返回false
     */
    bool Input::GetButtonUp(const std::string &name)
    {
        auto &instance = GetInstance();
        if (!instance.m_impl->window)
            return false;

        auto it = instance.m_impl->customKeyBindings.find(name);
        if (it != instance.m_impl->customKeyBindings.end())
        {
            return GetKeyUp(it->second);
        }

        Logger::Warn("Button not found: {}", name);
        return false;
    }

    /**
     * @brief 获取触摸输入（基础实现，可根据平台扩展）
     * @param touchIndex 触摸索引
     * @param x 用于存储X坐标的引用
     * @param y 用于存储Y坐标的引用
     * @return bool 如果触摸有效返回true
     */
    bool Input::GetTouch(int touchIndex, float &x, float &y)
    {
        x = 0.0f;
        y = 0.0f;
        return false;
    }

    /**
     * @brief 获取当前触摸数量
     * @return int 当前触摸数量
     */
    int Input::GetTouchCount()
    {
        return 0;
    }

    /**
     * @brief 设置滚轮增量值（由滚轮回调函数调用）
     * @param delta 滚轮增量值
     */
    void Input::SetScrollDelta(double delta)
    {
        m_impl->scrollDelta = delta;
    }

} // namespace framework
