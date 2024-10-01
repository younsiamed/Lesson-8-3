#include <iostream>
#include <fstream>
#include <stdexcept>
#include <string>

enum class Type {
    Warning,
    Error,
    FatalError,
    Unknown
};

class LogMessage {
public:
    LogMessage(Type t, const std::string& msg) : type_(t), message_(msg) {}

    Type type() const { return type_; }
    const std::string& message() const { return message_; }

private:
    Type type_;
    std::string message_;
};

class LogHandler {
protected:
    LogHandler* nextHandler_;

public:
    LogHandler() : nextHandler_(nullptr) {}

    virtual ~LogHandler() {}

    void setNext(LogHandler* handler) {
        nextHandler_ = handler;
    }

    virtual void handle(const LogMessage& logMessage) {
        if (nextHandler_) {
            nextHandler_->handle(logMessage);
        }
        else {
            throw std::runtime_error("Unhandled log message");
        }
    }
};

class FatalErrorHandler : public LogHandler {
public:
    void handle(const LogMessage& logMessage) override {
        if (logMessage.type() == Type::FatalError) {
            throw std::runtime_error("Fatal error: " + logMessage.message());
        }
        else {
            LogHandler::handle(logMessage);
        }
    }
};

class ErrorHandler : public LogHandler {
private:
    std::string logFilePath_;

public:
    ErrorHandler(const std::string& filePath) : logFilePath_(filePath) {}

    void handle(const LogMessage& logMessage) override {
        if (logMessage.type() == Type::Error) {
            std::ofstream outFile(logFilePath_, std::ios::app);
            if (!outFile.is_open()) {
                throw std::runtime_error("Unable to open log file");
            }
            outFile << "Error: " << logMessage.message() << std::endl;
        }
        else {
            LogHandler::handle(logMessage);
        }
    }
};

class WarningHandler : public LogHandler {
public:
    void handle(const LogMessage& logMessage) override {
        if (logMessage.type() == Type::Warning) {
            std::cout << "Warning: " << logMessage.message() << std::endl;
        }
        else {
            LogHandler::handle(logMessage);
        }
    }
};

class UnknownHandler : public LogHandler {
public:
    void handle(const LogMessage& logMessage) override {
        if (logMessage.type() == Type::Unknown) {
            throw std::runtime_error("Unknown log message: " + logMessage.message());
        }
        else {
            LogHandler::handle(logMessage);
        }
    }
};

int main() {
    FatalErrorHandler fatalHandler;
    ErrorHandler errorHandler("log.txt");
    WarningHandler warningHandler;
    UnknownHandler unknownHandler;

    warningHandler.setNext(&errorHandler);
    errorHandler.setNext(&fatalHandler);
    fatalHandler.setNext(&unknownHandler);

    LogMessage warning(Type::Warning, "Low disk space.");
    LogMessage error(Type::Error, "Failed to open file.");
    LogMessage fatal(Type::FatalError, "Memory corruption detected.");
    LogMessage unknown(Type::Unknown, "Unrecognized format.");

    try {
        warningHandler.handle(warning);
        warningHandler.handle(error);
        warningHandler.handle(fatal);
    }
    catch (const std::exception& e) {
        std::cerr << "Exception caught: " << e.what() << std::endl;
    }

    try {
        warningHandler.handle(unknown);
    }
    catch (const std::exception& e) {
        std::cerr << "Exception caught: " << e.what() << std::endl;
    }

    return 0;
}
