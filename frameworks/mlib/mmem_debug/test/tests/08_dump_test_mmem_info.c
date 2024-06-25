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

static char *buffer = NULL;

/* run at the start of each test */
CU_TEST_SETUP() {
    buffer = malloc(1024);
    CU_ASSERT_PTR_NOT_NULL(buffer);
    if (buffer == NULL) {
        CU_SKIP_IF(1);
        printf("malloc(1024) failed\n");
    }
}

/* run at the end of each test */
CU_TEST_TEARDOWN() {
    static int i = 1;
    int malloc_size = 0;

    free(buffer);

    /**
     * malloc 1024, 2048, 3072, 4096, 5120, 6144, 7168, 8192, 9216, 10240
     * and free them in order.
     * 
     * will update ${max_active_size}.
    */
    malloc_size = 1024 * (++i);
    buffer = malloc(malloc_size);
    CU_ASSERT_PTR_NOT_NULL(buffer);
    if (buffer == NULL) {
        CU_SKIP_IF(1);
        printf("malloc(%d) failed\n", malloc_size);
    }

    free(buffer);
}

static void dump_test_mmem_info_A1024_MA1024(void)
{
    long ret = 0;
    mmem_info_t mmem_info = {0};

    ret = mmem_dump(MMEM_DUMP_CMD_MMEM_INFO, 0, &mmem_info, sizeof(mmem_info_t));
    CU_ASSERT_EQUAL(ret, MMEM_DUMP_RET_OK);
    CU_ASSERT_EQUAL(mmem_info.counts, 1);
    CU_ASSERT_EQUAL(mmem_info.active_size, 1024);
    CU_ASSERT_EQUAL(mmem_info.max_active_size, 1024);
}

static void dump_test_mmem_info_A1024_MA2048(void)
{
    long ret = 0;
    mmem_info_t mmem_info = {0};

    ret = mmem_dump(MMEM_DUMP_CMD_MMEM_INFO, 0, &mmem_info, sizeof(mmem_info_t));
    CU_ASSERT_EQUAL(ret, MMEM_DUMP_RET_OK);
    CU_ASSERT_EQUAL(mmem_info.counts, 1);
    CU_ASSERT_EQUAL(mmem_info.active_size, 1024);
    CU_ASSERT_EQUAL(mmem_info.max_active_size, 2048);
}

static void dump_test_mmem_info_A1024_MA3072(void)
{
    long ret = 0;
    mmem_info_t mmem_info = {0};

    ret = mmem_dump(MMEM_DUMP_CMD_MMEM_INFO, 0, &mmem_info, sizeof(mmem_info_t));
    CU_ASSERT_EQUAL(ret, MMEM_DUMP_RET_OK);
    CU_ASSERT_EQUAL(mmem_info.counts, 1);
    CU_ASSERT_EQUAL(mmem_info.active_size, 1024);
    CU_ASSERT_EQUAL(mmem_info.max_active_size, 3072);
}

CUNIT_CI_RUN(CU_MAIN_EXE_NAME,
             CUNIT_CI_TEST(dump_test_mmem_info_A1024_MA1024),
             CUNIT_CI_TEST(dump_test_mmem_info_A1024_MA2048),
             CUNIT_CI_TEST(dump_test_mmem_info_A1024_MA3072)
);
