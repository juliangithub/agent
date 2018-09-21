/*===============================================================
*	Copyright (C) 2018 All rights reserved.
*	
*	Filename：config.c
*	Author：julian
*	Date：2018-07-27
*	Discription:
*	
*	release notes：
*
================================================================*/
#include "config.h"
//#include "data_tree.h"
#include "utils.h"

static int get_server_port(unsigned int* server_port)
{
    int ret = -1;
	*server_port = DEFAULT_HTTP_PORT;
	return ret;
}

static int get_server_url(char *url)
{
	int ret = -1;
	snprintf(url, "%s", "www.uri.com");
	if(ret !=0)
	{
	    ret = 0;
	}
	return ret;
}

static int get_share_key(char *key)
{
	int ret = -1;
	snprintf(key, "%s", "share.key");
	if(ret !=0)
	{
	    ret = 0;
	}
	return ret;

}

int read_config(CONFIG_PKT *config_pkt)
{
	get_server_port(&(config_pkt->port));

	get_server_url(config_pkt->addr);

	get_share_key(config_pkt->share_key);
}


