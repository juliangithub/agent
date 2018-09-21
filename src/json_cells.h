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
//include once

JSON_CELL_T json_cells[]=
{
	/*	{"json_key",	mib_type,	INIT_FLAG_T, mibindex, get_func, set_pre_func, set_func}*/
	{"cpuUsed",	MIB_STR,	INIT_FLAG_NULL,	0,	cpu_used_percent,	NULL,	NULL},
	{"ramUsed",	MIB_STR,	INIT_FLAG_NULL,	0,	ram_used_percent,	NULL,	NULL},

	{NULL,	MIB_NUM,	INIT_FLAG_NULL, 0, NULL, NULL, NULL}
};



