/******************************************************
*these code can be used as lib.
*some of them is copyed from ohers, chengqiuping, etc.
******************************************************/
#include <apmib.h>
#include "driver_api.h"

#define NONE_CLIENT		"\"0.0.0.0\":\"00:00:00:00:00:00\""
//#define NONE_CLIENT		"\"\":\"\""

#define IOCTL_GETWANLINKSTATUS 2000
int
getWanLink(char *ifname)
{
        unsigned int ret;
        unsigned int args[0];

        ioctl(ifname, IOCTL_GETWANLINKSTATUS, (unsigned int)(args), 0, (unsigned int)&ret);
        return ret;
}

int
ioctl(char *name, unsigned int arg0, unsigned int arg1, unsigned int arg2, unsigned int arg3)
{
	unsigned int args[4];
	struct ifreq ifr;
	int sockfd;
	
	args[0] = arg0;
	args[1] = arg1;
	args[2] = arg2;
	args[3] = arg3;

	if((sockfd = socket(AF_INET, SOCK_DGRAM, 0)) < 0) {
		perror("fatal error socket\n");
		return -3;
	}
  
	strcpy((char*)&ifr.ifr_name, name);
	((unsigned int *)(&ifr.ifr_data))[0] = (unsigned int)args;
	
	if(ioctl(sockfd, SIOCDEVPRIVATE, &ifr)<0) {
		perror("device ioctl:");
		close(sockfd);
		return -1;
	}

	close(sockfd);
	return 0;
}

int
get_vxd_state(char *ifname)
{
	int state = 0;
	char buffer[500] = {0x0};
	bss_info bss;
	
	memset((void*)&bss ,0x0, sizeof(bss));
	strcpy(buffer, ifname);
	getWlBssInfo(buffer, &bss);
        
	switch(bss.state) {
	case STATE_CONNECTED:
		state = 0;
	break;
	
	default:
		state = -1;
	break;
	}
	
	return state;
}

static int
getWlBssInfo(char *interface, bss_info *pInfo)
{
#ifndef NO_ACTION
	int skfd=0;
	struct iwreq wrq;
	
	skfd = socket(AF_INET, SOCK_DGRAM, 0);
	if(skfd == -1) {
		return -1;
	}

	/* Get wireless name */
	if(iw_get_ext(skfd, interface, SIOCGIWNAME, &wrq) < 0)
	#ifdef VOIP_SUPPORT 
	{
		// rock: avoid status page error if no wlan interface
		memset(pInfo, 0, sizeof(bss_info));
		return 0;
	}
	#else
	/* If no wireless name : no wireless extensions */
	{
		close(skfd);
		return -1;
	}
	#endif

	wrq.u.data.pointer = (caddr_t)pInfo;
	wrq.u.data.length = sizeof(bss_info);
	
	if(iw_get_ext(skfd, interface, SIOCGIWRTLGETBSSINFO, &wrq) < 0) {
		close(skfd);
		return -1;
	}
	close(skfd);
#else
	memset(pInfo, 0, sizeof(bss_info)); 
#endif

	return 0;
}

static int
iw_get_ext(int skfd, char *ifname, int request, struct iwreq *pwrq)
{
  /* Set device name */
  strncpy(pwrq->ifr_name, ifname, IFNAMSIZ);
  /* Do the request */
  return ioctl(skfd, request, pwrq);
}



static int parse_dhcp_lease(char **ppStart, unsigned long *size, char *ip, char *mac, char *liveTime, char *hostname)
{
	unsigned long tmp_expires = 0;
	
	struct dhcpOfferedAddr entry;
	u_int8_t empty_haddr[16]; 
    
     	memset(empty_haddr, 0, 16); 
	if ( *size < sizeof(entry) )
		return -1;

	entry = *((struct dhcpOfferedAddr *)*ppStart);
	*ppStart = *ppStart + sizeof(entry);
	*size = *size - sizeof(entry);

	if (entry.expires == 0)
		return 0;

	if(!memcmp(entry.chaddr, empty_haddr, 16)){
		//fprintf(stderr, "got a unavailable entry for ip=%s\n",inet_ntoa(*((struct in_addr *)&entry.yiaddr)));
		return 0;
	}
	strcpy(ip, inet_ntoa(*((struct in_addr *)&entry.yiaddr)) );
	snprintf(mac, 20, "%02x:%02x:%02x:%02x:%02x:%02x",
			entry.chaddr[0],entry.chaddr[1],entry.chaddr[2],entry.chaddr[3],
			entry.chaddr[4], entry.chaddr[5]);
	
	if(entry.expires == 0xffffffff)
	{
		sprintf(liveTime, "%s", "Always");
	}
	else
	{
		tmp_expires= (unsigned long)ntohl(entry.expires);
		
		sprintf(liveTime, "%lu:%lu:%lu", 
			(tmp_expires/60)/60, (tmp_expires/60)%60, tmp_expires%60);
	}
	sprintf(hostname, "%s", entry.hostname);
	return 1;
}



int get_dhcp_client_list_json(char *json_buf, int buf_size)
{
	FILE *fp;
	int nBytesSent=0;
	int element=0, ret;
	char ipAddr[40], macAddr[40], liveTime[80], hostname[80],*buf=NULL, *ptr, tmpBuf[100];
	struct stat status;
	int pid;
	unsigned long fileSize=0;
	char json_item[128];
	int item_len;
	// siganl DHCP server to update lease file
	snprintf(tmpBuf, 100, "%s/%s.pid", _DHCPD_PID_PATH, _DHCPD_PROG_NAME);
	pid = get_pid(tmpBuf);
	snprintf(tmpBuf, 100, "kill -SIGUSR1 %d\n", pid);
	
	if ( pid > 0)
	{
		//kill(pid, SIGUSR1);
		system(tmpBuf);
	}
	usleep(1000);

	if ( stat(_PATH_DHCPS_LEASES, &status) < 0 )
		goto err;

	fileSize=status.st_size;
	buf = malloc(fileSize);
	if ( buf == NULL )
		goto err;
	fp = fopen(_PATH_DHCPS_LEASES, "r");
	if ( fp == NULL )
		goto err;

	fread(buf, 1, fileSize, fp);
	fclose(fp);

	ptr = buf;
	
	while (1) {
		ret = parse_dhcp_lease(&ptr, &fileSize, ipAddr, macAddr, liveTime, hostname);

		if (ret < 0 )
			break;
		if (ret == 0)
			continue;
		if(element == 0)
		{
			item_len = snprintf(json_item, sizeof(json_item), 
				"\"%s\":\"%s\"", ipAddr, macAddr);
		}
		else
		{
			item_len = snprintf(json_item, sizeof(json_item), 
				",\"%s\":\"%s\"", ipAddr, macAddr);
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
	
	if (buf)
		free(buf);

	return nBytesSent;
}

