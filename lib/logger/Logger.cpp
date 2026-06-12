#include "Logger.h"

namespace logging {
    Logger::Logger(const OutputLocation& location, const LogLevel& min_log_level, const std::string& logger_name) {
    this->logger_name = logger_name;
    this->min_log_level = min_log_level;
    this->output_location = location;
    this->sub_loggers = std::vector<std::unique_ptr<Logger>>();
}

std::string to_string(LogLevel log_level) {
    switch (log_level) {
        case LogLevel::CRITICAL: {
            return "critical";
        }
        case LogLevel::ERROR: {
            return "error";
        }
        case LogLevel::WARNING: {
            return "warning";
        }
        case LogLevel::INFO: {
            return "info";
        }
        case LogLevel::DEBUG: {
            return "debug";
        }
        case LogLevel::SHUTDOWN: {
            return "shutdown";
        }
    }
    throw std::runtime_error{"this should never happen"};
}

Result<Empty, BaseErrorType> Logger::log_message(const LogLevel& log_level, const std::string& log_message) {
    const std::string& formated_log_message = fmt::format("[{}] [{}] logged: {}", logger_name, to_string(log_level), log_message);
    if (log_level < min_log_level) {
        throw std::runtime_error{"log level is to low compared to the existing logger"};
    }
    switch (output_location) {
    case OutputLocation::CONSOLE: {
        std::cerr << formated_log_message << std::endl;
        if (log_level == LogLevel::SHUTDOWN) {
            std::exit(89);
        }
        return Result<Empty, BaseErrorType>{BaseErrorType::OK};
    }
    }
    throw std::runtime_error{"no runtime output was ever given"};
}

std::string to_string(LoggerError log_level) {
    switch (log_level) {
        case LoggerError::LOGGER_LOWER_LEVEL: {
            return "level of current logger is higher then the one off the log event";
        }
    }
    throw std::runtime_error{"this should never happen if the func is designed well"};
}

Logger& Logger::create_logger_leaf(const OutputLocation& location, const LogLevel& min_log_level_in, const std::string& logger_name_in) {
         sub_loggers.push_back(std::make_unique<Logger>(location, min_log_level_in, logger_name_in));
         return *sub_loggers.back();
}
}