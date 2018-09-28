/*===============================================================
*	Copyright (C) 2018 All rights reserved.
*	
*	Filename：socket_api.c
*	Author：julian.chen
*	Date：2018-09-04
*	Discription:
*	
*	release notes：
*
================================================================*/
#include <sys/types.h>			
#include <sys/socket.h>
#include <netinet/in.h>
#include <netinet/ip.h>
#include <netinet/tcp.h>
#include <sys/stat.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <fcntl.h>
#include <netdb.h>
#include <stdlib.h>
#include <error.h>
#include <stdio.h>
#include <string.h>
#include <assert.h>
#include <signal.h>

#include "socket_api.h"
#include "utils.h"
#include "dlog.h"

int init_socket(int *sock)
{
    int ret = RET_FAILD;
    int sockfd = -1;

    if(!sock)
    {
        dlog_err("Invalid params!");
        return RET_INVALID_PARMT;
    }

    sockfd = socket(AF_INET, SOCK_STREAM, 0);
    if(sockfd < 0)
    {
        dlog_err("Open socket failed.");
        return RET_ERR_GENERAL;
    }
    *sock = sockfd;
    ret = RET_SUCCESS;

    return ret;
}


int connect_socket(int sock, char *url,int port)
{
    int   ret = -1;
    struct sockaddr_in  addr;
    struct hostent      *host;
    char *ip_str;
    char host_addr[256];                 /* ip, hostname, or ip:port */

    if(sock < 0 || !url)
    {
        dlog_err("invalid sock handle!");
        goto end;
    }

    strncpy(host_addr, url, sizeof(host_addr)-1);
#if 0
    dlog_info("url   b: %s", url2);
    colon = strchr(url2, ':');      /* ip:port */
    if(colon)
    {
        port = atoi(colon+1);
        *colon = '\0';
    }
#endif
    dlog_info("host_addr: %s", host_addr);
    dlog_info("port : %d", port);

    bzero(&addr,sizeof(struct sockaddr_in));
    addr.sin_family = AF_INET;
    addr.sin_port   = htons(port);

    if(inet_aton(host_addr, &addr.sin_addr) == 0)
    {
        host = gethostbyname(host_addr);
        if(host == NULL)
        {
            ret = -1;
            dlog_err("Illegal HostName!");
            goto end;
        }
        addr.sin_addr = *(struct in_addr *)(host->h_addr_list[0]);
    }

    /* MUST include <arpa/inet.h>, otherwise inet_ntoa cause coredump on 64-bit machine. */
    if((ip_str = inet_ntoa(addr.sin_addr)))
    {
        dlog_info("Connect ip : %s:%d", ip_str, port);
    }

    if(connect(sock, (struct sockaddr *)(&addr), sizeof(struct sockaddr)) == -1)
    {
        dlog_err("Connect server failed!");
        goto end;
    }

    dlog_info("Connect successful!");
    ret = 0;

    /* Just for test */
    {
        /* Adjust recv / send buffer size to 32K */
        int recv_bufsize = 32*1024;
        int send_bufsize = 32*1024;
        char opt = 1;

#if defined(__MINGW32__)
        unsigned long mode = 1;
        ioctlsocket(sock, FIONBIO, &mode);
#endif

        setsockopt(sock,SOL_SOCKET,SO_RCVBUF,(const char*)&recv_bufsize,sizeof(int));
        setsockopt(sock,SOL_SOCKET,SO_SNDBUF,(const char*)&send_bufsize,sizeof(int));
        setsockopt(sock,IPPROTO_TCP,TCP_NODELAY,&opt,sizeof(char));
        int nNetTimeout = 10*1000;
        setsockopt(sock, SOL_SOCKET, SO_RCVTIMEO, (char *)&nNetTimeout, sizeof(int));
        int flags = fcntl(sock, F_GETFL, 0);
        fcntl(sock, F_SETFL,   flags | O_NONBLOCK /*| O_NDELAY*/);
    }
end:
    return ret ;
}

