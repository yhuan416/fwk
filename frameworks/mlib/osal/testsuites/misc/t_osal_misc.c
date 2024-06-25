#include "t_osal_misc.h"

static int suite_init(void) { return 0; }
static int suite_clean(void) { return 0; }

CU_pSuite misc_suite = NULL;

void MiscAddTests(void)
{
    misc_suite = CU_add_suite_with_setup_and_teardown("MISC", suite_init, suite_clean, NULL, NULL);

    t_osal_misc_001();
    t_osal_misc_002();
}
