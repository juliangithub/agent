/*===============================================================
*	Copyright (C) 2018 All rights reserved.
*	
*	Filename：transfer.h
*	Author：julian.chen
*	Date：2018-09-04
*	Discription:
*	
*	release notes：
*
================================================================*/
#ifndef _TRANSFER_H
#define _TRANSFER_H

#define TRSF_DATA_SIZE	81920

typedef struct trsf_buf_s
{
	const unsigned int  truesize;
    unsigned int  data_len;
    unsigned char *data;

} trsf_buf, *p_trsf_buf ;

int sock_transfer(int sock, trsf_buf *trsfb);

#endif //TRANSFER_H
