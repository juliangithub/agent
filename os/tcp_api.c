/*===============================================================
*	Copyright (C) 2018 All rights reserved.
*	
*	Filename：tcp_api.c
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

#include "tcp_api.h"
#include "utils.h"
#include "dlog.h"

int sock_tcp_send(int sock, unsigned char *buf, unsigned int buf_len)
{
	int  ret = -1;
    struct timeval timeout;
    fd_set fds;
    int max_fd ;

    if(sock < 0 || !buf || !buf_len)
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
		return ret;
    }

    if(FD_ISSET(sock,&fds))
    {
        ret = send(sock, (const char *)buf, buf_len, 0);
		if(ret == -1)
        {
            dlog_err("send data failed!");
            return ret;
        }
    }
    else
    {
        dlog_err("select failed!");
        return ret;
    }

	return ret;
}


//return recv data length.
int sock_tcp_recv(int sock, unsigned char *buf, unsigned int buf_len)
{
	int  ret = -1;
    struct timeval timeout;
    fd_set fds;
    int max_fd ;

    timeout.tv_sec  = 8;
    timeout.tv_usec = 0;
	max_fd = sock + 1;
	
    FD_ZERO(&fds);
    FD_SET(sock,&fds);

    ret = select(max_fd, &fds, NULL, NULL, &timeout);
    if(ret <= 0)
    {
        dlog_err("recv timeout!");
        return ret;
    }

    if(FD_ISSET(sock, &fds))
    {
        ret = recv(sock, (void *)buf, buf_len-1, 0);
        if(ret == 0 || ret == -1)
        {
            dlog_err("no recv!");
           return ret;
        }
    }
    else
    {
        dlog_err("seelct failed!");
        return ret;
    }

	return ret;
}


