#ifndef _DALARM_H_
#define _DALARM_H_

#ifndef _ETH_FLAG_H
#include <net/if.h>
#endif
#include "notice_comm.h"

#define ALM_MSG_MAX_LEN 1000

/* alarm id */
typedef enum _ALMID{
	ALM_ID_FLT_START=1, 		/* begin of fault alarm */

	ALM_ID_AP_CPU_USAGE_TOO_HIGH = ALM_ID_FLT_START,
	ALM_ID_AP_MEM_USAGE_TOO_HIGH,
	ALM_ID_TEMP_TOO_HIGH,
	ALM_ID_COCH_INTERF,
	ALM_ID_ADJCH_INTERF,
	ALM_ID_RADIO_DOWN,
	ALM_ID_STA_FULL,
	ALM_ID_STA_INTERF,
	ALM_ID_OTHER_INTERF,
	ALM_ID_AIRITF_RSSI_LOW,
	ALM_ID_FLT_END, 		/* end of fault alarm */
	ALM_ID_EVT_START=1001, 		/* begin of event alarm */
	ALM_ID_SYS_COLD_START = ALM_ID_EVT_START,
	ALM_ID_SYS_WARM_START,
	ALM_ID_AP_OFFLINE,
	ALM_ID_AP_ONLINE,
	ALM_ID_AP_AC_TIME_SYNC_FAILURE,
	ALM_ID_AP_MT_WORK_MODE_CHG,
	ALM_ID_AP_SW_UPGRADE_SUCC,
	ALM_ID_AP_SW_UPGRADE_FAIL,
	ALM_ID_SSID_KEY_CONFLICT,
	ALM_ID_AP_VER_DOWNLOAD,
	ALM_ID_AP_VER_BEGIN_UPDATE,
	ALM_ID_AP_VER_END_UPDATE,
	ALM_ID_CFGFILE_ERR,
	ALM_ID_STA_ONLINE,
	ALM_ID_STA_OFFLINE,
	ALM_ID_AP_RADIO_UP,
	ALM_ID_AP_RADIO_DOWN,
	ALM_ID_IP_ADDR_CHANGE,
	ALM_ID_UNAUTH_SSID,
	ALM_ID_STA_TRAFFIC_OVERLIMIT,
	ALM_ID_UNAUTH_MAC,
	ALM_ID_CPE_WORKMODE_CHG,
	ALM_ID_ETH_UP,
	ALM_ID_ETH_DOWN,
	ALM_ID_STA_AUTH_ERR,
	ALM_ID_STA_ASSOC_FAIL,
	ALM_ID_USR_AUTH_FAIL,
	ALM_ID_AP_HEART_BEAT,
	ALM_ID_AP_RADIO_CHAN_CHG,
	ALM_ID_STA_SNIFF_REPORT,
	
	ALM_ID_EVT_END,			/* end of event alarm */
}ALMID;

EVT_ID 
dl_get_evt_id_by_alm(ALMID alm);
ALMID 
dl_get_alm_id_by_evt(EVT_ID evt);

typedef struct _alm_evt_map_s{
    ALMID alm;
    EVT_ID org_evt;
    EVT_ID final_evt;
}alm_evt_map;

/* alarm number */
#define ALM_FLT_NUM (ALM_ID_FLT_END -  ALM_ID_FLT_START)
#define ALM_EVT_NUM (ALM_ID_EVT_END - ALM_ID_EVT_START)
#define ALM_NUM ((ALM_FLT_NUM) + (ALM_EVT_NUM))

#define IS_EVT_ALM(id) (((id) >= ALM_ID_EVT_START) && ((id) < ALM_ID_EVT_END))
#define IS_FLT_ALM(id) (((id) >= ALM_ID_FLT_START) && ((id) < ALM_ID_FLT_END))

/* alarm type */
typedef enum _ALMTYPE{
	ALM_FLT_RPT=0,
	ALM_FLT_RECV=1,
	ALM_EVT=2,
}ALMTYPE;

const char *
dl_alm_get_type_str(ALMTYPE t);


/* common header for alarm */
typedef struct alarm_msg_header{
	ALMID id;
	ALMTYPE type;
	int length; /* msg length, include message header & body */
	int rpt_time; /* seconds since Epoch */
	int up_time; /* seconds since system start up */
	int serial_id; /* alarm serial number. don't fill in, it be automatically calculated */
}alm_msg_h_t;

/* specific alarm msg body */
#define VERSION_MAX_SIZE 50

enum {
	SW_UPG_FAIL_REASON_FILE_CHECK_FAIL             = 1,
	SW_UPG_FAIL_REASON_SERVER_NOT_AVAILABLE        = 2,
	SW_UPG_FAIL_REASON_FILE_TRANS_ERR              = 3,
	SW_UPG_FAIL_REASON_VER_DEVICE_MODEL_MISMATCH   = 4,
	SW_UPG_FAIL_REASON_INTERNAL_ERR                = 5,
	SW_UPG_FAIL_REASON_ANOTHER_UPG_DOING           = 6,
	SW_UPG_FAIL_REASON_OLD_CONFIG_LOAD_FAIL        = 7,
	SW_UPG_FAIL_REASON_SHORTAGE_MEM                = 8,
	SW_UPG_FAIL_REASON_SYSINFO_ERR                 = 9,
	SW_UPG_FAIL_REASON_PARTITION_SIZE_CHECK        = 10,
	SW_UPG_FAIL_REASON_VERSION_DESC_CHECK 		   = 11,
	SW_UPG_FAIL_REASON_DEPEND_CHECK				   = 12,
};

enum {
    CFG_IMPORT_FAIL_PRASE_ERR               = 1,
    CFG_IMPORT_FAIL_DEVICE_MODEL_MISMATCH   = 2,
};

typedef struct alarm_msg_body_sw_upg_fail{
	int reason_id;
	char cur_sw_ver[VERSION_MAX_SIZE];
	char new_sw_ver[VERSION_MAX_SIZE];
}alm_msg_bd_sw_upg_t;

typedef struct alarm_msg_body_cpu_usg{
	unsigned int usage;
}alm_msg_bd_cpu_usg_t;

typedef struct alarm_msg_body_mem_usg{
	unsigned int usage;
}alm_msg_bd_mem_usg_t;

typedef struct alarm_msg_body_temp{
	unsigned int temp;
}alm_msg_bd_temp_t;

typedef struct alarm_msg_body_sta_online{
	unsigned char sta_mac[6];
	unsigned long int sta_timestamp;
	unsigned int sta_signal;
	unsigned int sta_channel;
    char vapname[IFNAMSIZ];
	char ssid[32 + 1];
    unsigned char bssid[6];
    u_int32_t       phy_rate;
    int8_t          rssi;
    int8_t          antenna_rssi0;
    int8_t          antenna_rssi1;
    int8_t          antenna_rssi2;
    int8_t          snr;
    int8_t          antenna_snr0;
    int8_t          antenna_snr1;
    int8_t          antenna_snr2;
    u_int16_t       associd;
    u_int16_t       inact;
    u_int16_t       state;
}alm_msg_bd_sta_onl_t;

typedef struct alarm_msg_body_sta_offline{
	unsigned char sta_mac[6];
	unsigned long int sta_timestamp;
	unsigned int sta_signal;
	unsigned int sta_channel;
    char vapname[IFNAMSIZ];
	char ssid[32 + 1];
    unsigned char bssid[6];
    u_int32_t       phy_rate;
    int8_t          rssi;
    int8_t          antenna_rssi0;
    int8_t          antenna_rssi1;
    int8_t          antenna_rssi2;
    int8_t          snr;
    int8_t          antenna_snr0;
    int8_t          antenna_snr1;
    int8_t          antenna_snr2;
    u_int32_t       phy_tx_rate;//txratekbps;
    u_int32_t       phy_rx_rate;//rxratekbps;
    u_int32_t       current_phy_tx_rate;//txcurrentratekbps;
    u_int32_t       current_phy_rx_rate;//rxcurrentratekbps;
    u_int32_t       data_tx_rate;//txdatarate;
    u_int32_t       data_rx_rate;//rxdatarate;
    u_int32_t       phy_tx_errors;//txerrors;
    u_int32_t       phy_rx_errors;//rxerrors;
    u_int32_t       phy_tx_drops;//txdrops;
    u_int32_t       phy_rx_drops;//rxdrops;
    u_int16_t       txpower;
    u_int32_t       uptime;
    u_int16_t       associd;
    u_int16_t       inact;
    u_int16_t       state;
    u_int32_t       tx_pkts;
    u_int32_t       rx_pkts;
    u_int64_t       tx_bytes;
    u_int64_t       rx_bytes;
    u_int32_t       rx_unicast;
    u_int32_t       tx_assoc;
}alm_msg_bd_sta_offl_t;

typedef struct alm_msg_bd_ap_radio_up_s {
	unsigned char radio_name[IFNAMSIZ];
}alm_msg_bd_ap_radio_up_t;
typedef struct alm_msg_bd_ap_radio_up_s alm_msg_bd_ap_radio_down_t;

typedef struct alarm_msg_body_cfgfile_err{
	char filename[256];
    int errcode;
}alm_msg_bd_cfg_err_t;

typedef struct alarm_msg_body_ip_addr_change{
	char ifname[IFNAMSIZ];
	unsigned int old_ip;
	unsigned int new_ip;
}alm_msg_bd_ip_chg_t;

typedef struct alarm_msg_body_unauth_ssid{
	char ssid[33];
	unsigned char bssid[6];
}alm_msg_bd_unauth_ssid_t;

typedef struct alarm_msg_body_coch_interf{
	char ifdescr[64];
	unsigned int channel;
	unsigned char interf_bssid[6];
}alm_msg_bd_coch_interf_t;

typedef struct alarm_msg_body_adjch_interf{
	char ifdescr[64];
	unsigned int channel;
	unsigned int interf_channel;
	unsigned char interf_bssid[6];
}alm_msg_bd_adjch_interf_t;

#define RADIO_DOWN_REASON_ADMIN_OP 1
#define RADIO_DOWN_REASON_RADIO_FAULT 2
typedef struct alarm_msg_body_radio_down{
	char ifdescr[64];
	unsigned int down_reason;
}alm_msg_bd_radio_down_t;

#define STA_FULL_FAIL_CODE_FULL 1
typedef struct alarm_msg_sta_full_down{
	unsigned int max_user_num;
	unsigned int fail_code;
}alm_msg_bd_sta_full_t;

#define TF_OVERLIMIT_TYPE_UPLINK 1
#define TF_OVERLIMIT_TYPE_DOWNLINK 2
#define TF_OVERLIMIT_TYPE_BOTH 3
typedef struct alarm_msg_body_sta_traffic_overlimit{
	unsigned char sta_mac[6];
	unsigned int type;
	unsigned int uplink_traffic;
	unsigned int uplink_traffic_limit;
	unsigned int downlink_traffic;
	unsigned int downlink_traffic_limit;
}alm_msg_bd_sta_tf_overlimit_t;

typedef struct alarm_msg_body_ssid_key_conflict{
	char ifname[IFNAMSIZ];
	char vap1[33];
	char vap2[33];
	unsigned int key_index;
	char key_value[256];	
}alm_msg_bd_ssid_key_cf_t;

typedef struct alarm_msg_body_unauth_mac{
	unsigned char mac[6];;
}alm_msg_bd_unauth_mac_t;

typedef struct alarm_msg_body_cpe_workmode_chg {
	char old_mode[16];
	char new_mode[16];
}alm_msg_bd_cpe_workmode_chg_t;

typedef struct alm_msg_bd_eth_up {
	char eth_name[IFNAMSIZ];
}alm_msg_bd_eth_up_t;
typedef struct alm_msg_bd_eth_up alm_msg_bd_eth_down_t;

typedef struct alarm_msg_body_sta_interf{
	char ifdescr[64];
	unsigned int channel;
	unsigned char sta_mac[6];
}alm_msg_bd_sta_interf_t;

typedef struct alarm_msg_body_other_interf{
	char ifdescr[64];
	unsigned int channel;
}alm_msg_bd_other_interf_t;

enum {
	USR_AUTH_ERR_REASON_ALG			= 1,
	USR_AUTH_ERR_REASON_OTHER		= 2,
	USR_AUTH_ERR_REASON_REFUSED		= 3,
	USR_AUTH_ERR_REASON_ASSOC_TOOMANY		= 4,
};

typedef struct alarm_msg_sta_auth_err{
	char radio_name[32];
	char vap_name[32];
	char ssid[33];
	unsigned char bssid[6];
	char authmode[32];
	unsigned char sta_mac[6];
	unsigned int fail_code;
}alm_msg_sta_auth_err_t;

enum {
	STA_ASSOC_FAIL_REASON_INVALID_PARAM	= 1,
	STA_ASSOC_FAIL_REASON_RSN_REQUIRED	= 2,
	STA_ASSOC_FAIL_REASON_CAPINFO		= 3,
	STA_ASSOC_FAIL_REASON_RATES			= 4,
	STA_ASSOC_FAIL_REASON_SHORTSLOT_REQUIRED	= 5,
	STA_ASSOC_FAIL_REASON_NO_HT			= 6,
};

typedef struct alarm_msg_sta_assoc_fail{
	char radio_name[32];
	char vap_name[32];
	char ssid[33];
	unsigned char bssid[6];
	unsigned char sta_mac[6];
	unsigned int fail_code;
}alm_msg_sta_assoc_fail_t;

typedef struct alarm_msg_usr_auth_fail{
	char radio_name[32];
	char vap_name[32];
	char ssid[33];
	unsigned char bssid[6];
	unsigned char sta_mac[6];
	unsigned int fail_code;
}alm_msg_usr_auth_fail_t;

typedef struct alarm_msg_airitf_rssi_low{
	int rssi;
}alm_msg_airitf_rssi_low_t;

typedef struct alm_msg_bd_ap_heart_beat {
	unsigned int ip_addr;
}alm_msg_bd_ap_heart_beat_t;

typedef struct alm_msg_bd_radio_chan_chg {
	char radio_name[IFNAMSIZ];
	unsigned int old_chan;
	unsigned int new_chan;
	unsigned int radio_mode;
}alm_msg_bd_radio_chan_chg_t;

#define STA_SNIFFER_MANUFACTURE_LEN    64
#define STA_SNIFFER_NAME_LEN           32
typedef struct alm_msg_bd_sta_sniff {
    unsigned char   macaddr[6];
    char            rf_name[IFNAMSIZ+1];
    char            signal;
    char            noise;
    u_int8_t        state; //0-online, 1-offline
    u_int8_t        ssid_count;
    char            has_wps;
    //================= the follow field is valid only when has_wps is ture
    u_int16_t       assoc_state;
    u_int16_t       category;
    u_int16_t       subcategory;
    u_int8_t        rfband;//BIT0-2.4G, BIT1-5.8G
    char            manufacture[STA_SNIFFER_MANUFACTURE_LEN+1];
    char            model_name[STA_SNIFFER_NAME_LEN+1];
    char            model_number[STA_SNIFFER_NAME_LEN+1];
    char            device_name[STA_SNIFFER_NAME_LEN+1];
    //=====================================================================
    long            snifftime;              //first snifftime, sys up-time
    long            sniffsystime;              //first snifftime, sys date-time 
    long            duration;               //online time
    char            ssid[0][32+1];  //data depends on ssid_count
}alm_msg_bd_sta_sniff_t;

typedef struct alm_sys_cfg_chg {
    char op_id[16];        //which session trigger this change. array size come from DMS_SYS_SESS_SIZE(dms_sys.h)
//    int sequence;
}alm_sys_cfg_chg_t;

typedef struct alm_sys_cmd_report {
    long long id;
}alm_sys_cmd_report_t;

typedef struct alm_download_notify {
    char id[16];        //download id
    int result;         //download result: 0: normal finishe, other:error or timeout
}alm_download_notify_t;

typedef struct alm_sta_dhcp_notify {
    char mac[18];        //sta mac
    char hostname[128];  //sta's name
}alm_sta_dhcp_notify_t;

typedef struct alm_sta_dhcp_ip_notify {
    char mac[18];        //sta mac
    char ip[18];  //sta's ip
}alm_sta_dhcp_ip_notify_t;

typedef struct alm_sta_dhcp_ip_name_notify {
    char mac[18];        //sta mac
    char ip[18];  //sta's ip
}alm_sta_dhcp_ip_name_notify_t;

typedef struct alm_sys_usb_disk_notify {
	int action; // 1:USB mount success;2:USB mount failed;3:USB remove;
    char msg[512];
    char dev[128];
    char path[128];
}alm_sys_usb_disk_notify_t;

typedef struct evt_portal_auth_info{
	u8 mac[6];
	int online;             //    1, online; 0,offline
	int offline_reason; //0, manually kickoff; 1, aged
	char ifname[IFNAMSIZ];
}evt_portal_auth_info_t;

typedef struct alm_wan_status_notify {
	char mode; // 0:unknow,1:DHCP;2:PPPOE,3:others;
    char action[32];
    char ip[18];
}alm_wan_status_notify_t;
/*************************** Functions ********************/
EVT_ID 
dl_get_org_evt_id_by_alm(ALMID alm);

int 
dl_init_alm_sock();

long int
dl_get_cur_time();

int 
dl_send_org_alm(alm_msg_h_t *msg);

int 
dl_send_alm(alm_msg_h_t *msg);

int
dl_send_sys_cold_start_alm();

int
dl_send_sys_warm_start_alm();

int
dl_send_sw_upgrade_succ_alm();
int
dl_send_sw_upgrade_fail_alm(int reason, const char *cur_ver, const char *new_ver);

int
dl_send_cpu_usage_too_high_alm(unsigned int usage, ALMTYPE type);

int
dl_send_mem_usage_too_high_alm(unsigned int usage, ALMTYPE type);

int
dl_send_temp_too_high_alm(unsigned int temp, ALMTYPE type);

int
dl_send_sta_online_alm(alm_msg_bd_sta_onl_t *para);

int
dl_send_sta_offline_alm(alm_msg_bd_sta_offl_t *para);

int
dl_send_ap_radio_up_alm(alm_msg_bd_ap_radio_up_t *para);
int
dl_send_ap_radio_down_alm(alm_msg_bd_ap_radio_down_t *para);
int
dl_send_eth_up_alm(alm_msg_bd_eth_up_t *para);
int
dl_send_eth_down_alm(alm_msg_bd_eth_down_t *para);

int
dl_send_ap_ac_time_sync_failure_alm();

int
dl_send_cfg_err_alm(alm_msg_bd_cfg_err_t *para);

int
dl_send_ip_chg_alm(alm_msg_bd_ip_chg_t *para);

int
dl_send_ip_unauth_ssid(alm_msg_bd_unauth_ssid_t *para);

int
dl_send_coch_interf_alm(alm_msg_bd_coch_interf_t *para, ALMTYPE type);

int
dl_send_adjch_interf_alm(alm_msg_bd_adjch_interf_t *para, ALMTYPE type);

int
dl_send_radio_down_alm(alm_msg_bd_radio_down_t *para, ALMTYPE type);

int
dl_send_sta_full_alm(alm_msg_bd_sta_full_t *para, ALMTYPE type);

int
dl_send_sta_tf_overlimit(alm_msg_bd_sta_tf_overlimit_t *para);

int
dl_send_ssid_key_cf(alm_msg_bd_ssid_key_cf_t *para);

int
dl_send_unauth_mac(alm_msg_bd_unauth_mac_t *para);

int
dl_send_cpe_workmode_chg_alm(alm_msg_bd_cpe_workmode_chg_t *para);

int
dl_send_sta_interf_alm(alm_msg_bd_sta_interf_t *para, ALMTYPE type);

int
dl_send_other_interf_alm(alm_msg_bd_other_interf_t *para, ALMTYPE type);

int
dl_send_sta_auth_err_alm(alm_msg_sta_auth_err_t *para);

int
dl_send_sta_assoc_fail_alm(alm_msg_sta_assoc_fail_t *para);

int
dl_send_usr_auth_fail_alm(alm_msg_usr_auth_fail_t *para);

int
dl_send_airitf_rssi_low_alm(alm_msg_airitf_rssi_low_t *para, ALMTYPE type);

int
dl_send_ap_heart_beat_alm(alm_msg_bd_ap_heart_beat_t *para);

int
dl_send_ap_radio_chan_chg_alm(alm_msg_bd_radio_chan_chg_t *para);

int
dl_get_up_time();

int
dl_send_sta_sniff_report(alm_msg_bd_sta_sniff_t *para);

int
dl_send_sys_cfg_chg_alm(char *session);
int
dl_send_sys_cmd_report_alm(long long serial);
int
dl_send_portal_auth_evt(evt_portal_auth_info_t *portal_auth);

int 
alm_init();
#endif
