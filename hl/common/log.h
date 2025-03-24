#ifndef HL_COMMON_LOG_H
#define HL_COMMON_LOG_H


#include <string>
#include <queue>
#include <memory>
#include <semaphore.h>

#include"/home/hl/hl-tinyrpc/hl/common/mutex.h"

namespace hl{

/// @brief 格式化字符串
/// @tparam ...Args 
/// @param str 
/// @param ...args 
/// @return 
template<typename... Args>
std::string formatString(const char* str, Args&&... args) {

    int size = snprintf(nullptr, 0, str, args...);

    std::string result;
    if (size > 0) {
    result.resize(size);
    snprintf(&result[0], size + 1, str, args...);
    }

    return result;
}
    
#define DEBUGLOG(str,...) \
    if(hl::Logger::GetGlobalLogger()->getLogLevel()<=hl::Debug)\
    {\
    hl::Logger::GetGlobalLogger()->pushLog((hl::LogEvent(hl::LogLevel::Debug)).toString()+"["\
    +std::string(__FILE__)+":"+std::to_string(__LINE__)+"]\t"+hl::formatString(str,##__VA_ARGS__));\
    hl::Logger::GetGlobalLogger()->log();\
    }\

#define INFOLOG(str,...) \
if(hl::Logger::GetGlobalLogger()->getLogLevel()<=hl::Info)\
{\
hl::Logger::GetGlobalLogger()->pushLog((hl::LogEvent(hl::LogLevel::Info)).toString()\
+"["+std::string(__FILE__)+":"+std::to_string(__LINE__)+"]\t"+hl::formatString(str,##__VA_ARGS__));\
hl::Logger::GetGlobalLogger()->log();\
}\

#define ERRORLOG(str,...) \
if(hl::Logger::GetGlobalLogger()->getLogLevel()<=hl::Error)\
{\
hl::Logger::GetGlobalLogger()->pushLog((hl::LogEvent(hl::LogLevel::Error)).toString()\
+"["+std::string(__FILE__)+":"+std::to_string(__LINE__)+"]\t"+hl::formatString(str,##__VA_ARGS__));\
hl::Logger::GetGlobalLogger()->log();\
}\

enum LogLevel {
    Unknown = 0,
    Debug = 1,
    Info = 2,
    Error = 3
};
    

    

    

/// @brief 日志器
class Logger {
public:

    typedef std::shared_ptr<Logger>s_ptr;//智能指针管理内存

    Logger(LogLevel level):m_set_level(level){}

    void pushLog(const std::string &msg);//放入队列中

    static Logger*GetGlobalLogger();//全局访问接口，对应单例模式

    static void InitGlobalLogger();

    void log();//打印日志

    LogLevel getLogLevel()const{
        return m_set_level;
    }

private:
    LogLevel m_set_level;

    std::queue<std::string>m_buffer;//阻塞队列

    Mutex m_mutex;



};
    

LogLevel StringToLogLevel(const std::string&log_level);

std::string LogLevelToString(LogLevel level);

/// @brief 日志事件
class LogEvent {
public:

    LogEvent(LogLevel level):m_level(level){}

    std::string getFileName() const {
    return m_file_name;  
    }

    LogLevel getLogLevel() const {
    return m_level;
    }
    std::string toString();


    private:
    std::string m_file_name;  // 文件名
    int32_t m_file_line;  // 行号
    int32_t m_pid;  // 进程号
    int32_t m_thread_id;  // 线程号

    LogLevel m_level;     //日志级别

};

}


#endif