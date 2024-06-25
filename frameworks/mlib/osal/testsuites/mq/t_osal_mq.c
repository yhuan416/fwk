#include "t_osal_mq.h"

static int suite_init(void) { return 0; }
static int suite_clean(void) { return 0; }

CU_pSuite mq_suite = NULL;

void MqAddTests(void)
{
    mq_suite = CU_add_suite_with_setup_and_teardown("MQ", suite_init, suite_clean, NULL, NULL);

    t_osal_mq_001();
}
