/*
 * @Author       : xietao
 * @Date         : 2022-10-27 17:39:11
 * @LastEditors  : xietao
 * @LastEditTime : 2022-10-28 10:32:45
 * @FilePath     : /Learn_OpenGL/src/core/Log.h
 * @Description  : 日志格式 等级设置
 *
 * Copyright (c) 2022, All Rights Reserved.
 */
#ifndef _SRC_CORE_LOG_H_
#define _SRC_CORE_LOG_H_

#define SPDLOG_NAME
#define SPDLOG_ACTIVE_LEVEL SPDLOG_LEVEL_TRACE

#include <spdlog/spdlog.h>

#define CORE_TRACE(...) SPDLOG_LOGGER_TRACE(::core::Log::get_logger(), __VA_ARGS__)
#define CORE_DEBUG(...) SPDLOG_LOGGER_DEBUG(::core::Log::get_logger(), __VA_ARGS__)
#define CORE_INFO(...)  SPDLOG_LOGGER_INFO(::core::Log::get_logger(), __VA_ARGS__)
#define CORE_WARN(...)  SPDLOG_LOGGER_WARN(::core::Log::get_logger(), __VA_ARGS__)
#define CORE_ERROR(...) SPDLOG_LOGGER_ERROR(::core::Log::get_logger(), __VA_ARGS__)

#define CORE_ASERT(cond, ...)                           \
  {                                                     \
    if (!(cond)) {                                      \
      ::core::Log::get_logger()->critical(__VA_ARGS__); \
      assert(cnt);                                      \
    }                                                   \
  }

namespace core {
class Log {
 public:
  static void init();
  static void shut_down();
  static std::shared_ptr<spdlog::logger> get_logger() { return logger; };

 private:
  // 日志打印对象
  static std::shared_ptr<spdlog::logger> logger;
};

}  // namespace core

#endif