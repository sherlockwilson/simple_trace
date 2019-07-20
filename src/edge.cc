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

using namespace zipkin;
using namespace opentracing;

#define MAXLINE 1024
#define LISTENQ 1024

int server_socket_fd = socket(PF_INET, SOCK_STREAM, 0);
struct sockaddr_in serv_addr;


void send_msg_to_server(
        std::string& out, 
        int data, 
        opentracing::expected<std::unique_ptr<opentracing::SpanContext>>& parent_span_context) {
    char buf[MAXLINE];
    out.resize(MAXLINE);
    auto child_span = XTRACE_START_CHILD_SPAN(parent_span_context,"edge_request_server");
    std::string contxt = XTRACE_PACK_SPAN(child_span);

    sprintf(buf, "%s\r\n%d",contxt.c_str(), data);
    if(write(server_socket_fd, buf, MAXLINE) == -1)
        printf("write error\n");

    sleep(1);

    // wait for response
    while(read(server_socket_fd, (char*)(out.c_str()), MAXLINE) == -1) { sleep(1);}
    printf("get message from server: %s\n", out.c_str());
    XTRACE_FINISH_SPAN(child_span);
}

int main(int argc, char **argv)
{
    bzero(&serv_addr, sizeof(serv_addr));
    serv_addr.sin_family = AF_INET ;   
    serv_addr.sin_port = htons(45316);    
    inet_pton(AF_INET, "127.0.0.1", &serv_addr.sin_addr);

    int listen_fd, connect_fd;
    char buf[MAXLINE];
    char buf2[MAXLINE];
    socklen_t len;
    struct sockaddr_in edge_addr, client_addr;

    XTRACE_CREATE_CONTEXT("edge_app");

    listen_fd = socket(PF_INET, SOCK_STREAM, 0);
    bzero(&edge_addr, sizeof(edge_addr));
    edge_addr.sin_family = AF_INET;          
    edge_addr.sin_port = htons(39152);
    inet_pton(AF_INET, "127.0.0.1", &edge_addr.sin_addr) ;
    bind(listen_fd, (struct sockaddr*)&edge_addr, sizeof(struct sockaddr_in));

    printf("start listening ..\n");
    listen(listen_fd, LISTENQ); 
    if(connect(server_socket_fd, (struct sockaddr*)&serv_addr, sizeof(serv_addr)) != 0)
    {
        return -1;
    }
    for( ; ; ) 
    {
        connect_fd = accept(listen_fd, (struct sockaddr*)&client_addr, &len);
        if(connect_fd < 0) continue;

        printf("accept from %s : %d \n", inet_ntoa(client_addr.sin_addr), client_addr.sin_port);
        while(1)
        {
            if (read(connect_fd, buf, MAXLINE) > -1)
            {
                char contxt[MAXLINE];
                std::string context;
                context.resize(MAXLINE);
                int data;
                sscanf(buf,"%s\r\n%d",contxt,&data);
                auto span_context_maybe = XTRACE_UNPACK_SPAN_CONTEXT(contxt);
                if(!span_context_maybe) 
                    printf("error context............\n");

                auto span = XTRACE_START_CHILD_SPAN(span_context_maybe,"edge handle client msg");
                printf("receive message : %d\n", data);
                
                // for testing
                if (data == 4) sleep(2);

                sprintf(buf2, "back %d", data);
                sleep(1);

                XTRACE_FINISH_SPAN(span);
                std::string out;
                send_msg_to_server(out, data, span_context_maybe);

                auto child_span = XTRACE_START_CHILD_SPAN(span_context_maybe,"edge_send_client");
                if(write(connect_fd, (char*)out.c_str(), MAXLINE) == -1) {
                    printf("write error");
                    break;
                }
                XTRACE_FINISH_SPAN(child_span);
                
            }
            sleep(1);
        }
        close(connect_fd);
        sleep(2);
    }
    close(server_socket_fd);
    XTRACE_DESTROY_CONTEXT();
}