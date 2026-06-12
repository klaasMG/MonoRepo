#pragma once
#include <memory>
#include <string>
#include <vector>
#include "../cpp_base/Error.h"
#include "fmt/format.h"

struct Empty {};
namespace logging {
    enum class LogLevel { SHUTDOWN , CRITICAL, ERROR, WARNING, INFO, DEBUG };

    enum class OutputLocation { CONSOLE };

    enum class LoggerError { LOGGER_LOWER_LEVEL };

    std::string to_string(LogLevel log_level);

    class Logger {
    public:
        Logger(const OutputLocation& location, const LogLevel& min_log_level, const std::string& logger_name);
        Result<Empty, BaseErrorType> log_message(const LogLevel& log_level, const std::string& log_message);
        Logger& create_logger_leaf(const OutputLocation& location, const LogLevel& min_log_level,const std::string& logger_name);
    private:
        std::vector<std::unique_ptr<Logger>> sub_loggers;
        OutputLocation output_location;
        LogLevel min_log_level;
        std::string logger_name;
    };
}
