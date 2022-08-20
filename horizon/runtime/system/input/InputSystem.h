#pragma once

#include <runtime/core/math/Math.h>
#include <runtime/core/window/Window.h>
#include <runtime/function/scene/camera/Camera.h>

namespace Horizon {

class InputSystem {
  public:
    enum class Key { ESCAPE, SPACE, KEY_W, KEY_S, KEY_A, KEY_D, KEY_LCTRL, KEY_LSHIFT };

    enum class MouseButton { LEFT_BUTTON, RIGHT_BUTTON };

    enum class INPUT_STATE { PRESS, RELEASE };

  public:
    InputSystem(Window *window, Camera *camera) noexcept;

    ~InputSystem() noexcept;

    InputSystem(const InputSystem &rhs) noexcept = delete;

    InputSystem &operator=(const InputSystem &rhs) noexcept = delete;

    InputSystem(InputSystem &&rhs) noexcept = delete;

    InputSystem &operator=(InputSystem &&rhs) noexcept = delete;

  public:
    void Tick();

  private:
    void ProcessKeyboardInput();

    void ProcessMouseInput();

    bool GetKeyPress(Key key) const;

    int GetMouseButtonPress(MouseButton button) const;

    int GetMouseButtonRelease(MouseButton button) const;

  private:
    Window *m_window = nullptr;
    Camera *m_camera = nullptr;

    f32 m_last_x;
    f32 m_last_y;
    f32 m_mouse_sensitivity_x = 1.0f;
    f32 m_mouse_sensitivity_y = 1.0f;
    bool m_first_mouse;
};
} // namespace Horizon