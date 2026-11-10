#pragma once

#include <exception>
#include <string>

class AppException : public std::exception {
private:
    std::string errorMessage;
public:
    explicit AppException(const std::string& message) : errorMessage(message) {}

    const char* what() const noexcept override {
        return errorMessage.c_str();
    }
};

class FilePathException : public AppException {
public:
    using AppException::AppException;
    struct messages {
        static const std::string rootFileNotFound;
        static const std::string modelNotFound;
    };
};

class RecognitionException : public AppException {
public:
    using AppException::AppException;
    struct messages {
        static const std::string microphoneInitializeError;
        static const std::string recordFail;
        static const std::string modelLoadError;
        static const std::string recognizerFail;
    };
};

class AudioException : public AppException {
public:
    using AppException::AppException;
    struct messages {
        static const std::string audioInitializeError;
        static const std::string audioPlayError;
    };
};

class CommandException : public AppException {
public:
    using AppException::AppException;
    struct messages {

    };
};