#ifndef _DRIVER_API_H_
#define _DRIVER_API_H_

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

#define _DHCPC_PID_PATH		"/etc/udhcpc"
#define _DHCPC_PROG_NAME	"udhcpc"
#define _PATH_PROCNET_ROUTE	"/proc/net/route"

#define _DHCPD_PROG_NAME	"udhcpd"
#define _DHCPD_PID_PATH		"/var/run"
#define _PATH_DHCPS_LEASES	"/var/lib/misc/udhcpd.leases"

struct dhcpOfferedAddr {
		u_int8_t chaddr[16];
		u_int32_t yiaddr;		/* network order */
		u_int32_t expires;		/* host order */
		char hostname[64];
};

#define SSID_LEN 32
typedef struct _bss_info {
    unsigned char state;
    unsigned char channel;
    unsigned char txRate;
    unsigned char bssid[6];
    unsigned char rssi, sq;
    unsigned char ssid[SSID_LEN+1];
} bss_info;

typedef enum _wlan_mac_state {
    STATE_DISABLED=0, STATE_IDLE, STATE_SCANNING, STATE_STARTED, STATE_CONNECTED, STATE_WAITFORKEY
} wlan_mac_state;

int
getWanLink(char *ifname);

int
ioctl(char *name, unsigned int arg0, unsigned int arg1, unsigned int arg2, unsigned int arg3);

int
get_vxd_state(char *ifname);

static int
getWlBssInfo(char *interface, bss_info *pInfo);

static int
iw_get_ext(int skfd, char *ifname, int request, struct iwreq *pwrq);

int get_dhcp_client_list_json(char *json_buf, int buf_size);

#endif
