/*
 * @Author       : xietao
 * @Date         : 2022-07-27 11:37:36
 * @LastEditors  : xietao
 * @LastEditTime : 2022-12-15 23:20:16
 * @FilePath     : /Learn_OpenGL/src/core/Window.h
 * @brief  : 窗口系统 储存和设置各种窗口事件供调用
 *
 * Copyright (c) 2022, All Rights Reserved.
 */
#ifndef _LEARN_OPENGL_SRC_CORE_WINDOW_H_
#define _LEARN_OPENGL_SRC_CORE_WINDOW_H_

// clang-format off
#include <string>

#include "glad/glad.h"
#include "GLFW/glfw3.h"

// clang-format on

namespace core {
enum Layer { Scene = 0, ImGui, OnViewPort};
class Window {
 public:
  /**
   * @brief : 初始化资源
   * @return       {*}
   */
  static void init();

  /**
   * @brief : 执行双缓存和窗口事件回调
   * @return       {*}
   */
  static void update();

  /**
   * @brief : 清空颜色缓冲和深度缓冲
   * @return       {*}
   */
  static void clear_buffer();

  /**
   * @brief : 结束程序
   * @return       {*}
   */
  static void clear();

  /**
   * @brief : 窗口重命名
   * @param        {string&} title
   * @return       {*}
   */
  static void rename(const std::string& title);

  static bool should_close();

  static void resize();

  /**
   * @brief : 切换layer
   * @return       {*}
   */
  static void on_layer_switch();

  static inline GLuint m_width = 0, m_height = 0;
  static inline GLuint m_x = 0, m_y = 0;
  static inline std::string title = "";
  static inline GLFWwindow* m_window = 0;
  static inline Layer layer = Layer::ImGui;
  static inline float bound_viewport_x = 0;
  static inline float bound_viewport_y = 0;

 private:
  Window();
  Window(const Window&) = delete;
  Window& operator=(const Window&) = delete;
  ~Window();
};
}  // namespace core

#endif
