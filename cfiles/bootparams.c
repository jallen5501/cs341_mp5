/* bootparams.h: screen info, etc. */
#include <sysapi.h>
/*
 * This refers to data set up by the setup-routine at boot-time
 * and added to by Tutor
 */
#include <params.h>

SysAPI2 *sysapi2 = 0;		/* master pointer to Tutor dispatch table */

void get_boot_params()
{
#ifdef NEED
  screen_info = SCREEN_INFO;
#endif
  sysapi2 = SYS_API;	/* establish master pointer */
}
