#include"/home/hl/hl-tinyrpc/hl/common/util.h"
#include"/home/hl/hl-tinyrpc/hl/common/log.h"
#include"/home/hl/hl-tinyrpc/hl/net/tcp/tcp_buffer.h"
#include<memory>
#include<string.h>
namespace hl
{
    TcpBuffer::TcpBuffer(int size):m_size(size){
        m_buffer.resize(size);
    }

TcpBuffer::~TcpBuffer(){

}

int TcpBuffer::readAble(){
return m_write_index-m_write_index;
}

int TcpBuffer::writeAble(){
return m_buffer.size()-m_write_index;
}

int TcpBuffer::readIndex(){
return m_read_index;
}

int TcpBuffer::writeIndex(){
return m_write_index;
}

/// @brief 写入缓冲区 先判断是否需要扩容 
/// @param buf 写入的buf
/// @param size 写入的大小
void TcpBuffer::writeToBuffer(const char*buf,int size){
if(size>writeAble()){
    int newSize=(int)(1.5*(m_write_index+size));
    resizeBuffer(newSize);
}
    memcpy(&m_buffer[m_write_index],buf,size);

}

/// @brief 
/// @param re 
/// @param size 
void TcpBuffer::readFromBuffer(std::vector<char>&re,int size){
    if(readAble()==0){
        return;
    }

    int read_size=readAble()>size?size:readAble();

    std::vector<char>tmp(read_size);

    memcpy(&tmp[0],&m_buffer[m_read_index],read_size);

    re.swap(tmp);
    m_read_index+=read_size;
    adjustBuffer();

}

/// @brief 回收已读取的字节
void TcpBuffer::adjustBuffer(){
    if(m_read_index<(int)(m_buffer.size()/3)){
        return;
    }
    std::vector<char>buffer(m_buffer.size());
    int count=readAble();

    memcpy(&buffer[0],&m_buffer[m_read_index],count);

    m_buffer.swap(buffer);
    m_read_index=0;
    m_write_index=m_read_index+count;
    buffer.clear();
}


/// @brief 扩容缓存区
/// @param new_size 
void TcpBuffer::resizeBuffer(int new_size){
    std::vector<char>tmp(new_size);
    int count=std::min(new_size,readAble());
    memcpy(&tmp[0],&m_buffer[m_read_index],count);
    m_buffer.swap(tmp);

    m_read_index=0;
    m_write_index=m_read_index+count;
}

/// @brief 
/// @param size 
void TcpBuffer::moveReadIndex(int size){
size_t j=m_read_index+size;
if(j>=m_buffer.size()){
    ERRORLOG("moveReadIndex error,invail size %d,ol_read_index%d,buffer size%d",size,m_read_index,m_buffer.size());

}
m_read_index=j;
adjustBuffer();
}

void TcpBuffer::moveWriteIndex(int size){
    size_t j=m_write_index+size;
    if(j>=m_buffer.size()){
        ERRORLOG("moveReadIndex error,invail size %d,ol_read_index%d,buffer size%d",size,m_write_index,m_buffer.size());
    
    }
    m_write_index=j;
    adjustBuffer();
    }



} // namespace hl
