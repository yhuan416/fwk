#include "common.h"

/* run at the start of each suite */
CU_SUITE_SETUP() {
    return CUE_SUCCESS;
}

/* run at the end of the suite */
CU_SUITE_TEARDOWN() {
    mmem_free_all();
    return CUE_SUCCESS;
}

/* run at the start of each test */
CU_TEST_SETUP() {

}

/* run at the end of each test */
CU_TEST_TEARDOWN() {

}

static void dump_test_cmd_invalid_cmd(void)
{
    long ret = 0;
    unsigned long counts = 0;

    // invalid cmd
    ret = mmem_dump(0, 0, &counts, sizeof(unsigned long));
    CU_ASSERT_EQUAL(ret, MMEM_DUMP_RET_INVALID_CMD);
}

CUNIT_CI_RUN(CU_MAIN_EXE_NAME,
             CUNIT_CI_TEST(dump_test_cmd_invalid_cmd)
);
