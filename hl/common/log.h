#ifndef HL_COMMON_LOG_H
#define HL_COMMON_LOG_H


#include <string>
#include <queue>
#include <memory>
#include <semaphore.h>
#include<semaphore.h>
#include"/home/hl/hl-tinyrpc/hl/common/mutex.h"
#include"/home/hl/hl-tinyrpc/hl/net/timer_event.h"
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
    }\

#define INFOLOG(str,...) \
if(hl::Logger::GetGlobalLogger()->getLogLevel()<=hl::Info)\
{\
hl::Logger::GetGlobalLogger()->pushLog((hl::LogEvent(hl::LogLevel::Info)).toString()\
+"["+std::string(__FILE__)+":"+std::to_string(__LINE__)+"]\t"+hl::formatString(str,##__VA_ARGS__));\
}\

#define ERRORLOG(str,...) \
if(hl::Logger::GetGlobalLogger()->getLogLevel()<=hl::Error)\
{\
hl::Logger::GetGlobalLogger()->pushLog((hl::LogEvent(hl::LogLevel::Error)).toString()\
+"["+std::string(__FILE__)+":"+std::to_string(__LINE__)+"]\t"+hl::formatString(str,##__VA_ARGS__));\
}\

#define APPDEBUGLOG(str,...) \
    if(hl::Logger::GetGlobalLogger()->getLogLevel()<=hl::Debug)\
    {\
    hl::Logger::GetGlobalLogger()->pushAppLog((hl::LogEvent(hl::LogLevel::Debug)).toString()+"["\
    +std::string(__FILE__)+":"+std::to_string(__LINE__)+"]\t"+hl::formatString(str,##__VA_ARGS__));\
    }\

#define APPINFOLOG(str,...) \
if(hl::Logger::GetGlobalLogger()->getLogLevel()<=hl::Info)\
{\
hl::Logger::GetGlobalLogger()->pushAppLog((hl::LogEvent(hl::LogLevel::Info)).toString()\
+"["+std::string(__FILE__)+":"+std::to_string(__LINE__)+"]\t"+hl::formatString(str,##__VA_ARGS__));\
}\

#define APPERRORLOG(str,...) \
if(hl::Logger::GetGlobalLogger()->getLogLevel()<=hl::Error)\
{\
hl::Logger::GetGlobalLogger()->pushAppLog((hl::LogEvent(hl::LogLevel::Error)).toString()\
+"["+std::string(__FILE__)+":"+std::to_string(__LINE__)+"]\t"+hl::formatString(str,##__VA_ARGS__));\
}\


enum LogLevel {
    Unknown = 0,
    Debug = 1,
    Info = 2,
    Error = 3
};
    

/// @brief 异步日志
///定时的将同步日志的文件交换到异步日志器
///专门的异步日志线程打印
class AsyncLogger{
    public:
    static void* loop(void*);
    
    public:
    
    typedef std::shared_ptr<AsyncLogger>s_ptr;
    
    AsyncLogger(const std::string&file_name,const std::string&file_path,int max_size);
    
    void stop();
    
    void flush();
    
    void pushLogBuffer(std::vector<std::string>&vec);
    public:

    pthread_t m_thread;

    private:
    
    std::queue<std::vector<std::string>>m_buffer;//
    
    std::string m_file_name;//输出文件名
    
    std::string m_file_path;//输出文件路径
    
    int m_max_file_size{0};//输出文件最大
    
    sem_t m_sempahore;//信号量
    
    pthread_cond_t m_condition;//条件变量
    
    Mutex m_mutex;

    std::string m_date;//当前打印日志的文件日期

    FILE*m_file_handler{NULL};//当前打的日志文件句柄
    
    bool m_reopen_flag{false};//判断是否需要重新打开文件
    
    int m_no{0};//日志文件序号
    
    bool m_stop_flag{false};
    };
        





/// @brief 日志器
class Logger {
public:

    typedef std::shared_ptr<Logger>s_ptr;//智能指针管理内存

    Logger(LogLevel level,int type=1);

    void pushLog(const std::string &msg);//放入队列中

    void pushAppLog(const std::string &msg);//放入队列中

    static Logger*GetGlobalLogger();//全局访问接口，对应单例模式

    static void InitGlobalLogger(int type=1);

    void log();//打印日志

    void init();

    LogLevel getLogLevel()const{
        return m_set_level;
    }
    
    void syncLoop();
    

private:
    LogLevel m_set_level;

    std::vector<std::string>m_buffer;//阻塞队列

    std::vector<std::string>m_app_buffer;//

    Mutex m_mutex;

    Mutex m_app_mutex;

    AsyncLogger::s_ptr m_asnyc_log;

    AsyncLogger::s_ptr m_asnyc_app_log;

    TimerEvent::s_ptr m_timer_event;

    int m_type{0};
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