#ifndef HL_COMMON_CONFIG_H
#define HL_COMMON_CONFIG_H

#include<map>

namespace hl{

class Config{
public:
    Config(const char*xmlfile);

public:
    static Config*GetGlobalConfig();

    static void SetGlobalConfig(const char*xmlfile);

    std::string m_log_level;

};

}

#endif