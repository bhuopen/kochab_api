#ifndef _DMDB_AUTH_H_
#define _DMDB_AUTH_H_

#include "typedef.h"

struct db_auth_at{
	u16 r:1;
	u16 w:1;
	u16 res:2;
	u16 id:12;
}__attribute__((packed));

struct db_auth_tb{
	u32 r:1;
	u32 w:1;
	u32 c:1;
	u32 d:1;
	u32 res:16;
	u32 n:12;
	struct db_auth_at at[];
}__attribute__((packed));

#endif /* _DMDB_AUTH_H_ */
