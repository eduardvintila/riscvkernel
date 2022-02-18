/*
 * SPDX-License-Identifier: BSD-2-Clause
 *
 * Copyright (c) 2019 Western Digital Corporation or its affiliates.
 *
 * Authors:
 *   Anup Patel <anup.patel@wdc.com>
 */

#include <sbi/sbi_ecall_interface.h>

#ifndef __SBI_ECALLS_H__
#define __SBI_ECALLS_H__

struct sbiret {
    long error;
    long value;
};

static inline struct sbiret sbi_ecall(unsigned long __eid, unsigned long __fid, unsigned long __a0, unsigned long __a1)
{
    register unsigned long a0 asm("a0") = __a0;
    register unsigned long a1 asm("a1") = __a1;
    register unsigned long a6 asm("a6") = __fid;
    register unsigned long a7 asm("a7") = __eid;
    asm volatile("ecall"
                : "+r"(a0), "+r"(a1)
                : "r"(a6), "r"(a7)
                : "memory");

    return (struct sbiret) {.error = a0, .value = a1};
}


#define SBI_ECALL_0(__eid, __fid) sbi_ecall(__eid, __fid, 0, 0)
#define SBI_ECALL_1(__eid, __fid, __a0) sbi_ecall(__eid, __fid, __a0, 0)
#define SBI_ECALL_2(__eid, __fid, __a0, __a1) sbi_ecall(__eid, __fid, __a0, __a1)


static inline void sbi_console_putc(char c)
{
    SBI_ECALL_1(SBI_EXT_0_1_CONSOLE_PUTCHAR, 0, (c));
}

static inline struct sbiret sbi_get_spec_version(void)
{
    return SBI_ECALL_0(SBI_EXT_BASE, SBI_EXT_BASE_GET_SPEC_VERSION);
}


#endif
