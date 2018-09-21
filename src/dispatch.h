/*===============================================================
*	Copyright (C) 2018 All rights reserved.
*	
*	Filename：dispatch.h
*	Author：julian.chen
*	Date：2018-08-21
*	Discription:
*	
*	release notes：
*
================================================================*/
#ifndef _DISPATCH_H
#define _DISPATCH_H

#define FAILD -1

#define MIB_VAL_MAX_LEN		8192

typedef enum { 
	JSON_REQ_NOSPT = 0,
	JSON_REQ_GET=1, 
	JSON_REQ_SET=2
}JSON_REQ_MODE_T;
	
typedef enum { 
	JSON_ERR_NOERROR=0, 
	JSON_ERR_BADVALUE=1,
	JSON_ERR_WRONGTYPE=2,
	JSON_ERR_NOTFOUND=3,
	JSON_ERR_MEMORY=4
}SK_JSON_ERR_T;



#endif //DISPATCH_H
