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

static void dump_test_mmem_block_info_err_null_buffer(void)
{
    long ret;
    ret = mmem_dump(MMEM_DUMP_CMD_MMEM_BLOCK_INFO, 10, NULL, sizeof(mmem_block_info_t) * 10);
    CU_ASSERT_EQUAL(ret, MMEM_DUMP_RET_EMPTY_BUF);
}

static void dump_test_mmem_block_info_err_small_size(void)
{
    long ret;
    mmem_block_info_t mmem_block_info[10] = {0};
    ret = mmem_dump(MMEM_DUMP_CMD_MMEM_BLOCK_INFO, 10, mmem_block_info, sizeof(mmem_block_info_t) - 1);
    CU_ASSERT_EQUAL(ret, MMEM_DUMP_RET_BUF_SIZE_TOO_SMALL);
}

static void dump_test_mmem_block_info_err_set_counts_0(void)
{
    long ret;
    mmem_block_info_t mmem_block_info[10] = {0};
    ret = mmem_dump(MMEM_DUMP_CMD_MMEM_BLOCK_INFO, 0, mmem_block_info, sizeof(mmem_block_info_t) * 10);
    CU_ASSERT_EQUAL(ret, 0);
}

static void dump_test_mmem_block_info_err_set_counts_1(void)
{
    long ret;
    mmem_block_info_t mmem_block_info[10] = {0};
    ret = mmem_dump(MMEM_DUMP_CMD_MMEM_BLOCK_INFO, 1, mmem_block_info, sizeof(mmem_block_info_t) * 10);
    CU_ASSERT_EQUAL(ret, 1);
}

CUNIT_CI_RUN(CU_MAIN_EXE_NAME,
             CUNIT_CI_TEST(dump_test_mmem_block_info_err_null_buffer),
             CUNIT_CI_TEST(dump_test_mmem_block_info_err_small_size),
             CUNIT_CI_TEST(dump_test_mmem_block_info_err_set_counts_0),
             CUNIT_CI_TEST(dump_test_mmem_block_info_err_set_counts_1)
);
