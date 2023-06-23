#ifndef _OS_H
#define _OS_H

#include "types.h"
#include "stdarg.h"

typedef char *outfun(char*,const char*,unsigned int);
int _Printf(outfun prout, char *arg, const char *fmt, va_list args);

typedef volatile unsigned long		vu32;	/* unsigned 32-bit */
#define	PHYS_TO_K1(x)	((u32)(x)|0xA0000000)	/* physical to kseg1 */
#define	IO_READ(addr)		(*(vu32 *)PHYS_TO_K1(addr))

#define OS_EVENT_SW1              0     /* CPU SW1 interrupt */
#define OS_EVENT_SW2              1     /* CPU SW2 interrupt */
#define OS_EVENT_CART             2     /* Cartridge interrupt: used by rmon */
#define OS_EVENT_COUNTER          3     /* Counter int: used by VI/Timer Mgr */
#define OS_EVENT_SP               4     /* SP task done interrupt */
#define OS_EVENT_SI               5     /* SI (controller) interrupt */
#define OS_EVENT_AI               6     /* AI interrupt */
#define OS_EVENT_VI               7     /* VI interrupt: used by VI/Timer Mgr */
#define OS_EVENT_PI               8     /* PI interrupt: used by PI Manager */
#define OS_EVENT_DP               9     /* DP full sync interrupt */
#define OS_EVENT_CPU_BREAK        10    /* CPU breakpoint: used by rmon */
#define OS_EVENT_SP_BREAK         11    /* SP breakpoint:  used by rmon */
#define OS_EVENT_FAULT            12    /* CPU fault event: used by rmon */
#define OS_EVENT_THREADSTATUS     13    /* CPU thread status: used by rmon */
#define OS_EVENT_PRENMI           14    /* Pre NMI interrupt */
#ifndef _FINALROM
#define OS_EVENT_RDB_READ_DONE    15    /* RDB read ok event: used by rmon */
#define OS_EVENT_RDB_LOG_DONE     16    /* read of log data complete */
#define OS_EVENT_RDB_DATA_DONE    17    /* read of hostio data complete */
#define OS_EVENT_RDB_REQ_RAMROM   18    /* host needs ramrom access */
#define OS_EVENT_RDB_FREE_RAMROM  19    /* host is done with ramrom access */
#define OS_EVENT_RDB_DBG_DONE     20
#define OS_EVENT_RDB_FLUSH_PROF   21
#define OS_EVENT_RDB_ACK_PROF     22
#endif

typedef u32 OSEvent;

/*
 * Structure for message
 */
typedef void *	OSMesg;

/*
 * Structure for message queue
 */
typedef s32	OSPri;
typedef s32	OSId;
typedef union	{ struct { f32 f_odd; f32 f_even; } f; f64 d; }	__OSfp;

typedef struct {
	u64	at, v0, v1, a0, a1, a2, a3;
	u64	t0, t1, t2, t3, t4, t5, t6, t7;
	u64	s0, s1, s2, s3, s4, s5, s6, s7;
	u64	t8, t9,         gp, sp, s8, ra;
	u64	lo, hi;
	u32	sr, pc, cause, badvaddr, rcp;
	u32	fpcsr;
	__OSfp	 fp0,  fp2,  fp4,  fp6,  fp8, fp10, fp12, fp14;
	__OSfp	fp16, fp18, fp20, fp22, fp24, fp26, fp28, fp30;
} __OSThreadContext;

typedef struct {
    u32 flag;
    u32 count;
    u64 time;
} __OSThreadprofile_s;

typedef struct OSThread_s {
	struct OSThread_s	*next;		/* run/mesg queue link */
	OSPri			priority;	/* run/mesg queue priority */
	struct OSThread_s	**queue;	/* queue thread is on */
	struct OSThread_s	*tlnext;	/* all threads queue link */
	u16			state;		/* OS_STATE_* */
	u16			flags;		/* flags for rmon */
	OSId			id;		/* id for debugging */
	int			fp;		/* thread has used fp unit */
	__OSThreadprofile_s     *thprof;        /* workarea for thread profiler */
	__OSThreadContext	context;	/* register/interrupt mask */
} OSThread;

typedef struct OSMesgQueue_s {
	OSThread	*mtqueue;	/* Queue to store threads blocked
					   on empty mailboxes (receive) */
	OSThread	*fullqueue;	/* Queue to store threads blocked
					   on full mailboxes (send) */
	s32		validCount;	/* Contains number of valid message */
	s32		first;		/* Points to first valid message */
	s32		msgCount;	/* Contains total # of messages */
	OSMesg		*msg;		/* Points to message buffer array */
} OSMesgQueue;

/* Message operations */

extern void		osCreateMesgQueue(OSMesgQueue *, OSMesg *, s32);
//extern s32		osSendMesg(OSMesgQueue *, OSMesg, s32);
//extern s32		osJamMesg(OSMesgQueue *, OSMesg, s32);
extern s32		osRecvMesg(OSMesgQueue *, OSMesg *, s32);
extern void		osCreateThread(OSThread *, OSId, void (*)(void *),
				       void *, void *, OSPri);

/* Event operations */

extern void		osSetEventMesg(OSEvent, OSMesgQueue *, OSMesg);
extern void		osStartThread(OSThread *);
extern void		osStopThread(OSThread *);

extern void		osWritebackDCacheAll(void);

void osSetTime(u64 time);
extern void		osViBlack(u8);
extern void		osViRepeatLine(u8);
extern void		osViSwapBuffer(void *);

/*
*************************************************************************
 * Peripheral Interface (PI) Registers
*/
#define PI_BASE_REG		0x04600000

/* PI DRAM address (R/W): [23:0] starting RDRAM address */
#define PI_DRAM_ADDR_REG	(PI_BASE_REG+0x00)	/* DRAM address */

/* PI pbus (cartridge) address (R/W): [31:0] starting AD16 address */
#define PI_CART_ADDR_REG	(PI_BASE_REG+0x04)

/* PI read length (R/W): [23:0] read data length */
#define PI_RD_LEN_REG		(PI_BASE_REG+0x08)

/* PI write length (R/W): [23:0] write data length */
#define PI_WR_LEN_REG		(PI_BASE_REG+0x0C)

/*
 * PI status (R): [0] DMA busy, [1] IO busy, [2], error
 *           (W): [0] reset controller (and abort current op), [1] clear intr
 */
#define PI_STATUS_REG		(PI_BASE_REG+0x10)

#define PI_STATUS_DMA_BUSY  (1 << 0)
#define PI_STATUS_IO_BUSY   (1 << 1)
#define PI_STATUS_ERROR     (1 << 2)

#define WAIT_ON_IOBUSY(stat)                                                                \
    while (stat = IO_READ(PI_STATUS_REG), stat & (PI_STATUS_IO_BUSY | PI_STATUS_DMA_BUSY)); (void)0                                                                    \
    
#endif