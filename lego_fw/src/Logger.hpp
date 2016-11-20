#ifndef LOGGER_HPP
#define LOGGER_HPP

#include <chrono>
#include <ctime>
#include <iomanip>
#include <memory>
#include <mutex>
#include <iostream>
#include <string>
#include <thread>

#define ERROR Logger().get(Logger::Level::kError)
#define WARNING Logger().get(Logger::Level::kWarning)
#define INFO Logger().get(Logger::Level::kInfo)
#define DEBUG Logger().get(Logger::Level::kDebug)

class Logger {
   public:
    typedef typename std::stringstream Stream;

    enum class Level : char{
        kError,
        kWarning,
        kInfo,
        kDebug,
        kLastReserved
    };

    Logger() = default;

    Stream& get(const Level& level) {
        std::time_t time =
            std::chrono::system_clock::to_time_t(std::chrono::system_clock::now());
        buffer_ << "[" << std::put_time(std::localtime(&time), "%Z %T") << "][" <<
            std::this_thread::get_id() << "][" << levelToString(level) << "] ";
        return buffer_;
    }

    ~Logger() {
        buffer_ << std::endl;
        static std::mutex cerr_mtx_;
        std::lock_guard<std::mutex> lock_(cerr_mtx_);
        std::cerr << buffer_.rdbuf();
    }

   private:
    Logger(const Logger&) = delete;
    Logger& operator=(const Logger&) = delete;

    static const char* levelToString(const Level& level) {
        static const char *const strings[static_cast<int>(Level::kLastReserved)+1] =
        {"error  ",
         "warning",
         "info   ",
         "debug  ", ""};
        return strings[static_cast<int>(level)];
    }

    Stream buffer_;
};

#endif /* end of include guard: LOGGER_HPP */
