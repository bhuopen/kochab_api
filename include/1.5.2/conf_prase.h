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
//  Filename:       conf_prase.h
//  Author:         Terry Yang <ybable@gmail.com>
//  Creation Date:  19/08/2011
//  Change history:
//
//  Description:
//	TODO
//
*****************************************************************************/

#ifndef _CONF_PRASE_H_
#define _CONF_PRASE_H_

struct conf_at {/* Attribute */
	struct conf_at *next;
	char *name;
	char *val;
#if 0
	unsigned short id;
	unsigned short res;
#endif
};

struct conf_cs {/* Container Subitem */
	struct conf_cs *next;
	struct conf_tb *st;/* Subtable */
	char *name;
};

struct conf_tb {/* Table */
	struct conf_tb *next;
	struct conf_at *at;
	struct conf_cs *cs;
};

struct conf_ct {/* Container Table */
	struct conf_ct *next;
	struct conf_ct *child;
	struct conf_tb *tb;
	char *name;
	int flags;
};

enum {
	CONF_DAT_TYPE_NULL,
	CONF_DAT_TYPE_CT,
	CONF_DAT_TYPE_TB,
	CONF_DAT_TYPE_CS,
	CONF_DAT_TYPE_AT,
};

struct conf_l {
	void *mem;
	int sz;
	int fsz;
	struct conf_ct *ct;
	struct conf_l *next;
	struct conf_ct *pi;
	void *tmp;
	int group;
	/* For error msg return */
	int err;
	char *err_msg;
	void *err_dat; /* Point to dat in list, don't free it! */
	char *err_path; /* Need free it */
	int err_dat_type;
};

struct conf_arg {
	int lv;
	int sc;
	int group;
	int tb_id;
	struct conf_l *cl;
};

#define CONF_TABLE_NAME_MAXLEN	32
#define CONF_TABLE_LEVEL_MAX	16

struct conf_item_cvt {
	const char *old_name;
	const char *new_name;
};

struct conf_item_def {
	const char *name;
	const char *val;
};

void
conf_printf_tb (struct conf_tb *tb, const char *title, int log_id, int log_lv);

void
conf_printf_ct (struct conf_ct *ct, int log_id, int log_lv);

struct conf_l *
conf_newlist (void);

struct conf_tb *
conf_newtb (void);

struct conf_ct *
conf_newct (const char *name);

struct conf_l *
conf_file2list (const char *fn);

int
conf_item_convert(struct conf_item_cvt *cvt, struct conf_tb *ctb, struct conf_tb *ctb2);

int
conf_item_default(struct conf_item_def *def, struct conf_tb *ctb);

struct conf_l *
conf_str2list (char *buf, int len);

char *
conf_find_value(struct conf_tb *tb, const char *name);

int
conf_fill_head_value (struct conf_tb *tb, char *name, char *val);

int
conf_fill_value(struct conf_tb *tb, char *name, char *val);

char *
conf_get_value(struct conf_arg *arg, struct conf_tb *tb, int id, const char *name);

int
conf_set_value(struct conf_arg *arg, struct conf_tb *tb, int id, char *name, char *val);

struct conf_cs *
conf_add_sub (struct conf_tb *tb, const char *name);

struct conf_tb *
conf_new_subitem (struct conf_cs *cs);

int
conf_list_free_cs (struct conf_cs *cs);

int
conf_list_free_tb (struct conf_tb *tb);

int
conf_list_free_ct (struct conf_ct *ct);

int
conf_list_free (struct conf_l *l);

int
conf_list2text_tb (struct conf_tb *tb, int lv, char *buf);

int
conf_list2text_ct (struct conf_ct *ct, int lv, char *buf);

char *
conf_list2text (struct conf_l *l);

char *
conf_path2value (struct conf_l *l, const char *path, const char *id, const char *idv);

char *
conf_pi_path2value (struct conf_l *l, const char *path, const char *id, const char *idv);
int
conf_list_set_pi(struct conf_l *l, const char *tbname, const char *atname, const char *val);

int
conf_path2ct (struct conf_l *l, const char *path, struct conf_ct **pct);

int
conf_add_ct (struct conf_l *l, const char *path, struct conf_ct *ct);
int
conf_cut_ct (struct conf_l *l, const char *path);

struct conf_l *
conf_merge (struct conf_l *l1, struct conf_l *l2, char **tb, int tbn, int cover);

#define CONF_MERGE_COVER_REPLACE	1
#define CONF_MERGE_COVER_FORCE		2

struct conf_l *
conf_dup_l (struct conf_l *l);

struct conf_l *
conf_grab (struct conf_l *l, char **tb, int tbn);

int
conf_xml_convert(char *s, int lv, char *buf);

int
conf_xml_reconvert(char *s, int sz);


#endif /* _CONF_PRASE_H_ */
