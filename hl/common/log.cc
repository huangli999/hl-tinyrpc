#include"/home/hl/hl-tinyrpc/hl/common/log.h"
#include"/home/hl/hl-tinyrpc/hl/common/util.h"
#include<sys/time.h>
#include<sstream>
#include<stdio.h>

#include"/home/hl/hl-tinyrpc/hl/common/config.h"




namespace hl{
static Logger*g_logger=nullptr;

Logger*Logger::GetGlobalLogger(){
    return g_logger;
}

void Logger::InitGlobalLogger(){

    LogLevel global_log_level=StringToLogLevel(Config::GetGlobalConfig()->m_log_level);
    printf("init log level [%s]\n",LogLevelToString(global_log_level).c_str());
    g_logger=new Logger(global_log_level);//使用配置文件讀取的日志等級進行初始化
}
    


std::string LogLevelToString(LogLevel level){
    switch(level){
    case Debug:
        return "DEBUG";

    case Info:
        return "INFO";
    case Error:
        return "ERROR";
    default:
        return "UNKNOWN";
    }

}
    LogLevel StringToLogLevel(const std::string&log_level){
        if(log_level=="DEBUG"){
            return Debug;
        }else if(log_level=="INFO")
        {
            return Info;
        }else if(log_level=="ERROR")
        {
            return Error;
        }
        else{
            return Unknown;
        }
    }

    /// @brief 打印日志[Level][%y-%m-%d %H:%M:%s.%ms]\t[pid:thread_id]\t[file_name:line][%msg]
    /// @return 
    std::string LogEvent::toString()
    {
        struct timeval now_time;

        gettimeofday(&now_time, nullptr);
      
        struct tm now_time_t;
        localtime_r(&(now_time.tv_sec), &now_time_t);
      
        char buf[128];
        strftime(&buf[0], 128, "%y-%m-%d %H:%M:%S", &now_time_t);
        std::string time_str(buf);
        int ms = now_time.tv_usec / 1000;
        time_str = time_str + "." + std::to_string(ms);
      
        m_pid=getPid();
        m_thread_id=getThreadId();
      
        std::stringstream ss;
        ss<<"["<<LogLevelToString(m_level)<<"]\t"
        <<"["<<time_str<<"]\t"
        <<"["<<m_pid<<":"<<m_thread_id<<"]\t"
        <<"["<<std::string(__FILE__)<<":"<<__LINE__<<"]\t";

        return ss.str();

    }
    void Logger::pushLog(const std::string&msg)
    {
        m_buffer.push(msg);

    }
    
    void Logger::log()
    {
        while(!m_buffer.empty())
        {
            std::string msg=m_buffer.front();
            m_buffer.pop();
            printf("%s\n",msg.c_str());
        }
    }



}