/*===============================================================
*	Copyright (C) 2018 All rights reserved.
*	
*	Filename：transfer.c
*	Author：julian.chen
*	Date：2018-09-04
*	Discription:
*	
*	release notes：
*
================================================================*/
#include <unistd.h>

#include "transfer.h"
#include "tcp_api.h"
#include "utils.h"
int sock_transfer(int sock, trsf_buf *trsfb)
{
	int ret = RET_SUCCESS;
	sock_tcp_send( sock, trsfb->data,  trsfb->data_len);

#if defined(__MINGW32__)
    sleep(2*1000);
#else
    /* Wait a while */
    usleep(200 * 1000);
#endif

	ret = sock_tcp_recv( sock, trsfb->data, trsfb->truesize);
	if(ret > 0)
	{
		trsfb->data_len = ret;
	}
	return ret;
}

