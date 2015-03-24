#ifndef _NOTICE_ITF_H
#define _NOTICE_ITF_H

int
evt_subscribe(EVT_ID id, EVT_HANDLE_FUC handler);

int
evt_multi_subscribe(evt_reg_info *eri, u32 num);

int
evt_unsubscribe(EVT_ID id);

int
evt_multi_unsubscribe(EVT_ID *ids, u32 num);

int
evt_publish(evt_info *evt);

void
evt_subscribe_set_mode(EVT_SUBSCRIBE_MODE mode);
void
evt_subscribe_get_sock_and_handler(int *sock, void(**handler)(void*));

unsigned long int
evt_get_sock_thread();

void
evt_client_deinit();
#endif /* _NOTICE_ITF_H */
