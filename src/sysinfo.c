/*===============================================================
 *	Copyright (C) 2018 All rights reserved.
 *	
 *	Filename：sysinfo.c
 *	Author：julian.chen
 *	Date：2018-09-04
 *	Discription:
 *	
 *	release notes：
 *
 ================================================================*/
#include "sysinfo.h"
#include "utils.h"

#define NONE_CLIENT		"\"0.0.0.0\":\"00:00:00:00:00:00\""
//#define NONE_CLIENT		"\"\":\"\""


int get_arp_list_json(char *json_buf, int buf_size)
{
	FILE *fp;
	int nBytesSent=0;
	int element=0;
	char IPaddress[64];
	char HWtype[64];
	char Flags[64];
	char HWaddress[64];
	char Mask[64];
	char Device[64];
	char buf[128]={0x0};
	char *pfgets = NULL;
	char json_item[128];
	int item_len;

	fp = fopen(_ARP_LIST, "r");
	if ( fp == NULL )
		goto err;

	pfgets = fgets(buf, sizeof(buf), fp);
	dlog_debug("head:%s pfgets :%s\n",buf, pfgets);


	while(NULL != fgets(buf, sizeof(buf), fp) )
	{
		dlog_debug("line[%d]%s\n",element, buf );
		sscanf(buf, "%[^ ] %[^ ] %[^ ] %[^ ] %[^ ] %[^ ]", IPaddress, HWtype, Flags, HWaddress, Mask, Device);
		if(element == 0)
		{
			item_len = snprintf(json_item, sizeof(json_item), 
					"\"%s\":\"%s\"", IPaddress, HWaddress);
		}
		else
		{
			item_len = snprintf(json_item, sizeof(json_item), 
					",\"%s\":\"%s\"", IPaddress, HWaddress);
		}

		if(item_len > buf_size-nBytesSent-2)
		{
			printf("buffer space is not enough ,drop items.");
			break;
		}
		else{
			nBytesSent += snprintf(json_buf+nBytesSent, buf_size-nBytesSent,
					"%s", json_item);
		}
		element++;

	}


err:
	if (element == 0) {
		nBytesSent += snprintf(json_buf, buf_size-nBytesSent, 				
				NONE_CLIENT
				);
	}

	return nBytesSent;
}

int cpu_used_percent(void *paramt)
{
	FILE *fp = NULL;
	char line[128] = {0};
	unsigned long user=0, system=0, nice=0, idle=0;
	unsigned long total = 0;

	if((fp = popen("cat /proc/stat | grep cpu0", "r")) != NULL){
		fgets(line, sizeof(line), fp);
		pclose(fp);
		sscanf(line, "%*s %lu %lu %lu %lu", &user, &system, &nice, &idle);
	}

	total = user + system + nice + idle;
	if(total == 0) return RET_FAILD;

	sprintf(paramt, "%.2lf", 100.00*(user + system + nice)/total);
	return RET_SUCCESS;
}


int ram_used_percent(void *paramt)
{
	FILE *fp = NULL;
	unsigned long total = 0;
	unsigned long used = 0;
	char line[128] = {0};

	if((fp = popen("free | grep Mem:", "r")) != NULL){
		fgets(line, sizeof(line), fp);
		pclose(fp);
		sscanf(line, "%*s %lu %lu", &total, &used);
	}

	if(total == 0) return RET_FAILD;
	sprintf(paramt, "%.2lf", 100.00*used/total);
	return RET_SUCCESS;
}


