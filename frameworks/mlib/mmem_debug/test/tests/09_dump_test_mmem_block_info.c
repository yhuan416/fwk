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

static void _dump_mmem_block_info(mmem_block_info_t *buf, int counts)
{
    int i;
    for (i = 0; i < counts; i++) {
        CU_ASSERT_EQUAL(buf[i].size, 1024);
        printf("\n\tmmem_block_info[%d]: \n", i);
        printf("\t\tsize: %lu\n", buf[i].size);
        printf("\t\tt-size: %lu\n", buf[i].total_size);
        printf("\t\tfile: %s[%lu]\n", buf[i].file, buf[i].line);
    }
}

static void dump_test_mmem_block_info_1(void)
{
    long ret;
    mmem_block_info_t mmem_block_info[10] = {0};

    ret = mmem_dump(MMEM_DUMP_CMD_MMEM_BLOCK_INFO, 10, (void *)mmem_block_info, sizeof(mmem_block_info_t) * 10);
    CU_ASSERT_EQUAL(ret, 1);
    _dump_mmem_block_info(mmem_block_info, ret);
}

static void dump_test_mmem_block_info_2(void)
{
    long ret;
    mmem_block_info_t mmem_block_info[10] = {0};

    ret = mmem_dump(MMEM_DUMP_CMD_MMEM_BLOCK_INFO, 10, (void *)mmem_block_info, sizeof(mmem_block_info_t) * 10);
    CU_ASSERT_EQUAL(ret, 2);
    _dump_mmem_block_info(mmem_block_info, ret);
}

static void dump_test_mmem_block_info_3(void)
{
    long ret;
    mmem_block_info_t mmem_block_info[10] = {0};

    ret = mmem_dump(MMEM_DUMP_CMD_MMEM_BLOCK_INFO, 10, (void *)mmem_block_info, sizeof(mmem_block_info_t) * 10);
    CU_ASSERT_EQUAL(ret, 3);
    _dump_mmem_block_info(mmem_block_info, ret);
}

CUNIT_CI_RUN(CU_MAIN_EXE_NAME,
             CUNIT_CI_TEST(dump_test_mmem_block_info_1),
             CUNIT_CI_TEST(dump_test_mmem_block_info_2),
             CUNIT_CI_TEST(dump_test_mmem_block_info_3)
);
