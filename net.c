/*===============================================================
*	Copyright (C) 2018 All rights reserved.
*	
*	Filename：net.c
*	Author：julian
*	Date：2018-07-27
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


#include "net.h"
#include "utils.h"

int init_socket(int *sock)
{
    int ret = -1;
    int sockfd = -1;

    if(!sock)
    {
        dlog_err("Invalid params!");
        goto end;
    }

    sockfd = socket(AF_INET, SOCK_STREAM, 0);
    if(sockfd < 0)
    {
        dlog_err("Open socket failed.");
        goto end;
    }
    *sock = sockfd;
    ret = 0;

end:
    return ret;
}


int connect_socket(int sock, char *url,int port)
{
    int   ret = -1;
    struct sockaddr_in  addr;
    struct hostent      *host;
    char *ip_str;
    char *colon;
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

static int sock_send(int sock, trsf_buf *trsfb)
{
	int  ret = RET_FAILD;
    struct timeval timeout;
    fd_set fds;
    int max_fd ;

    if(sock < 0 || !trsfb || !trsfb->data_len)
    {
        dlog_err("Invalid params!");
        return RET_INVALID_PARMT;
    }

    timeout.tv_sec  = 1;
    timeout.tv_usec = 0;
    max_fd = sock + 1;

    FD_ZERO(&fds);
    FD_SET(sock,&fds);

    ret = select(max_fd, NULL, &fds, NULL, &timeout);
    if(ret <= 0)
    {
		dlog_err("send timeout!");
		return RET_ERR_GENERAL;
    }

    if(FD_ISSET(sock,&fds))
    {
        ret = send(sock, (const char *)trsfb->data, trsfb->data_len, 0);
		if(ret == -1)
        {
            dlog_err("send data failed!");
            return RET_ERR_GENERAL;
        }
    }
    else
    {
        dlog_err("select failed!");
        return RET_ERR_GENERAL;
    }
}


static int sock_recv(int sock, trsf_buf *trsfb)
{
	int  ret = -1;
    struct timeval timeout;
    fd_set fds;
    int max_fd ;

    memset(trsfb->data, 0x00, trsfb->truesize);
    trsfb->data_len = 0;

    timeout.tv_sec  = 8;
    timeout.tv_usec = 0;
	max_fd = sock + 1;
	
    FD_ZERO(&fds);
    FD_SET(sock,&fds);

    ret = select(max_fd, &fds, NULL, NULL, &timeout);
    if(ret <= 0)
    {
        dlog_err("recv timeout!");
        return RET_ERR_GENERAL;
    }

    if(FD_ISSET(sock, &fds))
    {
        ret = recv(sock, (void *)(trsfb->data), trsfb->truesize-1, 0);
        if(ret == 0 || ret == -1)
        {
            dlog_err("no recv!");
           return RET_ERR_GENERAL;
        }
		trsfb->data_len = ret;
    }
    else
    {
        dlog_err("seelct failed!");
        return RET_ERR_GENERAL;
    }

	return RET_SUCCESS;
}

int sock_transfer(int sock, trsf_buf *trsfb)
{
	sock_send(sock, trsfb);
#if defined(__MINGW32__)
    sleep(2*1000);
#else
    /* Wait a while */
    usleep(200 * 1000);
#endif
	sock_recv(sock, trsfb);
}

