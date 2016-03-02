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
//  Filename:       typedef.h
//  Author:         Terry Yang <ybable@gmail.com>
//  Creation Date:  19/08/2011
//  Change history:
//
//  Description:
//	TODO
//
*****************************************************************************/

#ifndef _TYPEDEF_H_
#define _TYPEDEF_H_

typedef unsigned char	u8;
typedef signed char	s8;
typedef unsigned short	u16;
typedef signed short	s16;
typedef unsigned int	u32;
typedef signed int	s32;
typedef unsigned long long	u64;
typedef signed long long	s64;

#include <stdlib.h>

#define init_buf(b, s) {if (b == NULL)b = malloc(s);if (b == NULL)return NULL;}

#endif /* _TYPEDEF_H_ */
