/* params supplied by boot to main, and from Tutor to download
   library.  Used in boot, saconsole.c (to init cursor on screen)
   and devio.c (to provide sysapi pointer to download lib). */
#ifndef PARAMS_H
#define PARAMS_H

/* from segment.h */
#define KERNEL_CS	0x10
#define KERNEL_DS	0x18

/* Tutor build parameters of interest to download lib */
#define DEFAULT_HOSTLINE_DEV 2	/* COM2 */
#define DEFAULT_BAUDRATE 9600   /* COM lines */

/* this was 0x105000 in linux, but Tutor fits in lower mem-- */
#define PARAM	0x55000
/*
 * This info is set up by the setup-routine at boot-time
 * Note similar def's in boot/compressed/misc.c
 */
#define EXT_MEM_K (*(unsigned short *) (PARAM+2))
#define DRIVE_INFO (*(struct drive_info_struct *) (PARAM+0x80))
#define SCREEN_INFO (*(struct screen_info *) (PARAM+0))

/* these are for download lib to find Tutor entry points */
#define TUTOR_CONSOLE_DEV (*(int *) (PARAM+0x10))
#define TUTOR_HOSTLINE_DEV (*(int *)(PARAM+0x14))
#define TUTOR_DEVTAB (*(struct dev_struct **) (PARAM+0x18))
/* new master pointer, obsoleting above three-- */
#define TUTOR_SYSAPI (*(SysAPI **) (PARAM+0x100))
#define SYS_API (*(SysAPI2 **) (PARAM+0x104))
#endif
