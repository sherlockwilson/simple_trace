// server.cc
#include <unistd.h>
#include <sys/types.h>       /* basic system data types */
#include <sys/socket.h>      /* basic socket definitions */
#include <netinet/in.h>      /* sockaddr_in{} and other Internet defns */
#include <arpa/inet.h>       /* inet(3) functions */
#include <netdb.h>           /* gethostbyname function */
#include <stdlib.h>
#include <errno.h>
#include <stdio.h>
#include <string.h>

#include <cassert>
#include <iostream>
#include <unordered_map>

#include "xtrace_sdk/xtrace.h"
#include "simple_tcp/simple_tcp.h"

using namespace zipkin;
using namespace opentracing;

#define MAXLINE 1024
#define LISTENQ 1024

using XTraceSDK = top::xtrace::XTraceSDK;

int main(int argc, char **argv) {
    top::network::SimpleTCP simple_tcp;
    simple_tcp.CreateContext(top::network::RoleType::Server);
    XTRACE_CREATE_CONTEXT("server_app", "192.168.50.211", 9411);
    simple_tcp.Bind("192.168.50.211",45316);
    //simple_tcp.Bind("127.0.0.1",39152);    
    printf("start listening ..\n");
    auto func_handler = [](std::string& rsp_stream, top::network::Packet& packet) {
        char contxt[MAXLINE];
        int data;
        sscanf((char*)packet.data_.c_str(),"%s\r\n%d",contxt,&data);
        printf("recv msg:%s\n",packet.data_.c_str());
        auto span_context_maybe = XTRACE_UNPACK_SPAN_CONTEXT(contxt);
        assert(span_context_maybe);
        auto span = XTRACE_START_CHILD_SPAN(span_context_maybe,"receive");
        XTRACE_FINISH_SPAN(span);
        rsp_stream.resize(MAXLINE);
        sprintf((char*)(rsp_stream.c_str()), "back %d", data);
    };
    top::network::Packet recv_packet;
    while(simple_tcp.WaitRspPacket(recv_packet, func_handler)) {}
    simple_tcp.DestoryContext();
    XTRACE_DESTROY_CONTEXT();
    return 0;
}