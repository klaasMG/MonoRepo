#ifndef SUPERBUILD_ERROR_H
#define SUPERBUILD_ERROR_H

enum class BaseErrorType {
    OK,
    NOT_IMPLEMENTED,
    FILE_NOT_FOUND,
    FILE_IN_USE,
    FILE_DATA_ERROR,
    QUEUE_EMPTY,
};

template<typename T>
concept HasStateAndUpdate =
requires(T obj)
{
    typename T::State;
    requires std::is_enum_v<typename T::State>;
    { obj.state } -> std::same_as<typename T::State&>;
    { obj.to_string() } -> std::same_as<std::string>;
};

template<typename Data, typename Error>
class [[nodiscard]] Result {
public:
    Result() = delete;
    Result(const Result&) = delete;
    Result& operator=(const Result&) = delete;
    Result(Result&&) = default;
    Result& operator=(Result&&) = default;
    Result(const Data& data);
    Result(const Data& data, const Error& type);
    Result(const Error& type);
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