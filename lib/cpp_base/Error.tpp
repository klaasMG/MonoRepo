#pragma once
#include "Error.h"
#include <iostream>

std::string inline ErrorType_to_string(BaseErrorType type) {
    switch (type) {
        case BaseErrorType::OK: {
            return "OK error";
        }
        case BaseErrorType::NOT_IMPLEMENTED: {
            return "Not implemented";
        }
        case BaseErrorType::FILE_NOT_FOUND: {
            return "File not found";
        }
        case BaseErrorType::FILE_IN_USE: {
            return "File in use";
        }
        case BaseErrorType::FILE_DATA_ERROR: {
            return "File data error";
        }
        case BaseErrorType::QUEUE_EMPTY: {
            return "Queue is empty";
        }
        case BaseErrorType::LOGGER_ERROR: {
            return "";
        }
    }
    std::cerr << "This can not happen add all other types and this will no longer fail" << std::endl;
    std::terminate();
};

template <typename Data, IsErrorEnum Error>
Result<Data, Error>::Result(Data data) : data(std::move(data)), type(Error{}) {}

template <typename Data, IsErrorEnum Error>
Result<Data, Error>::Result(Data data, Error type) : data(std::move(data)), type(std::move(type)) {
    if (this->type == Error{}) {
        std::cerr << "a error can not be ok" << std::endl;
        std::terminate();
    }
}

template <typename Data, IsErrorEnum Error>
Result<Data, Error>::~Result() {
    if (!is_error_handeled) {
        std::terminate();
    }
}

template <typename Data, IsErrorEnum Error>
Result<Data, Error>::Result(Error type) : data{}, type(std::move(type)) {
    if (this->type == Error{}) {
        std::cerr << "a error can not be ok" << std::endl;
        std::terminate();
    }
}

template <typename Data, IsErrorEnum Error>
Error Result<Data, Error>::check_error() {
    is_error_checked = true;
    return type;
}

template <typename Data, IsErrorEnum Error>
Data Result<Data, Error>::GetData() const {
    if (!is_error_checked) {
        std::cerr << "check error first" << std::endl;
        std::terminate();
    }
    return data;
}

template <typename Data, IsErrorEnum Error>
Data Result<Data, Error>::Handle_Error() {
    if (!is_error_checked) {
        std::terminate();
    }
    is_error_handeled = true;
    type = Error{};
    return data;
}
