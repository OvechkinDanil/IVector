#include "../include/ILogger.h"
#include "LoggerImpl.cpp"

ILogger *ILogger::createLogger() {
    return new LoggerImpl();
}

ILogger *ILogger::createLogger(const char *const &filename, bool overwrite) {
    return new LoggerImpl(filename, false);
}

ILogger::~ILogger() = default;