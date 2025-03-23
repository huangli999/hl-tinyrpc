#ifndef HL_NET_IO_THREAD_GROUP_H
#define HL_NET_IO_THREAD_GROUP_H
#include<vector>
#include"/home/hl/hl-tinyrpc/hl/net/io_thread.h"
#include"/home/hl/hl-tinyrpc/hl/common/log.h"
#include"/home/hl/hl-tinyrpc/hl/common/util.h"
namespace hl
{
class IOThreadGroup{
public:

IOThreadGroup(int size);

~IOThreadGroup();

void start();

void join();

IOThread*getIOThread();

private:
    int m_size{0};
    std::vector<IOThread*>m_io_thread_groups;

    int m_index{0};//当前io线程的下标
};    
} // namespace hl




#endif