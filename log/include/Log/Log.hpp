
#ifndef LOG_INCLUDE_LOG_LOG_HPP_
#define LOG_INCLUDE_LOG_LOG_HPP_

#include "LogLevels.hpp"

// define logging level
#ifndef LOG_LEVEL_LEVEL
#define LOG_LEVEL_LEVEL LOG_LEVEL_TRACE
#endif

#include <memory>
#include <string>

#include "spdlog/spdlog.h"

class Log {
 public:
  static void init();
  static void initClean();
  static void init(std::string loggerName);
  inline static std::shared_ptr<spdlog::logger>& getLogger() {
    return s_logger;
  }

 private:
  static void _init(std::string loggerName);
  static void _initClean();
  static std::shared_ptr<spdlog::logger> s_logger;
};

// logging macros
#define LTRACE(...) SPDLOG_LOGGER_TRACE(Log::getLogger(), __VA_ARGS__)
#define LDEBUG(...) SPDLOG_LOGGER_DEBUG(Log::getLogger(), __VA_ARGS__)
#define LINFO(...) SPDLOG_LOGGER_INFO(Log::getLogger(), __VA_ARGS__)
#define LWARN(...) SPDLOG_LOGGER_WARN(Log::getLogger(), __VA_ARGS__)
#define LERROR(...) SPDLOG_LOGGER_ERROR(Log::getLogger(), __VA_ARGS__)
#define LFATAL(...) SPDLOG_LOGGER_FATAL(Log::getLogger(), __VA_ARGS__)

// strip out logs to not be included in binary
#if LOG_LEVEL_LEVEL == LOG_LEVEL_TRACE
// NOTHING
#elif LOG_LEVEL_LEVEL == LOG_LEVEL_DEBUG
#undef LTRACE
#define LTRACE
#elif LOG_LEVEL_LEVEL == LOG_LEVEL_INFO
#undef LTRACE
#define LTRACE

#undef LDEBUG
#define LDEBUG
#elif LOG_LEVEL_LEVEL == LOG_LEVEL_WARN
#undef LTRACE
#define LTRACE

#undef LDEBUG
#define LDEBUG

#undef LINFO
#define LINFO
#elif LOG_LEVEL_LEVEL == LOG_LEVEL_ERROR
#undef LTRACE
#define LTRACE

#undef LDEBUG
#define LDEBUG

#undef LINFO
#define LINFO

#undef LWARN
#define LWARN
#elif LOG_LEVEL_LEVEL == LOG_LEVEL_FATAL
#undef LTRACE
#define LTRACE

#undef LDEBUG
#define LDEBUG

#undef LINFO
#define LINFO

#undef LWARN
#define LWARN

#undef LERROR
#define LERROR
#endif

#endif  // LOG_INCLUDE_LOG_LOG_HPP_