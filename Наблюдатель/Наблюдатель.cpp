#include <iostream>
#include <vector>
#include <memory>
#include <string>
#include <algorithm>
#include <fstream>

class Observer {
public:
    virtual ~Observer() = default;
    virtual void onWarning(const std::string& message) {}
    virtual void onError(const std::string& message) {}
    virtual void onFatalError(const std::string& message) {}
};

class Observable {
public:
    void addObserver(Observer* observer) {
        observers.push_back(observer);
    }

    void removeObserver(Observer* observer) {
        observers.erase(std::remove(observers.begin(), observers.end(), observer), observers.end());
    }

    void warning(const std::string& message) const {
        for (const auto& observer : observers) {
            if (observer) {
                observer->onWarning(message);
            }
        }
    }

    void error(const std::string& message) const {
        for (const auto& observer : observers) {
            if (observer) {
                observer->onError(message);
            }
        }
    }

    void fatalError(const std::string& message) const {
        for (const auto& observer : observers) {
            if (observer) {
                observer->onFatalError(message);
            }
        }
    }

private:
    std::vector<Observer*> observers;
};

class WarningObserver : public Observer {
public:
    void onWarning(const std::string& message) override {
        std::cout << "Warning: " << message << std::endl;
    }
};

class ErrorObserver : public Observer {
public:
    ErrorObserver(const std::string& filePath) : filePath(filePath) {}

    void onError(const std::string& message) override {
        std::ofstream outFile(filePath, std::ios_base::app);
        if (outFile.is_open()) {
            outFile << "Error: " << message << std::endl;
        }
    }

private:
    std::string filePath;
};

class FatalErrorObserver : public Observer {
public:
    FatalErrorObserver(const std::string& filePath) : filePath(filePath) {}

    void onFatalError(const std::string& message) override {
        std::cout << "Fatal Error: " << message << std::endl;
        std::ofstream outFile(filePath, std::ios_base::app);
        if (outFile.is_open()) {
            outFile << "Fatal Error: " << message << std::endl;
        }
    }

private:
    std::string filePath;
};

int main() {
    Observable logger;
    WarningObserver warningObserver;
    ErrorObserver errorObserver("error_log.txt");
    FatalErrorObserver fatalErrorObserver("fatal_error_log.txt");

    logger.addObserver(&warningObserver);
    logger.addObserver(&errorObserver);
    logger.addObserver(&fatalErrorObserver);

    logger.warning("This is a warning message");
    logger.error("This is an error message");
    logger.fatalError("This is a fatal error message");

    return 0;
}
