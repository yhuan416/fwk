#include "t_osal_task.h"

static int suite_init(void) { return 0; }
static int suite_clean(void) { return 0; }

CU_pSuite task_suite = NULL;

void TaskAddTests(void)
{
    task_suite = CU_add_suite_with_setup_and_teardown("TASK", suite_init, suite_clean, NULL, NULL);

    t_osal_task_001();
    t_osal_task_002();
    t_osal_task_003();
}
