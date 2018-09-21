/*===============================================================
*	Copyright (C) 2018 All rights reserved.
*	
*	Filename：session.h
*	Author：julian
*	Date：2018-07-27
*	Discription:
*	
*	release notes：
*
================================================================*/
#ifndef _SESSION_H
#define _SESSION_H

#define CONTENT_TYPE_JSON "application/json"
#define ALARM_URL "/ipmac/report"
typedef enum
{
    HTTP_GET = 1,
    HTTP_POST,
    HTTP_PUT,
    HTTP_DELETE
}HTTP_METHOD;

typedef enum err_code_e{
	ERCODE_NOERR = 0,
	ERCODE_CHALNG = 1,
	ERCODE_SHAE_KEY = 2
}err_code;

#define CHALG_MAX_LEN	256
#define TOKEN_MAX_LEN	256
typedef struct alarm_packet
{
	char challenge[CHALG_MAX_LEN];
	char token[TOKEN_MAX_LEN];
	char host_mac[64];
}ALARM_PKT; 

int init_paket(trsf_buf *trsfb, CONFIG_PKT *config);
int parse_recv_data(trsf_buf *trsfb);

 
#endif //SESSION_H
