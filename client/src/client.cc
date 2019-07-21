// client.cc, copy from example folder
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

int main()
{
    //serv_addr.sin_port = htons(39152);
    top::network::SimpleTCP simple_tcp;
    simple_tcp.CreateContext(top::network::RoleType::Client);
    XTRACE_CREATE_CONTEXT("client_app", "192.168.50.211", 9411);
    auto parent_span = XTRACE_START_ROOT_SPAN("startMain");
    assert(parent_span);
    {
        auto child_span = XTRACE_START_CHILD_SPAN(parent_span,"Step1");
        assert(child_span);

        // Set a simple tag.
        XTRACE_TAG_SPAN(child_span, "simple tag", 123);

        // Set a complex tag.
        XTRACE_TAG_SPAN(child_span, "complex tag", 
                (Values{123, Dictionary{{"abc", 123}, {"xyz", 4.0}}}));

        sleep(1);
        XTRACE_FINISH_SPAN(child_span);
    }

    // Create a follows from span.
    {
        auto child_span = 
            XTRACE_START_FOLLOW_SPAN(parent_span,"Step1_1");
        sleep(1);

        // child_span's destructor will finish the span if not done so explicitly.
    }

    // Use custom timestamps.
    {
        auto span = XTRACE_START_CHILD_SPAN(parent_span,"Step3");
        assert(span);
        XTRACE_FINISH_SPAN(span);
    }

 //   if(connect(socket_fd, (struct sockaddr*)&serv_addr, sizeof(serv_addr)) == 0)
  //  {
        printf("client starts ...\n");

    while(1) {
        static int n = 0;
        if( n>=6) break;
        char span_name[24];
        sprintf(span_name, "request_%d", n++);
        auto child_span = XTRACE_START_CHILD_SPAN(parent_span,span_name);
        std::string contxt = XTRACE_PACK_SPAN(child_span);
        // for testing
        if(n == 2) sleep(2);
        top::network::Packet req_packet;
        req_packet.data_.resize(MAXLINE);
        printf("send message : %d\n", n);
        sprintf((char*)(req_packet.data_.c_str()), "%s\r\n%d",contxt.c_str(), n);
        req_packet.peer_ip_ = "192.168.50.211";
        req_packet.peer_port_ = 45316;
        //req_packet.peer_port_ = 39152;
        top::network::Packet rsp_packet;
        simple_tcp.ReqPacket(req_packet,rsp_packet);
        printf("get message from server: %s\n", rsp_packet.data_.c_str());
        XTRACE_FINISH_SPAN(child_span);
    }
    simple_tcp.DestoryContext();
    XTRACE_FINISH_SPAN(parent_span);
    XTRACE_DESTROY_CONTEXT();
    return 0;
}