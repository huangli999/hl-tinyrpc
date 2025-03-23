#ifndef HL_NET_TCP_TCP_BUFFER_H
#define HL_NET_TCP_TCP_BUFFER_H
#include<vector>
namespace hl{

class TcpBuffer{
public:

TcpBuffer(int size);

~TcpBuffer();

int readAble();//返回可读字节数

int writeAble();//返回可写字节数

int readIndex();

int writeIndex();

void writeToBuffer(const char*buf,int size);

void readFromBuffer(std::vector<char>&re,int size);

void resizeBuffer(int new_size);

void adjustBuffer();

void moveReadIndex(int size);

void moveWriteIndex(int size);

private:
int m_read_index;
int m_write_index;
int m_size{0};

std::vector<char> m_buffer;

};


}



#endif