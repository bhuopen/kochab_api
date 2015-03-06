#include<stdio.h>
#include"tconv.h"
#include"dalarm.h"
#include"notice_itf.h"

int nt_wlanuser_handler(evt_info *ei)
{
    char buf[2][32];
    alm_msg_h_t *msg;
    alm_msg_bd_sta_onl_t *on_sta;
    alm_msg_bd_sta_offl_t *off_sta;

    if(ei == NULL) {
        printf("Error for null ei!!");
        return -1;
    }
    if (ei->flag) {
        printf("Expired event which generated before our subscribe!\n");
        return 0;
    }

    msg = (alm_msg_h_t *)ei->data;

    switch(ei->id){
    case EVT_ID_ALM_STA_ONLINE:
        on_sta = (alm_msg_bd_sta_onl_t *)(msg + 1);
        printf("New user(%s) on: %s %s\n",
                tc_mac2str(on_sta->sta_mac, buf[0]),
                tc_mac2str(on_sta->bssid, buf[1]),
                on_sta->ssid);
        break;
    case EVT_ID_ALM_STA_OFFLINE:
        off_sta = (alm_msg_bd_sta_offl_t *)(msg + 1);
        printf("User(%s) leave: %s %s\n",
                tc_mac2str(off_sta->sta_mac, buf[0]),
                tc_mac2str(off_sta->bssid, buf[1]),
                off_sta->ssid);
        break;
    }

    msg = (alm_msg_h_t *)ei->data;
}

int main()
{
    evt_reg_info regs[2];

    regs[0].id = EVT_ID_ALM_STA_ONLINE;
    regs[0].handler = nt_wlanuser_handler;

    regs[1].id = EVT_ID_ALM_STA_OFFLINE;
    regs[1].handler = nt_wlanuser_handler;

    if(evt_multi_subscribe(regs, 2) < 0){
        printf("Subscribe failed!\n");
        return -1;
    }

    printf("Input any char to stop!\n");
    if (getchar()) {
        ;
    }
    evt_client_deinit();
    return 0;
}
