#include "Logger.h"

Logger::Logger(const OutputLocation& location, const LogLevel& min_log_level, const std::string& logger_name) {
    this->logger_name = logger_name;
    this->min_log_level = min_log_level;
    this->output_location = location;
    this->sub_loggers = {};
}

Result<Empty, BaseErrorType> Logger::log_message(const LogLevel& logLevel, const std::string& logMessage) {

}
