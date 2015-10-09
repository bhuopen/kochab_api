/*****************************************************************************
//
//  Copyright (c) 2011 Shanghai Shenghua Communication Co., Ltd.
//  All Rights Reserved
//  No portions of this material may be reproduced in any form without the
//  written permission of:
//	Shanghai Shenghua Communication Co., Ltd.
//	Room 1222, Building 3, No.100, Jinyu Road,
//	Pudong New District, Shanghai, China
//  All information contained in this document is Shanghai Shenghua
//  Communication Co., Ltd. private, proprietary, and trade secret.
//
******************************************************************************
//
//  Filename:       smem.h
//  Author:         Terry Yang <ybable@gmail.com>
//  Creation Date:  19/08/2011
//  Change history:
//
//  Description:
//	TODO
//
*****************************************************************************/

#ifndef _SMEM_H_
#define _SMEM_H_

#ifdef LOCK_DEBUG
#include "lockdebug.h"
#endif

#define SMEM_OBJ_NAME_SIZE 32

struct smem_obj {
	int index;
	char name[SMEM_OBJ_NAME_SIZE];
	int shmid;
	int shmsize;
	int semid;
	int semn;
	int cnt;
	void *shmaddr;
};

#ifdef LOCK_DEBUG

#define SMEM_HOLDER_INFO_MAX    3
#define SMEM_FUNC_MAX           5

struct _func_info {
    char fname[LOCK_DEBUG_INFO_LEN];
    int lineno;
};

struct _pid_info {
    pid_t pid;
    int lock_count;
    struct _func_info funcinfo[SMEM_FUNC_MAX];
};

struct _lock_info {
    char name[LOCK_DEBUG_INFO_LEN];
    int max_count;
    struct _pid_info pidinfo[SMEM_HOLDER_INFO_MAX];    
};
#endif

int
smem_create (struct smem_obj *m);

int
smem_destroy (struct smem_obj *m);

#ifdef LOCK_DEBUG
int
_smem_access (struct smem_obj *m, int ro, const char *fname, int lineno);
#define smem_access(m, ro) _smem_access(m, ro , __func__, __LINE__)
#else
int
smem_access (struct smem_obj *m, int ro);
#endif

int
smem_leave (struct smem_obj *m);

int
smem_leave_all (struct smem_obj *m);

int
smem_share (struct smem_obj *m, int ro);

int
smem_clear(struct smem_obj *m);

#ifdef LOCK_DEBUG
int
_smem_timed_access (struct smem_obj *m, int ro, int tmout, const char *fname, int lineno);
#define smem_timed_access(m, ro, t) _smem_timed_access(m, ro, t, __func__, __LINE__)
#else
int
smem_timed_access (struct smem_obj *m, int ro, int tmout);
#endif

#ifdef LOCK_DEBUG
int
smem_get_holder_lock_info(struct _lock_info *pi);
#endif

#endif /* _SMEM_H_ */
