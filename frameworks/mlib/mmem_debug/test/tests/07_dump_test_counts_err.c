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

// invalid buffer(null)
static void dump_test_counts_err_null_buff(void)
{
    long ret = 0;
    ret = mmem_dump(MMEM_DUMP_CMD_COUNTS, 0, NULL, sizeof(unsigned long));
    CU_ASSERT_EQUAL(ret, MMEM_DUMP_RET_EMPTY_BUF);
}

// invalid buffer size(too small)
static void dump_test_counts_err_small_size(void)
{
    long ret = 0;
    unsigned long counts = 0;
    ret = mmem_dump(MMEM_DUMP_CMD_COUNTS, 0, &counts, 1);
    CU_ASSERT_EQUAL(ret, MMEM_DUMP_RET_BUF_SIZE_TOO_SMALL);
}

CUNIT_CI_RUN(CU_MAIN_EXE_NAME,
             CUNIT_CI_TEST(dump_test_counts_err_null_buff),
             CUNIT_CI_TEST(dump_test_counts_err_small_size)
);
