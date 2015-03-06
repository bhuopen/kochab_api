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
//  Filename:       dmdb.c
//  Author:         Terry Yang <ybable@gmail.com>
//  Creation Date:  19/08/2011
//  Change history:
//
//  Description:
//	TODO
//
*****************************************************************************/

#ifndef _DMDB_H_
#define _DMDB_H_

#include "typedef.h"
#include "smem.h"
#include "conf_prase.h"
#include "dmdb_auth.h"


#define BHU_PI_TAG              "binfo"
#define BHU_PI_MODEL_VER_NAME   "config_model_ver"
#define BHU_PI_DEV_NAME         "dev_name"

#define CONFIG_MODEL_VER        "V3"
#define CONFIG_LOAD_FAIL_FLAG   "/etc/config/conf_load_fail"

/* Common index type */
#define TB_INDEX_TYPE_NULL	0
#define TB_INDEX_TYPE_DEF	1
#define TB_INDEX_TYPE_NAME	2
#define TB_INDEX_TYPE_INDEX	3
#define TB_INDEX_TYPE_VAL	4

#define TB_SAVE_SCOPE_INTERNAL	1
#define TB_SAVE_SCOPE_ALL	3
#define TB_SAVE_SCOPE_SETTING	4
#define TB_SAVE_SCOPE_CONFIG	5

#define TB_LOAD_LEVEL_FAILRET	(1<<0)
#define TB_LOAD_LEVEL_BOOTING	(1<<1)
#define TB_LOAD_LEVEL_INHERENT	(1<<2)
#define TB_LOAD_LEVEL_CONVERT	(1<<3)
#define TB_LOAD_LEVEL_PRERELY	(1<<5)
#define TB_LOAD_LEVEL_RELY	(1<<6)
#define TB_LOAD_LEVEL_RELY2	(1<<7)
#define TB_LOAD_LEVEL_PREBASIC	(1<<8)
#define TB_LOAD_LEVEL_BASIC	(1<<9)
#define TB_LOAD_LEVEL_EXTEND	(1<<10)
#define TB_LOAD_LEVEL_USERDEF	(1<<11)
#define TB_LOAD_LEVEL_RUNSET	(1<<29)
#define TB_LOAD_LEVEL_MAX	(1<<30)
#define TB_LOAD_LEVEL_MAXN	31
#define TB_LOAD_LEVEL_SETTING	0x7FFFFFF0

#define TB_ITEM_FLAG_INTERNAL	(1<<0)
#define TB_ITEM_FLAG_NOCUT	(1<<1)
#define TB_ITEM_FLAG_NOSAVE	(1<<2)
#define TB_ITEM_FLAG_NOSAVEFILE	(1<<3)
#define TB_ITEM_FLAG_IGNORE_NOITEM	(1<<24)
#define TB_ITEM_FLAG_CHANGE	(1<<25)
#define TB_ITEM_FLAG_SYSINIT	(1<<26)
#define TB_ITEM_FLAGS_DEFAULT	0
#define TB_ITEM_FLAGS_APISET	0xF0000000	/* used by api internal. */
#define TB_ITEM_FLAGS_SAVEMASK	0xFFFF

#define DMDB_NAME_MAXLEN	32

extern const char *tb_index_type_name[];

struct db_table_hdr {
	int room;
	int used;
	int inher;/*inherent config*/
};

struct db_item_hdr {
	u32 flags;
};

struct db_err_msg {
	int id;
	int idn;
	const char **msg;
};

#define DB_ERR_MSG_SYSID_START	16384
#define DB_ERR_MSG_ID_START	32768
#define DB_ERR_MSG_ID_END	65535

enum {
    DB_ATTR_TYPE_NULL,

    DB_ATTR_TYPE_STR,   /* sz1: min length, sz2: max length */
    DB_ATTR_TYPE_ENUM,  /* sz1: start number, sz2: max number, arg: pointer to char pointer array */
    DB_ATTR_TYPE_INT,   /* sz1: min value, sz2: max value */
    DB_ATTR_TYPE_IPV4,  /* sz1: no use, sz2: no use */
    DB_ATTR_TYPE_IPV6,  /* sz1: no use, sz2: no use */
    DB_ATTR_TYPE_NETMASKV4,  /* sz1: no use, sz2: no use */
    DB_ATTR_TYPE_ENABLE,     /* sz1: no use, sz2: no use */
};

#define DB_ATTR_FLAG_ARRAY  0x1

struct db_attr_desc {
	const char *name;
	const char *desc;
	u16 id;
	u16 type;
    u32 flags;
    int sz1, sz2;
    void *arg;
	int (*check)(struct db_attr_desc *desc, const char *val, struct db_table_hdr *tbh, int m);
};
/* e.g.:
 * String: network interface
 *      type = DB_ATTR_TYPE_STR,
 *      sz1 = 1,
 *      sz2 = IFNAMSIZ,
 * Enum: wan link type
 *      type = DB_ATTR_TYPE_ENUM,
 *      sz1 = 1,
 *      sz2 = WAN_LINK_TYPE_MAX-1,
 *      arg = wan_link_type_str,
 *          other:
 *          enum {WAN_LINK_TYPE_NULL
 *              WAN_LINK_TYPE_STATIC,
 *              WAN_LINK_TYPE_DHCP,
 *              WAN_LINK_TYPE_PPPOE,
 *              WAN_LINK_TYPE_MAX,
 *          };
 *          const char wan_link_type_str[] = { "",
 *              "Static",
 *              "DHCP",
 *              "PPPoE",
 *          }
 * Integer: vlan id
 *      type = DB_ATTR_TYPE_INT,
 *      sz1 = 1,
 *      sz1 = 4094,
 * Array: bridge ports
 *      type = DB_ATTR_TYPE_STR,
 *      sz1 = 1,
 *      sz2 = IFNAMSIZ,
 *      flags |= DB_ATTR_FLAG_ARRAY,
 */

struct db_table {
	const char *name;
	u8 vector:1;/* If it is a vector table, otherwise, it is a scalar table. */
	u8 running:1;/* If is a running status table. It will not save in user config file. */
	u8 virtualtb:1;/* Virtual table. */
	int size;
	int maxn;
	int extsize;
	u32 loadlvs;/* Have load levels. 0 means TB_LOAD_LEVEL_BAISC */

    const char *desc;
	const struct db_err_msg *err_msg;
	const struct db_attr_desc *attr_desc;

	int (*load)(struct db_table_hdr *tbh, int m, struct conf_tb *ctb, struct conf_arg *arg);
	struct conf_tb * (*save)(struct db_table_hdr *tbh, int m, struct conf_arg *arg);
	int (*match)(struct db_table_hdr *tbh, int type, const void *dat, int sz);/* Get index, for vector */
	int (*add)(struct db_table_hdr *tbh, int type, const void *dat, int sz, int flags);
	int (*del)(struct db_table_hdr *tbh, int id, int flags);
	int (*preload)(struct db_table_hdr *tbh, int lv);
	int (*loaded)(struct db_table_hdr *tbh, int lv);
	int (*init)(struct db_table_hdr *tbh);
	int (*exit)(struct db_table_hdr *tbh);
	char * (*upload_path)(struct db_table_hdr *tbh, struct conf_arg *arg, char *ind, char *type, int *psz);
	int (*upload_finish)(struct db_table_hdr *tbh, struct conf_arg *arg, char *ind, char *type);

	char index_name[DMDB_NAME_MAXLEN];
};

#define DB_CT_FLAGS_NOSAVE  (1<<2)

struct db_container {
	const char *name;
	int  tb_id;
	struct db_container *child;
	u16  child_sz;
	u16  flags;
};

struct dmdb {
	struct smem_obj smem;
	struct db_table_hdr **tbhs;
};

#define DR(db, type, offset) ((type)((db).smem.shmaddr+(offset)))
#define DBADDR(db, i) ((struct db_table_hdr *)((char *)(db).smem.shmaddr + (int)((struct db_table_hdr **)(db).smem.shmaddr)[i]) )

extern struct dmdb g_db;

int
db_mem_init ();
#endif /* _DMDB_H_ */
