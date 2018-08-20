#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#include "config.h"
#include "net.h"
#include "session.h"
#include "utils.h"

static CONFIG_PKT g_conf_pkt = {0x0};
static trsf_buf	g_trsfb = {
	.truesize = TRSF_DATA_SIZE,
	.data_len = 0,
	.data = NULL,
};

static int initial()
{
	char *packet = NULL;
	if(init_data_tree() == 0) {
		dlog_err("%s:%d: apmib_init error\n", __FUNCTION__, __LINE__);
		return -1;
	}
	read_config(&g_conf_pkt);
	packet = (char*)malloc(TRSF_DATA_SIZE);

	if(NULL == packet){
		return RET_ERR_GENERAL;
	}
	else{
		g_trsfb.data = packet;
		g_trsfb.data_len = 0;
	}
	
}

static int clean_process(){
	free(g_trsfb.data);
}
int
main(int argc, char **argv)
{
	int sock_fd;               
	int ret;
	initial();
	
	while(1)
	{
		init_socket(&sock_fd);

		ret = connect_socket(sock_fd, g_conf_pkt.addr,g_conf_pkt.port);
		if(ret < 0)
		{
			dlog_err("http_sock_connect url failed!");
			sleep_interval_dynamic();
			continue;
		}
		
		init_paket(&g_trsfb, &g_conf_pkt);
		sock_transfer(sock_fd,&g_trsfb);
		ret = parse_recv_data(&g_trsfb);
		if(ret == RET_SUCCESS)
		{
			//5 mins
			usleep(5*60*1000000);
		}
		else{
			//faild retry in 5 seconds.
			usleep(5*1000000);
		}
		
	}

	clean_process();
}


