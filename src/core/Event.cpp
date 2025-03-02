#include "core/Event.h"

#include "core/Log.h"

namespace core {

void Event::register_callback() {
  GLFWwindow* w_ptr = Window::m_window;
  glfwSetCursorEnterCallback(w_ptr, cursor_enter_callback);
  glfwSetCursorPosCallback(w_ptr, cursor_pos_callback);
  glfwSetMouseButtonCallback(w_ptr, mouse_button_callback);
  glfwSetKeyCallback(w_ptr, key_callback);
  glfwSetWindowSizeCallback(w_ptr, window_size_callback);
}

void Event::cursor_pos_callback(GLFWwindow* window, double xpos, double ypos) {
  if (Window::layer == Layer::Scene) {
    Input::set_cursor(xpos, ypos);
  } else if (Window::layer == Layer::ImGui) {
  } else if (Window::layer == Layer::OnViewPort) {
    Input::set_cursor_point(xpos, ypos, Window::bound_viewport_x, Window::bound_viewport_y);
  }
}

void Event::mouse_button_callback(GLFWwindow* window, int button, int action,
                                  int mods) {
  if (button == GLFW_MOUSE_BUTTON_LEFT) {
    Input::set_mouse_down(Input::LEFT, action == GLFW_PRESS);
  } else if (button == GLFW_MOUSE_BUTTON_RIGHT) {
    Input::set_mouse_down(Input::RIGHT, action == GLFW_PRESS);
  } else if (button == GLFW_MOUSE_BUTTON_MIDDLE) {
    Input::set_mouse_down(Input::MID, action == GLFW_PRESS);
  }
}

void Event::key_callback(GLFWwindow* window, int key, int scancode, int action,
                         int mods) {
  uint8_t _key = '0';

  // clang-format off
  switch (key) {
    case GLFW_KEY_W: _key = 'w'; break;
    case GLFW_KEY_S: _key = 's'; break;
    case GLFW_KEY_A: _key = 'a'; break;
    case GLFW_KEY_D: _key = 'd'; break;
    case GLFW_KEY_Q: _key = 'q'; break;
    case GLFW_KEY_E: _key = 'e'; break;
    case GLFW_KEY_ENTER: _key = '\r'; break;
  }
  // clang-format on

  if (_key == '\r') {
    if (action == GLFW_PRESS || action == GLFW_REPEAT) {
      Input::set_key_down(_key, true);
    }
    return;
  }

  if (Window::layer == Layer::Scene) {
    Input::set_key_down(_key, action != GLFW_RELEASE);
  } else if (Window::layer == Layer::OnViewPort) {
    Input::set_key_down(_key, action != GLFW_RELEASE);
  }
}

void Event::window_size_callback(GLFWwindow* window, int width, int height) {
  // Window::resize();
}

void Event::cursor_enter_callback(GLFWwindow* window, int entered) {
  if (entered) {
    Input::set_first_enter_window(true);
    CORE_INFO("Cursor enters window");
  } else {
    CORE_INFO("Cursor leaves window");
  }
}

}  // namespace core
