#ifndef _NOTICE_COMM
#define _NOTICE_COMM

#include "typedef.h"

#define SUB_CLI_PATH "/tmp/evt/cli_"
#define DIS_SERVER_PATH "/tmp/evt/dis_server"
#define MAX_MSG_BODY_LEN 4096

#define MSG_LEN(s) sizeof(evt_inner_msg_hdr) + (s)
#define SOCK_UN_ADDR_LEN(s) \
          offsetof(struct sockaddr_un, sun_path) + (s)

#define EVT_SEND_TIMEOUT    30


typedef enum _EVT_SUBSCRIBE_MODE{
    EVT_SUBSCRIBE_MODE_NORMAL,
    EVT_SUBSCRIBE_MODE_EMBEDDED,
}EVT_SUBSCRIBE_MODE;

/**
 * Event identifier
 */
typedef enum _EVT_ID {
	EVT_ID_NULL,

    /*orginal alarm evt id*/
	EVT_ID_ORG_ALM_SYS_COLD_START,
	EVT_ID_ORG_ALM_SYS_WARM_START,
	EVT_ID_ORG_ALM_AP_SW_UPGRADE_SUCC,
	EVT_ID_ORG_ALM_AP_SW_UPGRADE_FAIL,
	EVT_ID_ORG_ALM_AP_CPU_USAGE_TOO_HIGH,
	EVT_ID_ORG_ALM_AP_MEM_USAGE_TOO_HIGH,
	EVT_ID_ORG_ALM_TEMP_TOO_HIGH,
	EVT_ID_ORG_ALM_STA_ONLINE,
	EVT_ID_ORG_ALM_STA_OFFLINE,
	EVT_ID_ORG_ALM_AP_RADIO_UP,
	EVT_ID_ORG_ALM_AP_RADIO_DOWN,
	EVT_ID_ORG_ALM_AP_AC_TIME_SYNC_FAILURE,
	EVT_ID_ORG_ALM_CFGFILE_ERR,
	EVT_ID_ORG_ALM_IP_ADDR_CHANGE,
	EVT_ID_ORG_ALM_UNAUTH_SSID,
	EVT_ID_ORG_ALM_COCH_INTERF,
	EVT_ID_ORG_ALM_ADJCH_INTERF,
	EVT_ID_ORG_ALM_STA_INTERF,
	EVT_ID_ORG_ALM_OTHER_INTERF,
	EVT_ID_ORG_ALM_RADIO_DOWN,
	EVT_ID_ORG_ALM_STA_FULL,
	EVT_ID_ORG_ALM_STA_TRAFFIC_OVERLIMIT,
	EVT_ID_ORG_ALM_SSID_KEY_CONFLICT,
	EVT_ID_ORG_ALM_UNAUTH_MAC,
	EVT_ID_ORG_ALM_CPE_WORKMODE_CHG,
	EVT_ID_ORG_ALM_ETH_UP,
	EVT_ID_ORG_ALM_ETH_DOWN,
	EVT_ID_ORG_ALM_STA_AUTH_ERR,
	EVT_ID_ORG_ALM_STA_ASSOC_FAIL,
	EVT_ID_ORG_ALM_USR_AUTH_FAIL,
	EVT_ID_ORG_ALM_AIRITF_RSSI_LOW,
	EVT_ID_ORG_ALM_AP_HEART_BEAT,
	EVT_ID_ORG_ALM_AP_RADIO_CHAN_CHG,
	EVT_ID_ORG_ALM_STA_SNIFF_REPORT,

    /*final alarm evt id*/
	EVT_ID_ALM_SYS_COLD_START,
	EVT_ID_ALM_SYS_WARM_START,
	EVT_ID_ALM_AP_SW_UPGRADE_SUCC,
	EVT_ID_ALM_AP_SW_UPGRADE_FAIL,
	EVT_ID_ALM_AP_CPU_USAGE_TOO_HIGH,
	EVT_ID_ALM_AP_MEM_USAGE_TOO_HIGH,
	EVT_ID_ALM_TEMP_TOO_HIGH,
	EVT_ID_ALM_STA_ONLINE,
	EVT_ID_ALM_STA_OFFLINE,
	EVT_ID_ALM_AP_RADIO_UP,
	EVT_ID_ALM_AP_RADIO_DOWN,
	EVT_ID_ALM_AP_AC_TIME_SYNC_FAILURE,
	EVT_ID_ALM_CFGFILE_ERR,
	EVT_ID_ALM_IP_ADDR_CHANGE,
	EVT_ID_ALM_UNAUTH_SSID,
	EVT_ID_ALM_COCH_INTERF,
	EVT_ID_ALM_ADJCH_INTERF,
	EVT_ID_ALM_STA_INTERF,
	EVT_ID_ALM_OTHER_INTERF,
	EVT_ID_ALM_RADIO_DOWN,
	EVT_ID_ALM_STA_FULL,
	EVT_ID_ALM_STA_TRAFFIC_OVERLIMIT,
	EVT_ID_ALM_SSID_KEY_CONFLICT,
	EVT_ID_ALM_UNAUTH_MAC,
	EVT_ID_ALM_CPE_WORKMODE_CHG,
	EVT_ID_ALM_ETH_UP,
	EVT_ID_ALM_ETH_DOWN,
	EVT_ID_ALM_STA_AUTH_ERR,
	EVT_ID_ALM_STA_ASSOC_FAIL,
	EVT_ID_ALM_USR_AUTH_FAIL,
	EVT_ID_ALM_AIRITF_RSSI_LOW,
	EVT_ID_ALM_AP_HEART_BEAT,
	EVT_ID_ALM_AP_RADIO_CHAN_CHG,
	EVT_ID_ALM_STA_SNIFF_REPORT,

    /* other event id */
    EVT_ID_TRAP_SWITCH,
	EVT_ID_TRAP_THRESHOLD,
    EVT_ID_STAT_INFO,
	EVT_ID_SYS_TIME_CHG,
	EVT_ID_BMS_DHCP_OPTION_GET,
	EVT_ID_SYS_IF_NOTICE,
	EVT_ID_WAN_STATUS_CHANGE,
	EVT_ID_SYS_CONFIG_CHANGE,
	EVT_ID_SYS_CMD_REPORT,
	EVT_ID_SYS_DOWNLOAD_END,

	EVT_ID_MAX,
}EVT_ID;

/**
 * Event info
 */
typedef struct evt_info_s {
	EVT_ID id;
	u32 flag:1; /*complementary dispatch flag, only used for subscriber to distinguish */
		    /*whether the received evt occur before subscribing or not. */
	u32 length; /*length of data */
	u8 data[0]; /*evt content*/
}evt_info;

/**
 * Event handle callback function
 */
typedef int (*EVT_HANDLE_FUC)(evt_info *evt);

typedef struct evt_reg_info_s {
	EVT_ID id;
	EVT_HANDLE_FUC handler;
}evt_reg_info;


#endif /* _NOTICE_COMM */
