#ifndef __LOCKDEBUG_H__
#define __LOCKDEBUG_H__

#define LOCK_DEBUG_INFO_LEN  64

struct _lock_debug_info {
    char fname[LOCK_DEBUG_INFO_LEN];
    time_t lock_time;
};

void 
lock_info_init();
struct _lock_debug_info *
get_eloop_holder();
struct _lock_debug_info *
get_evt_holder();
struct _lock_debug_info *
get_mevt_holder();
struct _lock_debug_info *
get_timer_holder();
struct _lock_debug_info *
get_schedule_holder();

#endif /* __FW_H__ */
