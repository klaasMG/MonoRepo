#ifndef SUPERBUILD_ERROR_H
#define SUPERBUILD_ERROR_H

#include <string>

enum class BaseErrorType {
    OK,
    NOT_IMPLEMENTED,
    FILE_NOT_FOUND,
    FILE_IN_USE,
    FILE_DATA_ERROR,
    QUEUE_EMPTY,
    LOGGER_ERROR,
};

std::string inline to_string(BaseErrorType type) {
    switch (type) {
        case BaseErrorType::OK:
            return "OK";
        case BaseErrorType::NOT_IMPLEMENTED:
        return "NOT_IMPLEMENTED";
        case BaseErrorType::FILE_NOT_FOUND:
        return "FILE_NOT_FOUND";
        case BaseErrorType::FILE_IN_USE:
        return "FILE_IN_USE";
        case BaseErrorType::FILE_DATA_ERROR:
        return "FILE_DATA_ERROR";
        case BaseErrorType::QUEUE_EMPTY:
        return "QUEUE_EMPTY";
        case BaseErrorType::LOGGER_ERROR:
        return "LOGGER_ERROR";
    }
    return "bitch";
};

template<typename T>
concept ErrorClass =
requires(T obj)
{
    typename T::ErrorType;
    requires std::is_enum_v<typename T::ErrorType>;
    { obj.state } -> std::same_as<typename T::ErrorType&>;
    { obj.to_string() } -> std::same_as<std::string>;
};

template<typename T>
concept HasToString =
requires(T value) {
    { to_string(value) } -> std::convertible_to<std::string>;
};

template<typename ErrorEnum>
concept IsErrorEnum = !std::is_enum_v<ErrorEnum> || HasToString<ErrorEnum>;

template<typename Data, IsErrorEnum Error>
class [[nodiscard]] Result {
public:
    Result() = delete;
    Result(const Result&) = delete;
    Result& operator=(const Result&) = delete;
    Result(Result&&) = default;
    Result& operator=(Result&&) = default;
    Result(Data data);
    Result(Data data, Error type);
    Result(Error type);
    [[nodiscard]] Error check_error();
    Data GetData() const;
    Data Handle_Error();
    ~Result();
private:
    bool is_error_handeled = false;
    bool is_error_checked = false;
    Error type;
    Data data;
};
#include "Error.tpp"

#endif //SUPERBUILD_ERROR_H