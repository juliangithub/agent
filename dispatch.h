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
   
#define MIB_VAL_MAX_LEN		8192

typedef int (*MIB_ASSERT_CB)(void* paramt);
typedef int (*MIB_SET_CB)(void* paramt);
typedef int (*MIB_GET_CB)(void* paramt);

typedef struct sk_json_cell{
	char *json_key;
	int mibType;
	int initType;
	unsigned int mib_index;	
	MIB_GET_CB mib_get;
	MIB_ASSERT_CB mib_assert;
	MIB_SET_CB mib_set;
}JSON_CELL_T,*JSON_CELL_TP;

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

#define FAILD -1 
#ifndef JSON_RET
#define JSON_RET(trsfb, err_code) { \
		trsfb->data_len += snprintf(trsfb->data,  trsfb->truesize,"{\"result\":\"%d\"}",err_code); \
		}
#endif

#endif //DISPATCH_H
