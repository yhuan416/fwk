#include "common.h"

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

static void free_all_test1(void)
{
    long ret;
    char *buffer;
    char *buffer2;
    unsigned long counts;

    // malloc 10 bytes
    buffer = malloc(1024);
    CU_ASSERT_PTR_NOT_NULL(buffer);

    // calloc 10 bytes
    buffer2 = calloc(1, 1024);
    CU_ASSERT_PTR_NOT_NULL(buffer2);

    // get counts, should be 2
    ret = mmem_dump(MMEM_DUMP_CMD_COUNTS, 0, (void *)&counts, sizeof(counts));
    CU_ASSERT_EQUAL(ret, MMEM_DUMP_RET_OK);
    CU_ASSERT_EQUAL(counts, 2);

    // free all
    mmem_free_all();

    // get counts, should be 0
    ret = mmem_dump(MMEM_DUMP_CMD_COUNTS, 0, (void *)&counts, sizeof(counts));
    CU_ASSERT_EQUAL(ret, MMEM_DUMP_RET_OK);
    CU_ASSERT_EQUAL(counts, 0);
}

CUNIT_CI_RUN(CU_MAIN_EXE_NAME,
             CUNIT_CI_TEST(free_all_test1)
);
