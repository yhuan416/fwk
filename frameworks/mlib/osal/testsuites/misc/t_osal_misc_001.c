#include "t_osal_misc.h"

#include <unistd.h>

static void uptime(void)
{
    uint64_t uptime = osal_uptime();
    CU_ASSERT(uptime > 0);

    sleep(1);
    uint64_t uptime2 = osal_uptime();
    CU_ASSERT(uptime2 = uptime + 1);

    sleep(1);
    uint64_t uptime3 = osal_uptime();
    CU_ASSERT(uptime3 = uptime + 2);
}

void t_osal_misc_001(void)
{
    CU_add_test(misc_suite, "uptime", uptime);
}
