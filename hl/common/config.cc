#include"/home/hl/hl-tinyrpc/hl/common/config.h"
#include<tinyxml/tinyxml.h>

#define READ_XML_NODE(name,parent) \
TiXmlElement*name##_node=parent->FirstChildElement(#name);\
if(!name##_node){\
    printf("start hl server error,failed to read node [%s]\n",#name);\
    exit(0);\
}\

#define READ_STR_FROM_XML_NODE(name,parent) \
TiXmlElement*name##_node=parent->FirstChildElement(#name);\
if(!name##_node||!name##_node->GetText()){\
    printf("Start hl server error,failed to read config file %s\n",#name);\
}\
std::string name##_str=std::string(name##_node->GetText());\

namespace hl{

static Config*g_config=NULL;



Config*Config::GetGlobalConfig(){

    return g_config;

}

void Config::SetGlobalConfig(const char*xmlfile){
    if(g_config==NULL)
    {
        if(xmlfile!=NULL){
            g_config=new Config(xmlfile);
        }else
        {
            g_config=new Config();
        }
       
    }
}

Config::Config(){
    m_log_level="DEBUG";
    
}

Config::Config(const char*xmlfile){
    TiXmlDocument*xml_document=new TiXmlDocument();
    bool rt=xml_document->LoadFile(xmlfile);
    if(!rt)
    {
        printf("Start hl server error,failed to read congif file %s,error info[%s]\n",xmlfile,xml_document->ErrorDesc());
        exit(0);

    }
    READ_XML_NODE(root,xml_document);
    READ_XML_NODE(log,root_node);
    READ_XML_NODE(server,root_node);

    READ_STR_FROM_XML_NODE(log_level,log_node);
    READ_STR_FROM_XML_NODE(log_file_name,log_node);
    READ_STR_FROM_XML_NODE(log_file_path,log_node);
    READ_STR_FROM_XML_NODE(log_max_size,log_node);
    READ_STR_FROM_XML_NODE(log_sync_interval,log_node);

    m_log_level=log_level_str;
    m_log_file_name=log_file_name_str;
    m_log_file_path=log_file_path_str;
    m_log_max_size=std::atoi(log_max_size_str.c_str());
    m_log_sync_interval=std::atoi(log_sync_interval_str.c_str());

    READ_STR_FROM_XML_NODE(port,server_node);
    READ_STR_FROM_XML_NODE(io_threads,server_node);
    m_port=std::atoi(port_str.c_str());
    m_io_threads=std::atoi(io_threads_str.c_str());


    printf(" LOG__CONFIG LEVEL[%s],FILENAME[%s]FILEPATH[%s],MAX_FILE_SIZE[%d],SYNC_INTERVAL[%d ms]\n",
       m_log_level.c_str(),m_log_file_name.c_str(),m_log_file_path.c_str(),m_log_max_size,m_log_sync_interval
    );

    printf("Server--port[%d],IO threads[%d]",m_port,m_io_threads);

}


}