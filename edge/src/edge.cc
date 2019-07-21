#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#include <cassert>

#include "xtrace_sdk/xtrace.h"
#include "simple_tcp/simple_tcp.h"

#define MAXLINE 1024

int main(int argc, char **argv)
{
    top::network::SimpleTCP simple_tcp;
    simple_tcp.CreateContext(top::network::RoleType::Server);
    simple_tcp.Bind("127.0.0.1",39152);
    XTRACE_CREATE_CONTEXT("edge_app", "192.168.50.211", 9411);
    printf("start listening ..\n");
    auto client_req_handler = [](top::network::Packet& packet) {
        char contxt[MAXLINE];
        int data;
        sscanf((char*)packet.data_.c_str(),"%s\r\n%d",contxt,&data);
        auto span_context_maybe = XTRACE_UNPACK_SPAN_CONTEXT(contxt);
        assert(span_context_maybe);
        auto span = XTRACE_START_CHILD_SPAN(span_context_maybe,"proxy client msg");
        XTRACE_FINISH_SPAN(span);
        printf("receive message : %d\n", data);
    };
    while(simple_tcp.WaitProxyReqPacket(
        "127.0.0.1",
        45316,
        client_req_handler)) {}
    simple_tcp.DestoryContext();
    XTRACE_DESTROY_CONTEXT();
    return 0;
}