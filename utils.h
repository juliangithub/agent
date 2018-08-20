/*===============================================================
*	Copyright (C) 2018 All rights reserved.
*	
*	Filename：utils.h
*	Author：julian
*	Date：2018-07-27
*	Discription:
*	
*	release notes：
*
================================================================*/
#ifndef _UTILS_H
#define _UTILS_H
   
typedef enum
{
	RET_SUCCESS = 0,
	RET_FAILD = 1,
	RET_INVALID_PARMT = 2,
	RET_ERR_GENERAL = 3,
}RET;
	

static inline int dump_buf(unsigned char *packet, int buf_len, char *filename, int line)
{
	printf("*****************%s %d*********************\n", filename, line);
	printf("%s \n", packet);
	printf("**************************************\n");
}

int get_pid(char *filename);
void sleep_interval_dynamic();

#endif //UTILS_H
