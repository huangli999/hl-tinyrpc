#ifndef HL_COMMON_CONFIG_H
#define HL_COMMON_CONFIG_H

#include<map>

namespace hl{

class Config{
public:
    Config(const char*xmlfile);
    Config();
public:
    static Config*GetGlobalConfig();

    static void SetGlobalConfig(const char*xmlfile);

    std::string m_log_level;
    std::string m_log_file_name;
    std::string m_log_file_path;
    int m_log_max_size{0};
    int m_log_sync_interval{0};//日志同步间隔

    int m_port{0};

    int m_io_threads{0};

};

}

#endif