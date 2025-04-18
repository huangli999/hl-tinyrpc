#include"/home/hl/hl-tinyrpc/hl/net/tcp/tcp_connection.h"
#include"/home/hl/hl-tinyrpc/hl/net/fd_event_group.h"
#include"/home/hl/hl-tinyrpc/hl/common/log.h"
#include<string.h>
#include<unistd.h>
#include"/home/hl/hl-tinyrpc/hl/net/coder/abstract_coder.h"
#include"/home/hl/hl-tinyrpc/hl/net/coder/string_coder.h"
#include"/home/hl/hl-tinyrpc/hl/net/coder/tinypb_coder.h"
#include"/home/hl/hl-tinyrpc/hl/net/rpc/rpc_dispatcher.h"
namespace hl{

    /// @brief 
    /// @param io_thread 持有当前连接的IO线程
    /// @param fd 
    /// @param buffer_size 
    /// @param peer_addr 
    TcpConnection::TcpConnection(EventLoop*eventloop,int fd,int buffer_size,NetAddr::s_ptr peer_addr,NetAddr::s_ptr local_addr,TcpConnectionType type)
    :m_event_loop(eventloop),m_peer_addr(peer_addr),m_local_addr(local_addr),m_state(NotConnected),m_fd(fd),m_connection_type(type){
       
        m_in_buffer=std::make_shared<TcpBuffer>(buffer_size);
        m_out_buffer=std::make_shared<TcpBuffer>(buffer_size);

        m_fd_event=FdEventGroup::GetFdEventGroup()->getFdEvent(fd);
        m_fd_event->setNonBlock();

        if(m_connection_type==TcpConnectionByServer){
            listenRead();
     
        }
       

        m_coder=new TinyPBCoder();

    }

    TcpConnection::~TcpConnection(){
        if(m_coder){
            delete m_coder;
            m_coder=NULL;
        }
    }

    void TcpConnection::setState(const TcpState state){
     m_state=Connected;
    }

    TcpState TcpConnection::getState(){
     return m_state;
    }


    void TcpConnection::onRead(){
        if(m_state!=Connected){
            ERRORLOG("client has already disconnected ,clientfd%d,addr[%s]",m_fd,m_peer_addr->toString().c_str());
            return;
        }
        bool is_read_all=false;
        bool is_close=false;
        while(!is_read_all){
            if(m_in_buffer->writeAble()==0){
                m_in_buffer->resizeBuffer(2*m_in_buffer->m_buffer.size());
            }
            int read_count=m_in_buffer->writeAble();
            int write_index=m_in_buffer->writeIndex();

            int rt=read(m_fd,&(m_in_buffer->m_buffer[write_index]),read_count);
            DEBUGLOG("success read%dbytes from addr[%s],client fd[%d]",rt,m_peer_addr->toString().c_str(),m_fd);
            if(rt>0){
                m_in_buffer->moveWriteIndex(rt);
                if(rt==read_count){
                    continue;
                }else if(rt<read_count){
                    is_read_all=true;
                    break;
                }
            }else if(rt==0){
                is_close=true;
                DEBUGLOG("succ is_cose",NULL);
                break;
            }else if(rt==-1&&errno == EAGAIN){
                is_read_all=true;
                break;
            }
        }
        if(is_close){
            clear();
            DEBUGLOG("peer closed,peer addr [%s],clientfd[%d]",m_peer_addr->toString().c_str(),m_fd);
            return;
        }
        if(!is_read_all){
            ERRORLOG("not read all data",NULL);
        }

        //解析
        excute();

    }


    void TcpConnection::clear(){
        //代表服务处理关闭连接后的清理动作
        if(m_state==Closed){
            return;
        }
        m_fd_event->cancle(FdEvent::IN_EVENT);
        m_fd_event->cancle(FdEvent::OUT_EVENT);

        m_event_loop->deleteEpollEvent(m_fd_event);

        m_state=Closed;//设置状态为关闭
    }

    /// @brief 服务端主动关闭连接
    void TcpConnection::shutDown(){
        if(m_state==Closed||m_state==NotConnected){
            return;
        }
        //处于半关闭状态
        m_state=HalfClosing;

        //用shutdown关闭读写，意味着服务器不会再对这个fd进行读写操作
        //发送FIN报文，触发四次挥手的第一个阶段
        //当fd发送可读事件，但可读数据为0，即对端发送了FIN报文
        ::shutdown(m_fd,SHUT_RDWR);

    }

    /// @brief 
    void TcpConnection::excute(){
        if(m_connection_type==TcpConnectionByServer){
        //将RPC请求执行业务逻辑，获取RPC响应,再把RPC响应发送回去
        // std::vector<char>tmp;
        // int size=m_in_buffer->readAble();
        // tmp.reserve(size);
        // m_in_buffer->readFromBuffer(tmp,size);
        std::vector<AbstractProtocol::s_ptr>result;//请求事件
        std::vector<AbstractProtocol::s_ptr>replay_result;//响应回包

        m_coder->decode(result,m_in_buffer);
        for(size_t i=0;i<result.size();++i){
            //针对每一个请求，调用roc方法，获取响应message
            //2、将响应message放入缓冲区，监听可写事件回包
            INFOLOG("sucess get request from client[%s]",m_peer_addr->toString().c_str());
            std::shared_ptr<TinyPBProtocol>message=std::make_shared<TinyPBProtocol>();
            // message->m_pb_data="hello";
            // message->m_req_id=result[i]->m_req_id;
            RpcDispatcher::GetRpcDispatcher()->dispatch(result[i],message,this);
            replay_result.emplace_back(message);
        }

        m_coder->encode(replay_result,m_out_buffer);
        listenWrite();
        }else{

            std::vector<AbstractProtocol::s_ptr>result;
            m_coder->decode(result,m_in_buffer);

            for(size_t i=0;i<result.size();++i){
                std::string req_id=result[i]->m_msg_id;
                auto it=m_read_dones.find(req_id);
                if(it!=m_read_dones.end()){
                    it->second(result[i]);
                    m_read_dones.erase(it);
                }
            }

        }




    }


    /// @brief 
    void TcpConnection::onWrite(){
        if(m_state!=Connected){
            ERRORLOG("client has already disconnected ,clientfd%d,addr[%s]",m_fd,m_peer_addr->toString().c_str());
            return;
        }
        if(m_connection_type==TcpConnectionByClient){
            //1，将message对象encode得到字节流
            //将数据写入buffer,然后全部发送
            std::vector<AbstractProtocol::s_ptr>message;

            for(size_t i=0;i<m_write_dones.size();++i){
                message.push_back(m_write_dones[i].first);
            }
            m_coder->encode(message,m_out_buffer);
        }
        bool is_all_write=false;
        while(true){
            if(m_out_buffer->readAble()==0){
                DEBUGLOG("no data need to send to client [%s]",m_peer_addr->toString().c_str());
                is_all_write=true;
                break;
            }
            int write_size=m_out_buffer->readAble();
            int read_index=m_out_buffer->readIndex();

            int rt=write(m_fd,&(m_out_buffer->m_buffer[read_index]),write_size);

            if(rt>=write_size){
                DEBUGLOG("no data need to send to client [%s]",m_peer_addr->toString().c_str());
                is_all_write=true;
                break;
            }
            if(rt==-1&&errno == EAGAIN){
                //发送缓冲区已满
                ERRORLOG("write data error errno==EAGAIN and rt==-1",NULL);
                break;
            }
        }
        if(is_all_write){
            m_fd_event->cancle(FdEvent::OUT_EVENT);
            m_event_loop->addEpollEvent(m_fd_event);
        }

        if(m_connection_type==TcpConnectionByClient){
            for(size_t i=0;i<m_write_dones.size();++i){
                m_write_dones[i].second(m_write_dones[i].first);
            }
            m_write_dones.clear();
        }
        
    }


    void TcpConnection::setConnectionType(TcpConnectionType type){
        m_connection_type=type;
    }

    void TcpConnection::listenWrite(){
        
        m_fd_event->listen(FdEvent::OUT_EVENT,std::bind(&TcpConnection::onWrite,this));

        m_event_loop->addEpollEvent(m_fd_event);
    }

    void TcpConnection::listenRead(){
        m_fd_event->listen(FdEvent::IN_EVENT,std::bind(&TcpConnection::onRead,this));

        m_event_loop->addEpollEvent(m_fd_event);
    }

    void TcpConnection::pushSendMessage(AbstractProtocol::s_ptr message,std::function<void(AbstractProtocol::s_ptr)>done){
        m_write_dones.push_back(std::make_pair(message,done));
    }

    void TcpConnection::pushReadMessage(const std::string&req_id,std::function<void(AbstractProtocol::s_ptr)> done){
        m_read_dones.insert(std::make_pair(req_id,done));
    }

    NetAddr::s_ptr TcpConnection::getLocalAddr(){
        return m_local_addr;
    }

    NetAddr::s_ptr TcpConnection::getPeerAddr(){
        return m_peer_addr;
    }
}