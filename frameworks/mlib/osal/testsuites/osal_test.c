#include "common.h"

void MemAddTests(void);
void TaskAddTests(void);
void SemAddTests(void);
void MiscAddTests(void);
void MqAddTests(void);

void AddTests(void)
{
    // mem
    MemAddTests();

    // task
    TaskAddTests();

    // mutex

    // sem
    SemAddTests();

    // mq
    MqAddTests();

    // event

    // misc
    MiscAddTests();
}

int osal_test_entry(void *arg)
{
    int ret = 0;
    if (CU_initialize_registry())
    {
        printf("\nInitialization of Test Registry failed.");
    }
    else
    {
        AddTests();
        CU_basic_set_mode(CU_BRM_VERBOSE);
        CU_set_error_action(CUEA_FAIL);
        ret = CU_basic_run_tests();
        printf("\nTests completed with return value %d.\n", ret);
        CU_cleanup_registry();
    }
    return ret;
}

#if defined(__linux__)
int main(int argc, char const *argv[])
{
    return osal_test_entry(NULL);
}
#endif
