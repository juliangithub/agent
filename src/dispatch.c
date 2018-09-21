/*===============================================================
*	Copyright (C) 2018 All rights reserved.
*	
*	Filename：dispatch.c
*	Author：julian.chen
*	Date：2018-08-21
*	Discription:
*	
*	release notes：
*
================================================================*/
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/time.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <sys/ioctl.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <sys/stat.h>
#include <linux/wireless.h>
#include <assert.h>

#include "dispatch.h"
#include "cJSON.h"
#include "utils.h"
#include "dlog.h"
#include "config.h"
#include "sysinfo.h"

#include "json_cells.h"

static JSON_CELL_T *find_json_cell(JSON_CELL_T *object, const char *string)	
{
	JSON_CELL_T *idx = object;
	
	while(idx->json_key != NULL){
		if(!strncasecmp(idx->json_key, string, MAX(strlen(idx->json_key),strlen(string)))){
			dlog_debug("found the cell key: %s", idx->json_key);
			return idx;
		}
		idx++;
	}

	return NULL;
}

void json_set_init(int init_flag)
{

	if(init_flag == INIT_FLAG_NULL){
		return;
	}
	else if(init_flag == INIT_FLAG_WIFI){
		//system("reinit_wifi");
	}
	else if(init_flag == INIT_FLAG_WAN){
		//system("reinit_wan");
	}
	else if(init_flag == INIT_FLAG_MODE){
		//system("change_mode");
	}
	else
	{
		//system("reinit_all");
	}

}

	
int set_jsonpkt(cJSON *object)
{
	SK_JSON_ERR_T json_ret = JSON_ERR_NOERROR;
	int apmib_ret = 0;
	cJSON *element = NULL;
	char setVal[MIB_VAL_MAX_LEN] = {0};
	JSON_CELL_T *idx = NULL;
	int init_flag = 0;
	
	
	for(idx = json_cells; (idx->json_key != NULL); idx++)
	{
	
		element = cJSON_GetObjectItem(object, idx->json_key);
		if(element == NULL){
			continue;
		}
		
		dlog_info("set key:%s, mib_index=%d, valuestring:%s, valueint:%d", 
			element->string, idx->mib_index, element->valuestring, element->valueint);
		if(element->type == cJSON_String && idx->mibType == MIB_STR)
		{
			strncpy(setVal, element->valuestring, sizeof(setVal));
		}
		else if(idx->mibType == MIB_NUM)
		{
			if(element->type == cJSON_Number)
			{
				memcpy(setVal, &(element->valueint), sizeof(int));
			}
			else if (element->type == cJSON_String && element->valuestring != NULL)
			{
				element->valueint = atoi(element->valuestring);
				memcpy(setVal, &(element->valueint), sizeof(int));
			}
		}
		else
		{
			json_ret = JSON_ERR_BADVALUE;
			return json_ret;
		}

		//if this mib unit need  some special step first. eg,check parameter.
		if(idx->mib_assert != NULL)
		{
			apmib_ret = idx->mib_assert((void *)setVal);
			if(apmib_ret == 0)
			{
				json_ret=  JSON_ERR_BADVALUE;
				return json_ret;
			}
		}

		if(idx->mib_set!= NULL){
			//some mib unit set no so simply as to update a value.need some special process.
			apmib_ret = idx->mib_set((void *)setVal);
			if(FAILD== apmib_ret)
			{
				dlog_err("mib_set failed");
				json_ret = JSON_ERR_MEMORY;
				return json_ret;
			}
		}
		else
		{
			apmib_ret = nvram_set(idx->mib_index, (void *)setVal);
			if(apmib_ret == 0)	
			{
				dlog_err("mib_set failed");
				json_ret = JSON_ERR_MEMORY;
				return json_ret;
			}
		}
		//set init_flag.
		init_flag |= idx->initType;
	}
	dlog_info("set_jsonpkt SUCCESS");
	json_set_init(init_flag);
	return json_ret;
}


int get_jsonpkt(cJSON *json_obj, char *json_str, int max_length)
{
	SK_JSON_ERR_T json_ret = JSON_ERR_NOERROR;
	int apmib_ret = 0;
	int nBytes=0;
	char mibVal[MIB_VAL_MAX_LEN] = {0};
	JSON_CELL_T *idx = NULL;
	cJSON *c = NULL;

	nBytes += snprintf(json_str+nBytes, max_length, "{");
	for(c=json_obj->child; (c!=NULL&&c->string!=NULL&&c->type!=cJSON_Object); c=c->next)
	{
		idx = find_json_cell(json_cells, c->string);

		if(idx == NULL)
		{
			dlog_err("no such json key:%s", c->string);
			nBytes += snprintf(json_str+nBytes, max_length, JSON_GET_RET, c->string, "unknow");
			continue;
		}
		
		memset(mibVal, 0x00, sizeof(mibVal));
				
		if(idx->mib_get!= NULL)
		{
			//some mib unit get no so simply as to update a value.need some special process.
			apmib_ret = idx->mib_get((void*)mibVal);
		}
		else
		{
			apmib_ret = nvram_get(idx->mib_index, (void *)mibVal);
		}

		if(apmib_ret == FAILD)
		{
			dlog_err("apmib_get Faild MIB index :%d", idx->mib_index);
			json_ret = JSON_ERR_MEMORY;
			return json_ret;
		}
		else
		{
			if(idx->mibType == MIB_NUM)
			{	
				int intVal = 0;
				memcpy(&intVal, mibVal, sizeof(intVal));
				sprintf(mibVal, "%d", intVal);
			}
			
			nBytes += snprintf(json_str+nBytes, max_length, JSON_GET_RET, idx->json_key, mibVal);
		}
	}

	
	if(nBytes <= 1){
		return JSON_ERR_NOTFOUND;
	}
	else{
		snprintf(json_str+nBytes-1, max_length, "}");
		return json_ret;
	}
}

