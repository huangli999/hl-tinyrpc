#ifndef HL_COMMON_RUN_TIME_H
#define HL_COMMON_RUN_TIME_H
#include<string>
namespace hl
{
    class RunTime{

        public:



        public:
        static RunTime*GetRunTime();


        public:
        std::string m_msg_id;
        std::string m_method_name;
    };
} // namespace hl




#endif