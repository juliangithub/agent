/*===============================================================
*	Copyright (C) 2018 All rights reserved.
*	
*	Filename：net.h
*	Author：julian
*	Date：2018-07-27
*	Discription:
*	
*	release notes：
*
================================================================*/
#ifndef _NET_H
#define _NET_H

#define TRSF_DATA_SIZE	81920

typedef struct trsf_buf_s
{
	const unsigned int  truesize;
    unsigned int  data_len;
    unsigned char *data;

} trsf_buf, *p_trsf_buf ;

int init_socket(int *sock);
int connect_socket(int sock, char *url,int port);
int sock_transfer(int sock, trsf_buf *trsfb);


#endif //NET_H
