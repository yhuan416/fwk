#include "common.h"

#include <unistd.h>

/* run at the start of each suite */
CU_SUITE_SETUP() {
    return CUE_SUCCESS;
}

/* run at the end of the suite */
CU_SUITE_TEARDOWN() {
    return CUE_SUCCESS;
}

/* run at the start of each test */
CU_TEST_SETUP() {

}

/* run at the end of each test */
CU_TEST_TEARDOWN() {

}

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

CUNIT_CI_RUN(CU_MAIN_EXE_NAME,
             CUNIT_CI_TEST(uptime)
);
