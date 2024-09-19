#include <math.h>
#include "ezlopi_util_trace.h"

// #include "ezlopi_core_timer.h"
#include "ezlopi_core_cjson_macros.h"
#include "ezlopi_core_valueformatter.h"
#include "ezlopi_core_device_value_updated.h"
#include "ezlopi_core_errors.h"

#include "ezlopi_hal_adc.h"

#include "ezlopi_cloud_items.h"
#include "ezlopi_cloud_constants.h"

#include "sensor_0056_ADC_Force_Sensitive_Resistor.h"
#include "EZLOPI_USER_CONFIG.h"

//------------------------------------------------------------------------------------------------------------------------------
static ezlopi_error_t __0056_prepare(void *arg);
static ezlopi_error_t __0056_init(l_ezlopi_item_t *item);
static ezlopi_error_t __0056_get_cjson_value(l_ezlopi_item_t *item, void *arg);
static ezlopi_error_t __0056_notify(l_ezlopi_item_t *item);
static float Calculate_GramForce(float Vout);
static void __prepare_device_cloud_properties(l_ezlopi_device_t *device, cJSON *cj_device);
static void __prepare_item_cloud_properties(l_ezlopi_item_t *item, cJSON *cj_device, void *user_data);
//------------------------------------------------------------------------------------------------------------------------------
ezlopi_error_t sensor_0056_ADC_Force_Sensitive_Resistor(e_ezlopi_actions_t action, l_ezlopi_item_t *item, void *arg, void *user_arg)
{
    ezlopi_error_t ret = EZPI_SUCCESS;
    switch (action)
    {
    case EZLOPI_ACTION_PREPARE:
    {
        __0056_prepare(arg);
        break;
    }
    case EZLOPI_ACTION_INITIALIZE:
    {
        __0056_init(item);
        break;
    }
    case EZLOPI_ACTION_HUB_GET_ITEM:
    case EZLOPI_ACTION_GET_EZLOPI_VALUE:
    {
        __0056_get_cjson_value(item, arg);
        break;
    }
    case EZLOPI_ACTION_NOTIFY_1000_MS:
    {
        __0056_notify(item);
        break;
    }
    default:
    {
        break;
    }
    }
    return ret;
}

//------------------------------------------------------------------------------------------------------
static void __prepare_device_cloud_properties(l_ezlopi_device_t *device, cJSON *cj_device)
{
    device->cloud_properties.category = category_level_sensor;
    device->cloud_properties.subcategory = subcategory_not_defined;
    device->cloud_properties.device_type = dev_type_sensor;
    device->cloud_properties.info = NULL;
    device->cloud_properties.device_type_id = NULL;
}
static void __prepare_item_cloud_properties(l_ezlopi_item_t *item, cJSON *cj_device, void *user_data)
{
    item->cloud_properties.has_getter = true;
    item->cloud_properties.has_setter = false;
    item->cloud_properties.item_name = ezlopi_item_name_applied_force_on_sensor;
    item->cloud_properties.value_type = value_type_force;
    item->cloud_properties.show = true;
    item->cloud_properties.scale = scales_newton;
    item->cloud_properties.item_id = ezlopi_cloud_generate_item_id();

    CJSON_GET_VALUE_DOUBLE(cj_device, ezlopi_dev_type_str, item->interface_type); // _max = 10
    CJSON_GET_VALUE_GPIO(cj_device, ezlopi_gpio_str, item->interface.adc.gpio_num);
    item->interface.adc.resln_bit = 3; // ADC 12_bit

    // passing the custom data_structure
    item->is_user_arg_unique = true;
    item->user_arg = user_data;
}

//----------------------------------------------------
static ezlopi_error_t __0056_prepare(void *arg)
{
    ezlopi_error_t ret = EZPI_ERR_PREP_DEVICE_PREP_FAILED;
    s_ezlopi_prep_arg_t *device_prep_arg = (s_ezlopi_prep_arg_t *)arg;
    if (device_prep_arg && (NULL != device_prep_arg->cjson_device))
    {
        fsr_t *fsr_struct = (fsr_t *)ezlopi_malloc(__FUNCTION__, sizeof(fsr_t));
        if (NULL != fsr_struct)
        {
            memset(fsr_struct, 0, sizeof(fsr_t));

            l_ezlopi_device_t *FSR_device = ezlopi_device_add_device(device_prep_arg->cjson_device, NULL, 0);
            if (FSR_device)
            {
                __prepare_device_cloud_properties(FSR_device, device_prep_arg->cjson_device);
                l_ezlopi_item_t *FSR_item = ezlopi_device_add_item_to_device(FSR_device, sensor_0056_ADC_Force_Sensitive_Resistor);
                if (FSR_item)
                {
                    __prepare_item_cloud_properties(FSR_item, device_prep_arg->cjson_device, fsr_struct);
                    ret = EZPI_SUCCESS;
                }
                else
                {
                    ezlopi_device_free_device(FSR_device);
                    ezlopi_free(__FUNCTION__, fsr_struct);
                }
            }
            else
            {
                ezlopi_free(__FUNCTION__, fsr_struct);
            }
        }
    }
    return ret;
}

static ezlopi_error_t __0056_init(l_ezlopi_item_t *item)
{
    ezlopi_error_t ret = EZPI_ERR_INIT_DEVICE_FAILED;
    if (item)
    {
        fsr_t *fsr_struct = (fsr_t *)item->user_arg;
        if (fsr_struct)
        {
            if (GPIO_IS_VALID_GPIO(item->interface.gpio.gpio_in.gpio_num))
            {
                if (EZPI_SUCCESS == ezlopi_adc_init(item->interface.adc.gpio_num, item->interface.adc.resln_bit))
                {
                    ret = EZPI_SUCCESS;
                }
            }
        }
    }

    return ret;
}

static ezlopi_error_t __0056_get_cjson_value(l_ezlopi_item_t *item, void *arg)
{
    ezlopi_error_t ret = EZPI_FAILED;

    if (item && arg)
    {
        cJSON *cj_result = (cJSON *)arg;
        fsr_t *fsr_struct = (fsr_t *)item->user_arg;
        if (fsr_struct)
        {
            ezlopi_valueformatter_float_to_cjson(cj_result, fsr_struct->fsr_value, scales_newton);
            ret = EZPI_SUCCESS;
        }
    }

    return ret;
}

static ezlopi_error_t __0056_notify(l_ezlopi_item_t *item)
{
    ezlopi_error_t ret = EZPI_FAILED;
    if (item)
    {
        fsr_t *fsr_struct = (fsr_t *)item->user_arg;
        if (fsr_struct)
        {
            s_ezlopi_analog_data_t ezlopi_analog_data = { .value = 0, .voltage = 0 };
            ezlopi_adc_get_adc_data(item->interface.adc.gpio_num, &ezlopi_analog_data);
            float Vout = (ezlopi_analog_data.voltage) / 1000.0f; // millivolt -> voltage

            // New Force[N] is :
            float new_force = 0.0098f * Calculate_GramForce(Vout);
            // TRACE_E(" Force[N]: %.4f", fsr_struct->fsr_value);
            if (new_force != fsr_struct->fsr_value)
            {
                fsr_struct->fsr_value = new_force;
                ezlopi_device_value_updated_from_device_broadcast(item);
            }
            ret = EZPI_SUCCESS;
        }
    }
    return ret;
}
//-------------------------------------------------------------------------------------------
static float Calculate_GramForce(float Vout)
{
    float Rs = 0, gramForce = 0;
    // TRACE_E("Voltage [V]: %.4f", Vout);
    if ((Vout - 0.142f) > 0.1f)
    {
        // calculate the 'Rs[kOhm]' corresponding to 'Vout' using equation(A):
        Rs = (((FSR_Vin / Vout) - 1) * FSR_Rout) / (1000.0f); // Ohm -> KOhm
    }
    else
    {
        Rs = 750; // kOhm
    }
    // TRACE_E("FSR value [Rs Kohm]: %.4f", Rs);

    // calculating the Force(g)
    if (Rs < 250)
    {
        // We choose Rs threshold below 250KOhm because:
        // The senor activation causes, imidiate resistance drop below 200KOhm
        // So, filter the 'Rs' range(250k to 2K)
        Rs = (Rs < 2) ? 2 : Rs;
        gramForce = (float)pow(10, (((float)log10(Rs)) - b_coeff_FSR) / m_slope_FSR);
    }
    else
    { //
        gramForce = 0;
    }
    // according to testing ; correct gramForce(G) is :
    gramForce = gramForce * FSR_correction_factor;
    // TRACE_E("GramForce[gN]: %.4f  ", gramForce);
    return gramForce;
}