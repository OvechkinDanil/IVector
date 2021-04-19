#include <fstream>
#include <iostream>
#include "map"

#include "../include/ILogger.h"
//#include "../include/RC.h"


namespace {
    class LoggerImpl : public ILogger {
    private:
        std::fstream _logFile;
        std::map<RC, const std::string> errors;
        std::map<Level, const std::string> levels;

    public:
        LoggerImpl(const std::string &fn = "logger.log", bool overwrite = true);

        RC log(RC code, Level level) override;

        RC log(RC code, Level level, const char *const &srcfile, const char *const &function, int line) override;

        ~LoggerImpl() override;
    };

}

LoggerImpl::LoggerImpl(const std::string &fn, bool overwrite) {
    levels = {{Level::INFO,    "INFO"},
              {Level::SEVERE,   "SEVERE"},
              {Level::WARNING, "WARNING"}};

    errors = {{RC::NULLPTR_ERROR,          "one of the pointers is null"},
              {RC::ALLOCATION_ERROR,       "problem with allocation"},
              {RC::MISMATCHING_DIMENSIONS, "dimensions do not match"},
              {RC::INFINITY_OVERFLOW,      "going beyond the boundaries of double"},
              {RC::INDEX_OUT_OF_BOUND,     "index out of bound"},
              {RC::FILE_NOT_FOUND,         "file not found"},
              {RC::INVALID_ARGUMENT,       "arguments are invalid "},
              {RC::NOT_NUMBER,             "it is not number"},
              {RC::UNKNOWN,                "unknown error"},
              {RC::VECTOR_NOT_FOUND,       "vector not found"},
              {RC::SUCCESS,                "done successfully"},
              {RC::MEMORY_INTERSECTION,    "found intersecting memory while copying instance"},
              {RC::IO_ERROR,               "couldn't write/read to/from file"}};

    if (!_logFile.is_open()) {
        overwrite ? _logFile.open(fn, std::ios::app) : _logFile.open(fn, std::ios::out);

        if (!_logFile.is_open()) {
            std::cerr << "could not open log file";
        }

    }

}

RC LoggerImpl::log(RC code, ILogger::Level level) {

    return RC::INDEX_OUT_OF_BOUND;
}


RC LoggerImpl::log(RC code, Level level, const char *const &srcfile, const char *const &function, int line) {
    if (!_logFile.is_open()) {
        std::cerr << "The log file is not open";
        return RC::IO_ERROR;
    }

    std::string fullMsg = errors[code];
    std::string levelMsg = levels[level];

    _logFile << levelMsg << ": " << srcfile << " function : " << function << "| line: " << line << " | " << fullMsg
             << std::endl;

    return RC::SUCCESS;
}

LoggerImpl::~LoggerImpl() {
    if (_logFile.is_open()) {
        _logFile.flush();
        _logFile.close();
    }
}





