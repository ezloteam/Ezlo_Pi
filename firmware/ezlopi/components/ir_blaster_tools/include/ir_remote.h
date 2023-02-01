#pragma once

#ifdef __cplusplus
extern "C" {
#endif

#include "ir_tools.h"


/**
 * @brief IR REMOTE DEVICE TYPE STRUCTURE 
 *  
 */
typedef enum ir_remote_device_s {
    IR_DEVICE_TV = 0,
    IR_DEVICE_AC,
    IR_DEVICE_DVD,
    IR_DEVICE_AVRECEIVER,
    IR_DEVICE_FAN,
    IR_DEVICE_PROJECTOR,
    IR_DEVICE_STB,                                         // STB = Set-Top-Box
    IR_DEVICE_BLUERAY,
    IR_DEVICE_LEDLAMP,
    IR_DEVICE_MAX
} ir_remote_device_t;

/**
 * @brief IR REMOTE DEVICE BRAND TYPE STRUCTURE 
 *  
 */
typedef enum ir_remote_device_brand_s {
    IR_DEVICE_BRAND_SONY = 0,
    IR_DEVICE_BRAND_PANASONIC,
    IR_DEVICE_BRAND_LG,
    IR_DEVICE_BRAND_SAMSUNG,
    IR_DEVICE_BRAND_TOSHIBA,
    IR_DEVICE_BRAND_DISH,
    IR_DEVICE_BRAND_AIRTON,
    IR_DEVICE_BRAND_AIWA,
    IR_DEVICE_BRAND_EPSON,
    IR_DEVICE_BRAND_GREE,
    IR_DEVICE_BRAND_JVC,
    IR_DEVICE_BRAND_PHILIPS,
    IR_DEVICE_BRAND_YAMAHA,
    IR_DEVICE_BRAND_AOC,
    IR_DEVICE_BRAND_VEONTV,
    IR_DEVICE_BRAND_ALOKA,
    IR_DEVICE_BRAND_MAX
} ir_remote_device_brand_t;

/**
 * @brief IR REMOTE DEVIE MODEL TYPE STRUCTURE 
 *  
 */
typedef enum {
    IR_DEVICE_TV_BRAND_PANASONIC_MODEL_PANASONICTV_1,                                       //      0
    IR_DEVICE_TV_BRAND_AOC_MODEL_M19W531,                                                   //      1
    IR_DEVICE_TV_BRAND_VEONTV_MODEL_SR0322016,                                              //      2
    IR_DEVICE_TV_BRAND_LG_MODEL_42H3000,                                                    //      3
    // IR_DEVICE_TV_BRAND_LG_MODEL_6710T00009B,
    // IR_DEVICE_TV_BRAND_LG_MODEL_6710V00008K,
    // IR_DEVICE_TV_BRAND_LG_MODEL_AKB72915207,
    // IR_DEVICE_TV_BRAND_LG_MODEL_AKB69680403,
    // IR_DEVICE_TV_BRAND_LG_MODEL_6710T00003B,
    IR_DEVICE_TV_BRAND_LG_MODEL_6710V00005G,                                                //      4
    IR_DEVICE_TV_BRAND_LG_MODEL_BC205P,                                                     //      5
    IR_DEVICE_TV_BRAND_SONY_MODEL_SONYTV_1,                                                 //      6
    IR_DEVICE_TV_BRAND_SONY_MODEL_SONYTV_2,                                                 //      7
    IR_DEVICE_TV_BRAND_SAMSUNG_MODEL_00077A,                                                //      8
    IR_DEVICE_TV_BRAND_SUPRATV_MODEL_RC3B,                                                  //      9
    IR_DEVICE_TV_BRAND_FUSIONTV_MODEL_RC3B_1042,                                            //      10


    IR_DEVICE_PROJECTOR_BRAND_SONY_MODEL_V500_VPL_W400,                                     //      11
    IR_DEVICE_PROJECTOR_BRAND_EPSON_MODEL_12807990,                                         //      12
    IR_DEVICE_PROJECTOR_BRAND_EPSON_MODEL_PROJECTOR_1,                                      //      13
    IR_DEVICE_PROJECTOR_BRAND_PANASONIC_MODEL_PROJECTOR_1,                                  //      14


    IR_DEVICE_BD_DVD_BRAND_LG_MODEL_6710CDAP01B,                                            //      15
    // IR_DEVICE_BD_DVD_BRAND_LG_MODEL_6710CDAL01G,
    IR_DEVICE_BD_DVD_BRAND_LG_MODEL_BD300,                                                  //      16
    IR_DEVICE_BD_DVD_BRAND_SAMSUNG_MODEL_00008E,                                            //      17
    IR_DEVICE_BD_DVD_BRAND_SAMSUNG_MODEL_00011K,                                            //      18
    IR_DEVICE_BD_DVD_BRAND_SAMSUNG_MODEL_00021C,                                            //      19
    IR_DEVICE_BD_DVD_BRAND_SAMSUNG_MODEL_000225G,                                           //      20
    IR_DEVICE_BD_DVD_BRAND_SAMSUNG_MODEL_000225G_2,                                         //      21
    IR_DEVICE_BD_DVD_BRAND_SAMSUNG_MODEL_00049C,                                            //      22
    IR_DEVICE_BD_DVD_BRAND_SAMSUNG_MODEL_00052B,                                            //      23
    IR_DEVICE_BD_DVD_BRAND_SAMSUNG_MODEL_00054D,                                            //      24
    IR_DEVICE_BD_DVD_BRAND_SAMSUNG_MODEL_00056A,                                            //      25
    IR_DEVICE_BD_DVD_BRAND_SAMSUNG_MODEL_00092M,                                            //      26
    IR_DEVICE_BD_DVD_BRAND_SAMSUNG_MODEL_SHOW_VIEW_00070_63,                                //      27
    IR_DEVICE_BD_DVD_BRAND_SAMSUNG_MODEL_01043A,                                            //      28
    IR_DEVICE_BD_DVD_BRAND_SAMSUNG_MODEL_633_205,                                           //      29
    IR_DEVICE_BD_DVD_BRAND_SAMSUNG_MODEL_10329K,                                            //      30
    IR_DEVICE_BD_DVD_BRAND_SONY_MODEL_DVP_S505,                                             //      31
    IR_DEVICE_BD_DVD_BRAND_SONY_MODEL_BD_S1500,                                             //      32
    IR_DEVICE_BD_DVD_BRAND_SONY_MODEL_RM_470,                                               //      33
    IR_DEVICE_BD_DVD_BRAND_SONY_MODEL_RMT_V501E,                                            //      34
    // IR_DEVICE_BD_DVD_BRAND_SONY_MODEL_RMT_V501A,
    IR_DEVICE_BD_DVD_BRAND_SONY_MODEL_RMT_V406B,                                            //      35


    IT_DEVICE_SOUND_BAR_BRAND_PANASONIC_MODEL_SCALL70T,                                     //      36
    IT_DEVICE_SOUND_BAR_BRAND_SOUNDCORE_MODEL_INFINI_PRO,                                   //      37
    IT_DEVICE_SOUND_BAR_BRAND_GOODMANS_MODEL_GDSBT1000P,                                    //      38
    IT_DEVICE_SOUND_BAR_BRAND_EDIFIER_MODEL_R1850DB,                                        //      39
    IT_DEVICE_SOUND_BAR_BRAND_SONY_MODEL_CMT_CP_100,                                        //      40


    IR_DEVICE_BRAND_REMOTE_MODEL_MAX                                                        //      41
} ir_remote_device_model_type_t;

/** 
 * @brief   /// Enumerator for defining and numbering of supported IR protocol.
* @note     Always add to the end of the list and should never remove entries
*           or change order. Projects may save the type number for later usage
*           so numbering should always stay the same.
* 
*/ 
typedef enum ir_protocol_type_t {
    IR_REMOTE_PROTOCOL_NEC = 0,
    IR_REMOTE_PROTOCOL_RC5,
    IR_REMOTE_PROTOCOL_SAMSUNGTV,
    IR_REMOTE_PROTOCOL_LG,
    IR_REMOTE_PROTOCOL_SONY, 
    IR_REMOTE_PROTOCOL_GREE,  //5
    IR_REMOTE_PROTOCOL_PANASONIC,
    IR_REMOTE_PROTOCOL_DISH,
    IR_REMOTE_PROTOCOL_JVC,
    IR_REMOTE_PROTOCOL_LEGO,
    IR_REMOTE_PROTOCOL_SAMSUNGAC,  //10
    IR_REMOTE_PROTOCOL_TOSHIBAAC50,
    IR_REMOTE_PROTOCOL_TOSHIBAAC100,
    IR_REMOTE_PROTOCOL_TOSHIBATV,
    IR_REMOTE_PROTOCOL_EPSON,
    IR_REMOTE_PROTOCOL_AIRTON,  //15
    IR_REMOTE_PROTOCOL_AIWA,
    IR_REMOTE_PROTOCOL_MAX
};


typedef struct ir_protocol_init_s {
    ir_builder_t ir_builder;
    ir_parser_t ir_parser;
} ir_protocol_init_t;

/**
 * @brief IR REMOTE INFO STRUCTURE TO STORE DATA FROM CLOUD FOR CONFIGURATION
 *
 *  ir_remote_brand = for storing remote brand info such as SAMSUNG, TOSHIBA,etc..
 *  ir_remote_type  = for storing remote type info for which it should work such as TV, PROJECTOR, etc...
 *  ir_remote_type  = for storing remote model info to configure protocol such as MR20GA (LGTV remote model)
 */
typedef struct ir_remote_info_s {
    ir_remote_device_t ir_remote_device_type;
    ir_remote_device_brand_t ir_remote_brand_type;
    ir_remote_device_model_type_t ir_remote_model_type;
} ir_remote_info_t;




#ifdef __cplusplus
}
#endif