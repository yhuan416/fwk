#include "t_osal_mem.h"

static int suite_init(void) { return 0; }
static int suite_clean(void) { return 0; }

CU_pSuite mem_suite = NULL;

void MemAddTests(void)
{
    mem_suite = CU_add_suite_with_setup_and_teardown("MEM", suite_init, suite_clean, NULL, NULL);

    t_osal_mem_001();
    t_osal_mem_002();
    t_osal_mem_003();
}
