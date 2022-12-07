#ifndef __GATTS_TEST_H__
#define __GATTS_TEST_H__

#define GATTS_CHECK_PRINT_ERROR(x, msg)                              \
    {                                                                \
        if (x)                                                       \
        {                                                            \
            TRACE_E("%s %s: %s", __func__, msg, esp_err_to_name(x)); \
            return;                                                  \
        }                                                            \
    }

#define TEST_DEVICE_NAME "gatts_test_name"

#endif // __GATTS_TEST_H__
