
#include "include/Log/Log.hpp"
#include "spdlog/sinks/stdout_color_sinks.h"

std::shared_ptr<spdlog::logger> Log::s_logger;

void Log::init(){_init("SIM");}
void Log::initClean(){_initClean();}

void Log::init(std::string loggerName){_init(loggerName);}

void Log::_init(std::string loggerName){
    spdlog::set_pattern("%^[%c %S.%e.%F] [%n] [%L] [thread %t] %s:%#: %v %$");
    s_logger = spdlog::stdout_color_mt(loggerName);
    s_logger->set_level(spdlog::level::trace);
}

void Log::_initClean(){
    spdlog::set_pattern("$^$v %$");
    s_logger = spdlog::stdout_color_mt("blank");
    s_logger->set_level(spdlog::level::trace);
}
