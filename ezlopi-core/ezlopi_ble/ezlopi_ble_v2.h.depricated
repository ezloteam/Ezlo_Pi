#ifndef __EZLOPI_BLE_V2_H__
#define __EZLOPI_BLE_V2_H__

#define CHECK_PRINT_ERROR(x, msg)                                    \
    {                                                                \
        if (x)                                                       \
        {                                                            \
            TRACE_E("%s %s: %s", __func__, msg, esp_err_to_name(x)); \
            return;                                                  \
        }                                                            \
    }

void ezlopi_ble_v2_init(void);

#endif //  __ EZLOPI_BLE_V2_H__
