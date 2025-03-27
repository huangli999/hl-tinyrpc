#include"/home/hl/hl-tinyrpc/hl/common/log.h"
#include"/home/hl/hl-tinyrpc/hl/common/util.h"
#include<sys/time.h>
#include<sstream>
#include<stdio.h>
#include"/home/hl/hl-tinyrpc/hl/net/eventloop.h"
#include"/home/hl/hl-tinyrpc/hl/common/runtime.h"
#include"/home/hl/hl-tinyrpc/hl/common/config.h"




namespace hl{
static Logger*g_logger=nullptr;

Logger::Logger(LogLevel level,int type):m_set_level(level),m_type(type){
    if(m_type==0){
        return;
    }
    m_asnyc_log=std::make_shared<AsyncLogger>(
        Config::GetGlobalConfig()->m_log_file_name+"_rpc",
        Config::GetGlobalConfig()->m_log_file_path,
        Config::GetGlobalConfig()->m_log_max_size);

    m_asnyc_app_log=std::make_shared<AsyncLogger>(
        Config::GetGlobalConfig()->m_log_file_name+"_app",
        Config::GetGlobalConfig()->m_log_file_path,
        Config::GetGlobalConfig()->m_log_max_size);

}
void Logger::init(){
    if(m_type==0){
        return;
    }
    m_timer_event=std::make_shared<TimerEvent>(Config::GetGlobalConfig()->m_log_sync_interval,
    true,std::bind(&Logger::syncLoop,this));

EventLoop::GetCurrentEventLoop()->addTimeEvent(m_timer_event);
}

Logger*Logger::GetGlobalLogger(){
    return g_logger;
}

void Logger::InitGlobalLogger(int type){

    LogLevel global_log_level=StringToLogLevel(Config::GetGlobalConfig()->m_log_level);
    printf("init log level [%s]\n",LogLevelToString(global_log_level).c_str());
    g_logger=new Logger(global_log_level,type);//使用配置文件讀取的日志等級進行初始化
    g_logger->init();
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
        <<"["<<m_pid<<":"<<m_thread_id<<"]\t";

        std::string msg_id=RunTime::GetRunTime()->m_msg_id;
        std::string method_name=RunTime::GetRunTime()->m_method_name;
        if(!msg_id.empty()){
            ss<<"["<<msg_id<<"]\t";
        }
        if(!method_name.empty()){
            ss<<"["<<method_name<<"]\t";
        }
        return ss.str();

    }
    void Logger::pushLog(const std::string&msg)
    {
        //如果时同步日志直接打印
        if(m_type==0){
            printf((msg+"\n").c_str());
            return;
        }
        ScopeMutex<Mutex>lock(m_mutex);
        m_buffer.push_back(msg);
        lock.unlock();

    }

    void Logger::pushAppLog(const std::string &msg){
        ScopeMutex<Mutex>lock(m_app_mutex);
        m_app_buffer.push_back(msg);
        lock.unlock();
    }
    
    void Logger::log()
    {
        // ScopeMutex<Mutex> lock(m_mutex);
        // std::queue<std::string>tmp;
        // m_buffer.swap(tmp);
        // lock.unlock();
        // while(!tmp.empty())
        // {
        //     std::string msg=tmp.front();
        //     tmp.pop();
        //     printf("%s\n",msg.c_str());
        // }
    }

    AsyncLogger::AsyncLogger(const std::string&file_name,const std::string&file_path,int max_size)
    :m_file_name(file_name),m_file_path(file_path),m_max_file_size(max_size){
        sem_init(&m_sempahore,0,0);

        pthread_create(&m_thread,NULL,&AsyncLogger::loop,this);

        pthread_cond_init(&m_condition,NULL);

        sem_wait(&m_sempahore);



    }

    /// @brief 将buffer中的文件全部打印到文件中，然后线程睡眠，直到有新的文件
    void* AsyncLogger::loop(void*arg){

        AsyncLogger*logger=reinterpret_cast<AsyncLogger*>(arg);

        sem_post(&logger->m_sempahore);

        while(1){
            ScopeMutex<Mutex>lock(logger->m_mutex);
            while(logger->m_buffer.empty()){
                pthread_cond_wait(&(logger->m_condition),logger->m_mutex.getMutex());
            }

            std::vector<std::string>tmp;
            tmp.swap(logger->m_buffer.front());
            logger->m_buffer.pop();

            lock.unlock();

            timeval now;
            gettimeofday(&now,NULL);

            struct tm now_time;
            localtime_r(&(now.tv_sec),&now_time);

            const char*format="%Y%m%d";
            char date[32];
            strftime(date,sizeof(date),format,&now_time);

            if(std::string(date)!=logger->m_date){
                logger->m_reopen_flag=true;
                logger->m_no=0;
                logger->m_date=std::string(date);
            }

            if(logger->m_file_handler==NULL){
                logger->m_reopen_flag=true;
            }

            std::stringstream ss;
            ss<<logger->m_file_path<<logger->m_file_path<<"_"
            <<std::string(date)<<"_log.";
            std::string log_file_name=ss.str()+std::to_string(logger->m_no);
            if(logger->m_reopen_flag){
                if(logger->m_file_handler){
                    fclose(logger->m_file_handler);
                }
                logger->m_file_handler=fopen(log_file_name.c_str(),"a");
                logger->m_reopen_flag=false;
            }
            //如果文件太大，打开新的文件
            if(ftell(logger->m_file_handler)>logger->m_max_file_size){
                fclose(logger->m_file_handler);

                logger->m_no++;
                log_file_name=ss.str()+std::to_string(logger->m_no);
                logger->m_file_handler=fopen(log_file_name.c_str(),"a");
                logger->m_reopen_flag=false;
            }

            for(auto&i:tmp){
                if(!i.empty()){
                    fwrite(i.c_str(),1,i.length(),logger->m_file_handler);
                }
            }

            fflush(logger->m_file_handler);//刷盘
            if(logger->m_stop_flag){
                return NULL;
            }
        }

        return NULL;
    }

    void AsyncLogger::stop(){
        m_stop_flag=true;
    }

    void AsyncLogger::flush(){
        if(m_file_handler){
            fflush(m_file_handler);
        }
    }
    /// @brief 同步m_buffer到async——logger的buffer队尾
    void Logger::syncLoop(){
        std::vector<std::string>tmp_vec;
        ScopeMutex<Mutex>lock(m_mutex);
       m_buffer.swap(tmp_vec);
       lock.unlock();

       if(!tmp_vec.empty()){
       m_asnyc_log->pushLogBuffer(tmp_vec);

       }
       tmp_vec.clear();

       std::vector<std::string>tmp_app_vec;
       ScopeMutex<Mutex>applock(m_app_mutex);
        m_app_buffer.swap(tmp_app_vec);
      applock.unlock();

      if(!tmp_app_vec.empty()){
      m_asnyc_app_log->pushLogBuffer(tmp_app_vec);
      }
      tmp_app_vec.clear();


    }

    void AsyncLogger::pushLogBuffer(std::vector<std::string>&vec){
        ScopeMutex<Mutex>lock(m_mutex);
        m_buffer.push(vec);
        lock.unlock();

    }

}