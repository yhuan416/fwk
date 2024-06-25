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
    char *buffer = malloc(1024);
    CU_ASSERT_PTR_NOT_NULL(buffer);
    if (buffer == NULL) {
        CU_SKIP_IF(1);
        printf("malloc(1024) failed\n");
    }
}

/* run at the end of each test */
CU_TEST_TEARDOWN() {

}

static void dump_test_counts1(void)
{
    long ret = 0;
    unsigned long counts = 0;

    ret = mmem_dump(MMEM_DUMP_CMD_COUNTS, 0, &counts, sizeof(counts));
    CU_ASSERT_EQUAL(ret, MMEM_DUMP_RET_OK);
    CU_ASSERT_EQUAL(counts, 1);
}

static void dump_test_counts2(void)
{
    long ret = 0;
    unsigned long counts = 0;

    ret = mmem_dump(MMEM_DUMP_CMD_COUNTS, 0, &counts, sizeof(counts));
    CU_ASSERT_EQUAL(ret, MMEM_DUMP_RET_OK);
    CU_ASSERT_EQUAL(counts, 2);
}

static void dump_test_counts3(void)
{
    long ret = 0;
    unsigned long counts = 0;

    ret = mmem_dump(MMEM_DUMP_CMD_COUNTS, 0, &counts, sizeof(counts));
    CU_ASSERT_EQUAL(ret, MMEM_DUMP_RET_OK);
    CU_ASSERT_EQUAL(counts, 3);
}

CUNIT_CI_RUN(CU_MAIN_EXE_NAME,
             CUNIT_CI_TEST(dump_test_counts1),
             CUNIT_CI_TEST(dump_test_counts2),
             CUNIT_CI_TEST(dump_test_counts3)
);
