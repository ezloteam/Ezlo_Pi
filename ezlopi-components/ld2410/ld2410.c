#include "ld2410.h"
#include "ezlopi_util_trace.h"

esp_err_t ld2410_setup(s_ezlopi_uart_t uart_settings)
{
    TRACE_I("Connect LD2410 radar TX to GPIO: %d", uart_settings.rx);
    TRACE_I("Connect LD2410 radar RX to GPIO: %d", uart_settings.tx);
    TRACE_I("LD2410 radar sensor initialising:");
    if (ld2410_begin(true, uart_settings))
    {
        TRACE_I("OK");

        ld2410_settings_t settings =
        {
            .max_still_distance = DISTANCE_525CM,
            .max_move_distance = DISTANCE_525CM,
            .min_still_distance = DISTANCE_0CM,
            .min_move_distance = DISTANCE_0CM,
            .no_one_duration = 5 };
        ld2410_set_template(CUSTOM_TEMPLATE, &settings);
        return ESP_OK;
    }
    else
    {
        TRACE_I("not connected");
    }
    return ESP_FAIL;
}

esp_err_t ld2410_set_template(ld2410_template_t template, ld2410_settings_t *p_settings)
{
    // esp_err_t ret = ESP_OK;

    ld2410_settings_t settings = {
        .max_still_distance = DISTANCE_600CM,
        .max_move_distance = DISTANCE_600CM,
        .no_one_duration = 5,
    };
    uint8_t move_sensitivity[9] = { 20, 20, 20, 20, 20, 20, 20, 20, 20 };  // Gate 0 to 8 sensitivity : sensitivity range 0-100
    uint8_t still_sensitivity[9] = { 20, 20, 20, 20, 20, 20, 20, 20, 20 }; // Gate 0 to 8 sensitivity : sensitivity range 0-100

    switch (template)
    {
    case SLEEP_TEMPLATE_CLOSE_RANGE:
    {
        settings.no_one_duration = 180; // 180 seconds
        settings.max_still_distance = DISTANCE_225CM;
        settings.max_move_distance = DISTANCE_225CM;
        break;
    }

    case SLEEP_TEMPLATE_MID_RANGE:
    {
        settings.no_one_duration = 180; // 180 seconds
        settings.max_still_distance = DISTANCE_375CM;
        settings.max_move_distance = DISTANCE_375CM;
        break;
    }

    case SLEEP_TEMPLATE_LONG_RANGE:
    {
        settings.no_one_duration = 180; // 180 seconds
        settings.max_still_distance = DISTANCE_600CM;
        settings.max_move_distance = DISTANCE_600CM;
        break;
    }

    case MOVEMENT_TEMPLATE_CLOSE_RANGE:
    {
        settings.no_one_duration = 2; // 2 seconds
        settings.max_still_distance = DISTANCE_75CM;
        settings.max_move_distance = DISTANCE_225CM;
        break;
    }

    case MOVEMENT_TEMPLATE_MID_RANGE:
    {
        settings.no_one_duration = 2; // 2 seconds
        settings.max_still_distance = DISTANCE_75CM;
        settings.max_move_distance = DISTANCE_375CM;
        break;
    }

    case MOVEMENT_TEMPLATE_LONG_RANGE:
    {
        settings.no_one_duration = 2; // 2 seconds
        settings.max_still_distance = DISTANCE_75CM;
        settings.max_move_distance = DISTANCE_600CM;
        break;
    }

    case BASIC_TEMPLATE_CLOSE_RANGE:
    {
        settings.no_one_duration = 5; // 5 seconds
        settings.max_still_distance = DISTANCE_225CM;
        settings.max_move_distance = DISTANCE_225CM;
        break;
    }

    case BASIC_TEMPLATE_MID_RANGE:
    {
        settings.no_one_duration = 5; // 5 seconds
        settings.max_still_distance = DISTANCE_375CM;
        settings.max_move_distance = DISTANCE_375CM;
        break;
    }

    case BASIC_TEMPLATE_LONG_RANGE:
    {
        settings.no_one_duration = 5; // 5 seconds
        settings.max_still_distance = DISTANCE_600CM;
        settings.max_move_distance = DISTANCE_600CM;
        break;
    }

    case CUSTOM_TEMPLATE:
    {
        if (p_settings)
        {
            settings.no_one_duration = p_settings->no_one_duration;
            settings.max_still_distance = p_settings->max_still_distance;
            settings.max_move_distance = p_settings->max_move_distance;
            for (uint8_t i = 0; i < p_settings->min_still_distance; i++)
            {
                still_sensitivity[i] = 100;
            }
            for (uint8_t i = 8; i > p_settings->max_still_distance; i--)
            {
                still_sensitivity[i] = 100;
            }
            for (uint8_t i = 0; i < p_settings->min_move_distance; i++)
            {
                move_sensitivity[i] = 100;
            }
            for (uint8_t i = 8; i > p_settings->max_move_distance; i--)
            {
                move_sensitivity[i] = 100;
            }
        }

        break;
    }

    default:
    {
#ifdef CONFIG_EZPI_UTIL_TRACE_EN
        TRACE_E("unknown template");
#endif
        return ESP_FAIL;
        break;
    }
    }

#if TEST_SETTINGS
#ifdef CONFIG_EZPI_UTIL_TRACE_EN
    TRACE_I("no_one_duration : %d max_still_distance : %d max_move_distance : %d", settings.no_one_duration, settings.max_still_distance, settings.max_move_distance);
    TRACE_I("still_sensitivity : ");
#endif
    for (uint8_t i = 0; i < 9; i++)
    {
        TRACE_I("%d ", still_sensitivity[i]);
    }
#ifdef CONFIG_EZPI_UTIL_TRACE_EN
    TRACE_I("move_sensitivity : ");
#endif
    for (uint8_t i = 0; i < 9; i++)
    {
        TRACE_I("%d ", still_sensitivity[i]);
    }
#endif

    if (!ld2410_set_max_values(settings.max_move_distance, settings.max_still_distance, settings.no_one_duration)) // (max_moving_distance, max_still_distance, no_one_duration in seconds)
    {
#ifdef CONFIG_EZPI_UTIL_TRACE_EN
        TRACE_E("unable to set configuations");
#endif
        return ESP_FAIL;
    }

    if (!ld2410_set_gates_sensitivity_threshold(move_sensitivity, still_sensitivity))
    {
#ifdef CONFIG_EZPI_UTIL_TRACE_EN
        TRACE_E("unable to set gates sensitivity");
#endif
        return ESP_FAIL;
    }

    return ESP_OK;
}

esp_err_t ld2410_get_data(ld2410_outputs_t *output)
{
    if (NULL == output)
    {
        return ESP_FAIL;
    }

    if (ld2410_presence_detected())
    {
        output->presence = true;
        if (ld2410_stationary_target_detected())
        {
            output->stationary_target_distance = ld2410_stationary_target_distance();
#ifdef CONFIG_EZPI_UTIL_TRACE_EN
            TRACE_S("Stationary target: %d cm | energy: %d", output->stationary_target_distance, ld2410_stationary_target_energy());
#endif
        }

        if (ld2410_moving_target_detected())
        {
            uint16_t new_distance = ld2410_moving_target_distance();

            if (output->moving_target_distance < new_distance)
            {
                output->direction = MOVING_AWAY;
#ifdef CONFIG_EZPI_UTIL_TRACE_EN
                TRACE_E(stringify(MOVING_AWAY));
#endif
            }
            else if (output->moving_target_distance > new_distance)
            {
                output->direction = APPROACHING;
#ifdef CONFIG_EZPI_UTIL_TRACE_EN
                TRACE_E(stringify(APPROACHING));
#endif
            }

            output->moving_target_distance = new_distance;
#ifdef CONFIG_EZPI_UTIL_TRACE_EN
            TRACE_S("Moving target: %d cm | energy: %d", output->moving_target_distance, ld2410_moving_target_energy());
#endif
        }
        else
        {
            output->direction = STATIONARY;
        }
    }
    else
    {
        output->presence = false;
#ifdef CONFIG_EZPI_UTIL_TRACE_EN
        TRACE_S("No target");
#endif
    }

    return ESP_OK;
}

void ld2410_start_reading()
{
    ld2410_driver_start_reading();
}

void ld2410_stop_reading()
{
    ld2410_driver_stop_reading();
}
