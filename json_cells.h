/*===============================================================
*	Copyright (C) 2018 All rights reserved.
*	
*	Filename：json_cells.h
*	Author：julian.chen
*	Date：2018-08-21
*	Discription:
*	
*	release notes：
*
================================================================*/
#ifndef _JSON_CELLS_H
#define _JSON_CELLS_H

	typedef enum { 
		MIB_NUM = 0,
		MIB_STR=1, 
	}JSON_CELL_TYPE_T;
		
	typedef enum{
		INIT_FLAG_NULL=0x00, 
		INIT_FLAG_ALL=0x01, 
		INIT_FLAG_WIFI=0x02, 
		INIT_FLAG_WAN=0x04, 
		INIT_FLAG_MODE=0x08
	}INIT_FLAG_T;

	JSON_CELL_T json_cells[]=
	{
	/*	{"json_key",		mib_type,		INIT_FLAG_T, apmib, get_func, set_pre_func, set_func}*/
	{"cpuUsed",			MIB_STR,	INIT_FLAG_NULL, 0, cpu_used_percent, NULL, NULL},
	{"ramUsed",			MIB_STR,	INIT_FLAG_NULL, 0, ram_used_percent, NULL, NULL},
	
	{NULL,		NULL,			MIB_NUM,	INIT_FLAG_NULL, 0, NULL, NULL, NULL}
};

#define JSON_GET_RET	"\"%s\":\"%s\","

#define JSON_RET(req, err_code) { \
	send_json_perm(req, (err_code==0)?"{\"result\":\"success\"}":"{\"result\":\"fail\"}"); }
   
#endif //JSON_CELLS_H