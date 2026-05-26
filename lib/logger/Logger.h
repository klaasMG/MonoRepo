#pragma once
#include <string>
#include <vector>
#include "../cpp_base/Error.h"

struct Empty {};

enum class LogLevel { SHUTDOWN , CRITICAL, ERROR, WARNING, INFO, DEBUG };

enum class OutputLocation { Console, File };

class Logger {
public:
    Logger(const OutputLocation& location, const LogLevel& min_log_level, const std::string& logger_name);
    Result<Empty, BaseErrorType> log_message(const LogLevel& logLevel, const std::string& logMessage);
    Logger& create_logger_leaf(const OutputLocation& location, const LogLevel& min_log_level,const std::string& logger_name);
private:
    std::vector<Logger> sub_loggers;
    OutputLocation output_location;
    LogLevel min_log_level;
    std::string logger_name;
};
