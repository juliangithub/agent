/*===============================================================
*	Copyright (C) 2018 All rights reserved.
*	
*	Filename：config.h
*	Author：julian
*	Date：2018-07-27
*	Discription:
*	
*	release notes：
*
================================================================*/
#ifndef _CONFIG_H
#define _CONFIG_H

#define DEFAULT_HTTP_PORT 80
#define addr_MAX_LEN	512
#define share_key_MAX_LEN	512

typedef struct config_pkt {
	unsigned int port;
	//ip or url
	char addr[addr_MAX_LEN];
	char share_key[share_key_MAX_LEN];
}CONFIG_PKT;


int read_config();
int nvram_set(unsigned int index, void * paramt);
int nvram_get(unsigned int index, void * paramt);


#endif //CONFIG_H
