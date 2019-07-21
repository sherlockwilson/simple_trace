#pragma once

#include <stdint.h>
#include <string>
#include <functional>

namespace top {
namespace network {

struct Packet {
    std::string data_;
    std::string peer_ip_;
    uint16_t    peer_port_;
};

enum RoleType {
    Client = 0x01,
    Server = 0x02,
    Proxy =  0x04,
};

typedef std::function<void(std::string&, Packet&)> HandlePacketRequest;
typedef std::function<void(Packet&)> HandlePacket;

class SimpleTCP {
public:
    SimpleTCP() = default;
    ~SimpleTCP() = default;

    void
    CreateContext(uint64_t role_type = (RoleType::Client | RoleType::Server | RoleType::Proxy));

    void
    Bind(
        const std::string& ip,
        uint16_t port);

    bool
    PushPacket(const Packet& packet);

    bool
    WaitPullPacket(
        Packet& packet);

    bool
    ReqPacket(
        const Packet& req_packet,
        Packet& rsp_packet);

    bool
    WaitProxyReqPacket(
        const std::string& des_ip,
        uint16_t des_port,
        HandlePacket client_req_handler = [](Packet&) {},
        HandlePacket server_rsp_handler = [](Packet&) {});
    
    bool
    WaitRspPacket(
        Packet& recv_packet,
        HandlePacketRequest request_handler);

    void
    ResetBackLog(
        const int32_t back_log);

    void
    ResetBufferLen(
        const int32_t buffer_len);

    void
    DestoryContext();

    std::string
    GetBindIP() { return local_ip_; }

    uint16_t
    GetBindPort() { return local_port_; }

private:
    bool
    GetPeerAddress(
        std::string& ip,
        uint16_t& port,
        int32_t peer_fd);
    bool
    ProxyPacket(
        const Packet& req_packet,
        Packet& rsp_packet);
    enum { kBackLog = 5 };
    enum { kBufferLen = 1024 };
    enum { kRecvBufferLen = 1024 };
    int32_t                           send_fd_ {};
    int32_t                           recv_fd_ {};
    int32_t                           peer_fd_ {};
    int32_t                           back_log_ {kBackLog};
    int32_t                           buffer_len_ {kBufferLen};
    int32_t                           recv_buffer_len_ {kRecvBufferLen};
    uint64_t                          role_type_ {};
    uint16_t                          local_port_;
    std::string                       local_ip_;
};

}
}