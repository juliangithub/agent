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
	
	/** @def MAX(a, b)
		Computers the maximum of a and b. */
#define MAX(a,b) ((a) > (b) ? (a) : (b))
	
	/** @def MIN(a, b)
		Computers the minimum of a and b. */
#define MIN(a,b) ((a) > (b) ? (b) : (a))

static inline int dump_buf(unsigned char *packet, int buf_len, char *filename, int line)
{
	printf("*****************%s %d*********************\n", filename, line);
	printf("%s \n", packet);
	printf("**************************************\n");
}

int get_pid(char *filename);
void sleep_interval_dynamic();

#endif //UTILS_H
