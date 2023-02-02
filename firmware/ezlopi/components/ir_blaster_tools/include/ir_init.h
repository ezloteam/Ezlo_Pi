#pragma once

#ifdef __cplusplus
extern "C" {
#endif


#include "driver/rmt.h"
#include "ir_tools.h"
#include "ir_remote.h"
#include "trace.h"


#define RMT_RX_GPIO 8
#define RMT_TX_GPIO 18
#define RMT_RX_CHANNEL 4
#define RMT_TX_CHANNEL 0

/**
 * @brief RMT IR receiver Initialization
 *
 */
ir_parser_config_t* rmt_rx_init() 
{
    rmt_config_t rmt_rx_config = RMT_DEFAULT_CONFIG_RX(RMT_RX_GPIO, RMT_RX_CHANNEL);
    rmt_config(&rmt_rx_config);
    rmt_driver_install(RMT_RX_CHANNEL, 3000, 0);
    ir_parser_config_t ir_parser_config = IR_PARSER_DEFAULT_CONFIG((ir_dev_t)RMT_RX_CHANNEL);
    ir_parser_config.flags |= IR_TOOLS_FLAGS_PROTO_EXT;// Using extended IR protocols (both NEC and RC5 have extended version)
    return &ir_parser_config;
}

/**
 * @brief RMT IR receiver Initialization
 *
 */
ir_builder_config_t* rmt_tx_init() 
{
    rmt_config_t rmt_tx_config = RMT_DEFAULT_CONFIG_TX(RMT_TX_GPIO, RMT_TX_CHANNEL);
    rmt_tx_config.tx_config.carrier_en = true;
    rmt_config(&rmt_tx_config);
    rmt_driver_install(RMT_TX_CHANNEL, 0, 0);
    ir_builder_config_t ir_builder_config = IR_BUILDER_DEFAULT_CONFIG((ir_dev_t)RMT_TX_CHANNEL);
    ir_builder_config.flags |= IR_TOOLS_FLAGS_PROTO_EXT; // Using extended IR protocols (both NEC and RC5 have extended version)
    return &ir_builder_config;
}

/**
 * @brief IR Protocol Selection
 *
 */
ir_protocol_type_t ir_protocol_selection(ir_remote_info_t*  ir_remote_info_handler)
{
    ir_protocol_type_t ir_protocol_type = 0;

    switch(ir_remote_info_handler->ir_remote_model_type)
    {
        case IR_DEVICE_TV_BRAND_SONY_MODEL_SONYTV_1:
        case IR_DEVICE_TV_BRAND_SONY_MODEL_SONYTV_2:
        case IR_DEVICE_PROJECTOR_BRAND_SONY_MODEL_V500_VPL_W400:
        case IR_DEVICE_BD_DVD_BRAND_SONY_MODEL_DVP_S505:
        case IR_DEVICE_BD_DVD_BRAND_SONY_MODEL_BD_S1500:
        case IR_DEVICE_BD_DVD_BRAND_SONY_MODEL_RM_470:
        case IR_DEVICE_BD_DVD_BRAND_SONY_MODEL_RMT_V501E:
        case IR_DEVICE_BD_DVD_BRAND_SONY_MODEL_RMT_V406B:
        case IR_DEVICE_SOUND_BAR_BRAND_SONY_MODEL_CMT_CP_100:
            ir_protocol_type = IR_REMOTE_PROTOCOL_SONY;
            break;

        case IR_DEVICE_TV_BRAND_PANASONIC_MODEL_PANASONICTV_1:
        case IR_DEVICE_PROJECTOR_BRAND_PANASONIC_MODEL_PROJECTOR_1:
        case IR_DEVICE_SOUND_BAR_BRAND_PANASONIC_MODEL_SCALL70T:
            ir_protocol_type = IR_REMOTE_PROTOCOL_PANASONIC;
            break;
        
        case IR_DEVICE_TV_BRAND_LG_MODEL_42H3000:
        case IR_DEVICE_TV_BRAND_LG_MODEL_6710V00005G:
        case IR_DEVICE_TV_BRAND_LG_MODEL_BC205P:
            ir_protocol_type = IR_REMOTE_PROTOCOL_LG;
            break;

        case IR_DEVICE_TV_BRAND_SAMSUNG_MODEL_00077A:
        case IR_DEVICE_TV_BRAND_FUSIONTV_MODEL_RC3B_1042:
        case IR_DEVICE_TV_BRAND_SUPRATV_MODEL_RC3B:
        case IR_DEVICE_BD_DVD_BRAND_LG_MODEL_6710CDAP01B:
        case IR_DEVICE_BD_DVD_BRAND_LG_MODEL_BD300:
        case IR_DEVICE_BD_DVD_BRAND_SAMSUNG_MODEL_00008E:
        case IR_DEVICE_BD_DVD_BRAND_SAMSUNG_MODEL_00011K:
        case IR_DEVICE_BD_DVD_BRAND_SAMSUNG_MODEL_00021C:
        case IR_DEVICE_BD_DVD_BRAND_SAMSUNG_MODEL_000225G_2:
        case IR_DEVICE_BD_DVD_BRAND_SAMSUNG_MODEL_00049C:
        case IR_DEVICE_BD_DVD_BRAND_SAMSUNG_MODEL_00052B:
        case IR_DEVICE_BD_DVD_BRAND_SAMSUNG_MODEL_633_205:
        case IR_DEVICE_BD_DVD_BRAND_SAMSUNG_MODEL_10329K:
        case IR_DEVICE_BD_DVD_BRAND_SAMSUNG_MODEL_SHOW_VIEW_00070_63:
            ir_protocol_type = IR_REMOTE_PROTOCOL_SAMSUNGTV;
            break;

        case IR_DEVICE_TV_BRAND_AOC_MODEL_M19W531:
        case IR_DEVICE_TV_BRAND_VEONTV_MODEL_SR0322016:
        case IR_DEVICE_PROJECTOR_BRAND_EPSON_MODEL_12807990:
        case IR_DEVICE_PROJECTOR_BRAND_EPSON_MODEL_PROJECTOR_1:
        case IR_DEVICE_PROJECTOR_BRAND_EPSON_MODEL_PROJECTOR_2:
        case IR_DEVICE_BD_DVD_BRAND_SAMSUNG_MODEL_000225G:
        case IR_DEVICE_BD_DVD_BRAND_SAMSUNG_MODEL_00054D:
        case IR_DEVICE_BD_DVD_BRAND_SAMSUNG_MODEL_00056A:
        case IR_DEVICE_BD_DVD_BRAND_SAMSUNG_MODEL_00092M:
        case IR_DEVICE_BD_DVD_BRAND_SAMSUNG_MODEL_01043A:
        case IR_DEVICE_SOUND_BAR_BRAND_SOUNDCORE_MODEL_INFINI_PRO:
        case IR_DEVICE_SOUND_BAR_BRAND_GOODMANS_MODEL_GDSBT1000P:
        case IR_DEVICE_SOUND_BAR_BRAND_EDIFIER_MODEL_R1850DB:
            ir_protocol_type = IR_REMOTE_PROTOCOL_NEC;
            break;

        default:
            TRACE_E("NO REMOTE MODEL SELECTED");
            break;
    }
    return ir_protocol_type;
}

/**
 * @brief IR Protocol Initialization
 *
 */
ir_protocol_init_t* ir_protocol_init(ir_remote_info_t *ir_remote_info_handler, ir_parser_config_t* ir_parser_config, ir_builder_config_t* ir_builder_config)
{
    ir_protocol_init_t *ir_protocol_init_props = NULL;
    ir_protocol_type_t ir_protocol_type;
    
    ir_protocol_type = ir_protocol_selection(ir_remote_info_handler);

    switch(ir_protocol_type)
    {
        case IR_REMOTE_PROTOCOL_NEC:
        {
            ir_protocol_init_props->ir_parser = ir_parser_rmt_new_nec(ir_parser_config);
            ir_protocol_init_props->ir_builder = ir_builder_rmt_new_nec(ir_builder_config);
            break;
        }
        case IR_REMOTE_PROTOCOL_RC5:
        {
            ir_protocol_init_props->ir_parser = ir_parser_rmt_new_rc5(ir_parser_config);
            ir_protocol_init_props->ir_builder = ir_builder_rmt_new_rc5(ir_builder_config);
            break;
        }

        case IR_REMOTE_PROTOCOL_SAMSUNGTV:
        {
            ir_protocol_init_props->ir_parser = ir_parser_rmt_new_samsung(ir_parser_config);
            ir_protocol_init_props->ir_builder = ir_builder_rmt_new_samsung(ir_builder_config);
            break;
        }

        case IR_REMOTE_PROTOCOL_LG:
        {
            ir_protocol_init_props->ir_parser = ir_parser_rmt_new_lg(ir_parser_config);
            ir_protocol_init_props->ir_builder = ir_builder_rmt_new_lg(ir_builder_config);
            break;
        }

        case IR_REMOTE_PROTOCOL_SONY:
        {
            ir_protocol_init_props->ir_parser = ir_parser_rmt_new_sony(ir_parser_config);
            ir_protocol_init_props->ir_builder = ir_builder_rmt_new_sony(ir_builder_config);
            break;
        }

        case IR_REMOTE_PROTOCOL_GREE:
        {
            ir_protocol_init_props->ir_parser = ir_parser_rmt_new_gree(ir_parser_config);
            ir_protocol_init_props->ir_builder = ir_builder_rmt_new_gree(ir_builder_config);
            break;
        }
        

        case IR_REMOTE_PROTOCOL_PANASONIC:
        {
            ir_protocol_init_props->ir_parser = ir_parser_rmt_new_panasonic(ir_parser_config);
            ir_protocol_init_props->ir_builder = ir_builder_rmt_new_panasonic(ir_builder_config);
            break;
        }
        

        case IR_REMOTE_PROTOCOL_DISH:
        {
            ir_protocol_init_props->ir_parser = ir_parser_rmt_new_dish(ir_parser_config);
            ir_protocol_init_props->ir_builder = ir_builder_rmt_new_dish(ir_builder_config);
            break;
        }
        

        case IR_REMOTE_PROTOCOL_JVC:
        {
            ir_protocol_init_props->ir_parser = ir_parser_rmt_new_jvc(ir_parser_config);
            ir_protocol_init_props->ir_builder = ir_builder_rmt_new_jvc(ir_builder_config);
            break;
        }
        

        case IR_REMOTE_PROTOCOL_LEGO:
        {
            ir_protocol_init_props->ir_parser = ir_parser_rmt_new_lego(ir_parser_config);
            ir_protocol_init_props->ir_builder = ir_builder_rmt_new_lego(ir_builder_config);
            break;
        }

        case IR_REMOTE_PROTOCOL_SAMSUNGAC:
        {
            ir_protocol_init_props->ir_parser = ir_parser_rmt_new_samsung(ir_parser_config);
            ir_protocol_init_props->ir_builder = ir_builder_rmt_new_samsung(ir_builder_config);
            break;
        }
        

        case IR_REMOTE_PROTOCOL_TOSHIBAAC50:
        {
            ir_protocol_init_props->ir_parser = ir_parser_rmt_new_toshibaAC(ir_parser_config);
            ir_protocol_init_props->ir_builder = ir_builder_rmt_new_toshibaAC(ir_builder_config);
            break;
        }
        

        case IR_REMOTE_PROTOCOL_TOSHIBAAC100:
        {
            ir_protocol_init_props->ir_parser = ir_parser_rmt_new_toshibaAC(ir_parser_config);
            ir_protocol_init_props->ir_builder = ir_builder_rmt_new_toshibaAC(ir_builder_config);
            break;
        }

        case IR_REMOTE_PROTOCOL_TOSHIBATV:
        {
            ir_protocol_init_props->ir_parser = ir_parser_rmt_new_nec(ir_parser_config);
            ir_protocol_init_props->ir_builder = ir_builder_rmt_new_nec(ir_builder_config);
            break;
        }
        

        case IR_REMOTE_PROTOCOL_EPSON:
        {
             ir_protocol_init_props->ir_parser = ir_parser_rmt_new_nec(ir_parser_config);
            ir_protocol_init_props->ir_builder = ir_builder_rmt_new_nec(ir_builder_config);
            break;
        }

        case IR_REMOTE_PROTOCOL_AIRTON:
        {
            ir_protocol_init_props->ir_parser = ir_parser_rmt_new_airton(ir_parser_config);
            ir_protocol_init_props->ir_builder = ir_builder_rmt_new_airton(ir_builder_config);
            break;
        }

        case IR_REMOTE_PROTOCOL_AIWA:
        {
            ir_protocol_init_props->ir_parser = ir_parser_rmt_new_nec(ir_parser_config);
            ir_protocol_init_props->ir_builder = ir_builder_rmt_new_nec(ir_builder_config);
            break;
        }
        default:
        {
            ir_protocol_init_props->ir_parser = ir_parser_rmt_new_nec(ir_parser_config);
            ir_protocol_init_props->ir_builder = ir_builder_rmt_new_nec(ir_builder_config);
            break;
        }
    }
    return ir_protocol_init_props;
}

#ifdef __cplusplus
}
#endif