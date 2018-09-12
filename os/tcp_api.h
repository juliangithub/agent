/*===============================================================
*	Copyright (C) 2018 All rights reserved.
*	
*	Filename：tcp_api.h
*	Author：julian.chen
*	Date：2018-09-04
*	Discription:
*	
*	release notes：
*
================================================================*/
#ifndef _TCP_API_H
#define _TCP_API_H
   
int sock_tcp_send(int sock, unsigned char *data, unsigned int data_len);

int sock_tcp_recv(int sock, unsigned char *buf, unsigned int buf_len);

#endif //TCP_API_H
