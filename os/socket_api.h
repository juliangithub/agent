/*===============================================================
*	Copyright (C) 2018 All rights reserved.
*	
*	Filename：socket_api.h
*	Author：julian.chen
*	Date：2018-09-04
*	Discription:
*	
*	release notes：
*
================================================================*/
#ifndef _SOCKET_API_H
#define _SOCKET_API_H

int init_socket(int *sock);
int connect_socket(int sock, char *url,int port);
   
#endif //SOCKET_API_H
