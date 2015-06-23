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

int
smem_create (struct smem_obj *m);

int
smem_destroy (struct smem_obj *m);

int
smem_access (struct smem_obj *m, int ro);

int
smem_leave (struct smem_obj *m);

int
smem_leave_all (struct smem_obj *m);

int
smem_share (struct smem_obj *m, int ro);

int
smem_clear(struct smem_obj *m);

#endif /* _SMEM_H_ */
