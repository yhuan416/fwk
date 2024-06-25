#include "t_osal_sem.h"

static int suite_init(void) { return 0; }
static int suite_clean(void) { return 0; }

CU_pSuite sem_suite = NULL;

void SemAddTests(void)
{
    sem_suite = CU_add_suite_with_setup_and_teardown("SEM", suite_init, suite_clean, NULL, NULL);

    t_osal_sem_001();
    t_osal_sem_002();
    t_osal_sem_003();
}
