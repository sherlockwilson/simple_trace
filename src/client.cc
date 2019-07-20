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

using namespace zipkin;
using namespace opentracing;

#define MAXLINE 1024

int main()
{
    char buf[MAXLINE];
    char buf2[MAXLINE];
    struct sockaddr_in serv_addr ;

    bzero( &serv_addr, sizeof(serv_addr) );
    serv_addr.sin_family = AF_INET ;   
    //serv_addr.sin_port = htons(39152);
    serv_addr.sin_port = htons(45316);    
    inet_pton(AF_INET, "127.0.0.1", &serv_addr.sin_addr);
    
    XTRACE_CREATE_CONTEXT("client_app");

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
        int socket_fd = socket(PF_INET, SOCK_STREAM, 0);
        if(connect(socket_fd, (struct sockaddr*)&serv_addr, sizeof(serv_addr)) == 0) {
            static int n = 0;

            if( n>=6) break;

            char span_name[24];
            sprintf(span_name, "request_%d", n++);

            auto child_span = XTRACE_START_CHILD_SPAN(parent_span,span_name);
            std::string contxt = XTRACE_PACK_SPAN(child_span);

            // for testing
            if(n == 2) sleep(2);
            
            printf("send message : %d\n", n);
            sprintf(buf, "%s\r\n%d",contxt.c_str(), n);
            if(write(socket_fd, buf, MAXLINE) == -1)
                printf("write error\n");

            sleep(1);

            // wait for response
            while(read(socket_fd, buf2, MAXLINE) == -1) { sleep(1);}
            printf("get message from server: %s\n", buf2);
            XTRACE_FINISH_SPAN(child_span);
        }
        close(socket_fd);
    }

    XTRACE_FINISH_SPAN(parent_span);
    XTRACE_DESTROY_CONTEXT();
    return 0;
}