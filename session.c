/*===============================================================
*	Copyright (C) 2018 All rights reserved.
*	
*	Filename：session.c
*	Author：julian
*	Date：2018-07-27
*	Discription:
*	
*	release notes：
*
================================================================*/
#include <assert.h>

#include "utils.h"
//#include "data_tree.h"
#include "driver_api.h"
#include "config.h"
#include "cJSON.h"
#include "net.h"
#include "session.h"

static struct alarm_packet alarm_pkt = {0x0};

int get_token(char *filename, char*token, size_t size)
{
	struct stat status;
	FILE *fp;

	if ( stat(filename, &status) < 0)
		return -1;
	fp = fopen(filename, "r");
	if (!fp) {
        	fprintf(stderr, "Read pid file error!\n");
		return -1;
   	}
	fgets(token, size, fp);
	fclose(fp);

	return 0;
}

int token_generate(char *share_key)
{
	int ret = 0;
	int len = 0;
	char token[TOKEN_MAX_LEN]={0x0};
	char cmd[256]={0x0};

	if(strlen(alarm_pkt.challenge) != 0)
	{
		snprintf(token, sizeof(token), "%s%s", share_key, alarm_pkt.challenge);
		dlog_info("token seed :%s \n", token);
		snprintf(cmd, sizeof(cmd), "echo -n \"%s%s\" | sha1sum |awk '{print $1}'> /tmp/token", 
			share_key, token);
		system(cmd);
		get_token("/tmp/token", token, sizeof(token));
		len = snprintf(alarm_pkt.token, TOKEN_MAX_LEN, "%s", token);
		dlog_info("token:%s \n", alarm_pkt.token);
	}
	
	return len;
}

int host_mac()
{
	int ret;
	unsigned char mac[128]={0x};	
	/*unimplemented get host mac addr*/
	
	ret = snprintf(alarm_pkt.host_mac, sizeof(alarm_pkt.host_mac), 
			"%02x:%02x:%02x:%02x:%02x:%02x", 
			mac[0],mac[1], mac[2], 
			mac[3], mac[4], mac[5]);
	return ret;
}

static int http_head_padding(HTTP_METHOD action,char *server_ip,char *url, char *ctype, unsigned char* buf, int clength)
{   

	int offset = 0;
    assert((buf)!=NULL);                        
    if (HTTP_GET == (action)) {                 
        strcat((buf), "GET ");}                 
    else if (HTTP_POST == (action)) {           
        strcat((buf), "POST "); }               
    strcat((buf), (url));                       
    strcat((buf), " HTTP/1.1\r\n");             
    strcat((buf), "Host: ");                    
    strcat((buf), (server_ip));                 
    strcat((buf), "\r\n");                      
	#if 1
	strcat((buf), "User-Agent: Mozilla/5.0 (X11; Ubuntu; Linux x86_64; rv:18.0) Gecko/20100101 Firefox/18.0\r\n");  
    strcat((buf), "Accept: text/html,application/xhtml+xml,application/xml;q=0.9,*/*;q=0.8\r\n");           
    strcat((buf), "Accept-Language: en-US,en;q=0.5\r\n");                      
    strcat((buf), "Connection: Keep-Alive\r\n");                               
    strcat((buf), "Cache-Control: no-cache\r\n");  
    #endif
	sprintf(buf+strlen(buf), "Content-Type:%s\r\n", ctype);
	sprintf(buf+strlen(buf), "Content-Length: %d\r\n", clength);  
	#if 1
	strcat((buf), "Cache-Control: no-cache\r\n");
	#endif
    strcat((buf), "\r\n"); 
	
	offset = strlen(buf);
    return offset;
} 

static int data_gen(char *data, int max_len)
{
	int offset = 0;
	offset += snprintf(data+offset, max_len-offset, "{");	
	offset += snprintf(data+offset, max_len-offset, "\"challange\":\"%s\",", alarm_pkt.challenge);
	offset += snprintf(data+offset, max_len-offset, "\"token\":\"%s\",", alarm_pkt.token);
	offset += snprintf(data+offset, max_len-offset, "\"host_mac\":\"%s\",", alarm_pkt.host_mac);
	
	offset += snprintf(data+offset, max_len-offset, "\"mac\":{");
	offset += get_dhcp_client_list_json(data+offset, max_len - offset -2);
	offset += snprintf(data+offset, max_len-offset, "}");

	//foot padding
	offset += snprintf(data+offset, max_len-offset, "}");

	return offset;
}
static int packet_gen(trsf_buf *trsfb, CONFIG_PKT *config)
{
	int ret;
	int offset=0;
	int max_len = 0;
	char *packet = NULL;
	char data[8192]={0x0};
	int data_len = 0;
	packet = trsfb->data;
	max_len = trsfb->truesize - 2;

	memset((void*)trsfb->data, 0x0, trsfb->truesize);

	data_len = data_gen(data, sizeof(data));
	offset += http_head_padding(HTTP_POST, config->addr, ALARM_URL, CONTENT_TYPE_JSON, trsfb->data, data_len);
	offset += snprintf(trsfb->data+offset, max_len-offset, "%s", data);

	trsfb->data_len = offset;

}

int init_paket(trsf_buf *trsfb, CONFIG_PKT *config)
{
	int ret;
	int offset=0;

	//challenge padding.

	ret = token_generate(config->share_key);
	ret = host_mac();

	dlog_info("challenge:%s \n", alarm_pkt.challenge);
	dlog_info("token:%s \n", alarm_pkt.token);
	dlog_info("host_mac:%s \n", alarm_pkt.host_mac);

	packet_gen(trsfb, config);

	dump_buf(trsfb->data, trsfb->truesize, __FILE__, __LINE__);

	
}

int parse_recv_data(trsf_buf *trsfb)
{
	cJSON *element = NULL;
	char *obj = NULL;
	dump_buf(trsfb->data, trsfb->data_len, __FILE__, __LINE__);
	obj = strchr(trsfb->data, '{');
	dump_buf(obj, trsfb->data_len, __FILE__, __LINE__);
	cJSON *json_root = cJSON_Parse(obj);
	int err_code = 0;
	if (!json_root) {
		dlog_err("cJSON Parse faild !");
		return RET_INVALID_PARMT;
	}
	
	cJSON_Print(json_root);
	element = cJSON_GetObjectItem(json_root, "error");
	if(element->type == cJSON_String && element->valuestring != NULL)
	{
		err_code = atoi(element->valuestring);
	}
	else if(element->type == cJSON_Number)
	{
		err_code = element->valueint;
	}
	else{
		dlog_err("invalid error code %s! \n", element->string);
	}

	element = cJSON_GetObjectItem(json_root, "challange");
	if(element->type == cJSON_String && element->valuestring != NULL)
	{
		dlog_info("last challenge:%s \n", alarm_pkt.challenge);
		dlog_info("new challenge:%s \n", element->valuestring);
		strncpy(alarm_pkt.challenge, element->valuestring, sizeof(alarm_pkt.challenge));
	}
	else{
		dlog_err("invalid challange code %s! \n", element->string);
	}
	
	cJSON_Delete(json_root);
	return RET_SUCCESS;
}

