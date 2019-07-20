#include "simple_tcp/simple_tcp.h"

#include <unistd.h>
#include <sys/types.h>       /* basic system data types */
#include <sys/socket.h>      /* basic socket definitions */
#include <netinet/in.h>      /* sockaddr_in{} and other Internet defns */
#include <arpa/inet.h>       /* inet(3) functions */
#include <netdb.h>           /* gethostbyname function */
#include <stdlib.h>
#include <string.h>
#include <thread>
#include <mutex>

namespace top {
namespace network {

void
SimpleTCP::CreateContext(uint64_t role_type) {
    if(role_type & RoleType::Client) {
        send_fd_ = socket(PF_INET, SOCK_STREAM, 0);
    }
    if(role_type & RoleType::Server) {
        recv_fd_ = socket(PF_INET, SOCK_STREAM, 0);
    }
}

void
SimpleTCP::Bind(
        const std::string& ip,
        uint16_t port) {
    static struct sockaddr_in local_addr;
    bzero(&local_addr, sizeof(local_addr));
    local_addr.sin_family = AF_INET;          
    local_addr.sin_port = htons(port);
    inet_pton(AF_INET, ip.c_str(), &local_addr.sin_addr) ;
    bind(recv_fd_, (struct sockaddr*)&local_addr, sizeof(struct sockaddr_in));
    listen(recv_fd_, back_log_);
    local_ip_ = ip;
    local_port_ = port;
}

bool
SimpleTCP::PushPacket(const Packet& packet) {
    struct sockaddr_in serv_addr;
    bzero( &serv_addr, sizeof(serv_addr) );
    serv_addr.sin_family = AF_INET ;   
    serv_addr.sin_port = htons(packet.peer_port_);    
    inet_pton(AF_INET, packet.peer_ip_.c_str(), &serv_addr.sin_addr);
    if(connect(send_fd_, (struct sockaddr*)&serv_addr, sizeof(serv_addr)) != 0) {
        return false;
    }
    if(write(send_fd_, (char*)(packet.data_.c_str()), packet.data_.length()) == -1) {
        return false;
    }
    return true;
}

bool
SimpleTCP::WaitPullPacket(Packet& packet) {
    struct sockaddr_in client_addr;
    socklen_t sock_len;
    int32_t peer_fd = accept(recv_fd_, (struct sockaddr*)&client_addr, &sock_len);
    if(peer_fd < 0)  { return false; }
    packet.data_.resize(buffer_len_);
    if(read(peer_fd, (char*)(packet.data_.c_str()), packet.data_.length()) <= -1) {
        close(peer_fd);
        return false;
    }
    if(!GetPeerAddress(packet.peer_ip_,packet.peer_port_,peer_fd)) {
        close(peer_fd);
        return false;
    }
    close(peer_fd);
    return true;
}

bool
SimpleTCP::ReqPacket(
        const Packet& req_packet,
        Packet& rsp_packet) {
    struct sockaddr_in serv_addr;
    bzero( &serv_addr, sizeof(serv_addr) );
    serv_addr.sin_family = AF_INET ;   
    serv_addr.sin_port = htons(req_packet.peer_port_);    
    inet_pton(AF_INET, req_packet.peer_ip_.c_str(), &serv_addr.sin_addr);
    if(connect(send_fd_, (struct sockaddr*)&serv_addr, sizeof(serv_addr)) != 0) {
        return false;
    }
    if(write(send_fd_, (char*)(req_packet.data_.c_str()), req_packet.data_.length()) == -1) {
        return false;
    }
    rsp_packet.data_.resize(recv_buffer_len_);
    if(read(send_fd_, (char*)(rsp_packet.data_.c_str()), rsp_packet.data_.length()) != -1) { 
        return false;
    }
    return true;
}

bool
SimpleTCP::WaitRspPacket(
        Packet& recv_packet,
        HandlePacketRequest request_handler) {
    struct sockaddr_in client_addr;
    socklen_t sock_len;
    int32_t peer_fd = accept(recv_fd_, (struct sockaddr*)&client_addr, &sock_len);
    if(peer_fd < 0)  {
        return false; 
    }
    recv_packet.data_.resize(recv_buffer_len_);
    if(read(peer_fd, (char*)(recv_packet.data_.c_str()), recv_packet.data_.length()) <= -1) {
        close(peer_fd);
        return false;
    }
    if(!GetPeerAddress(recv_packet.peer_ip_,recv_packet.peer_port_,peer_fd)) {
        close(peer_fd);
        return false;
    }
    std::string rsp_stream;
    request_handler(rsp_stream,recv_packet);
    if(write(peer_fd, (char*)(rsp_stream.c_str()), rsp_stream.length()) == -1) {
       close(peer_fd);
       return false;
    }
    close(peer_fd);
    return true;
}

void
SimpleTCP::DestoryContext() {
    if(role_type_ & RoleType::Client) {
        close(send_fd_);
    }
    if(role_type_ & RoleType::Server) {
        close(recv_fd_);
    }
}

bool
SimpleTCP::GetPeerAddress(
        std::string& ip,
        uint16_t& port,
        int32_t peer_fd) {
    struct sockaddr_in sa;
    socklen_t len = sizeof(sa);
    if(getpeername(peer_fd, (struct sockaddr *)&sa, &len)) {
        return false;
    }
    ip = inet_ntoa(sa.sin_addr);
    port = ntohs(sa.sin_port);
    return true;
}

void
SimpleTCP::ResetBackLog(const int32_t back_log) {
    back_log_ = back_log;
}

void
SimpleTCP::ResetBufferLen(const int32_t buffer_len) {
    buffer_len_ = buffer_len;
}

}
}