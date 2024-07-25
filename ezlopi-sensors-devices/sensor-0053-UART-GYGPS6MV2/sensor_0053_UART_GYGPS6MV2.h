
#ifndef _SENSOR_0053_UART_GYGPS6MV2_H_
#define _SENSOR_0053_UART_GYGPS6MV2_H_

#include "ezlopi_core_actions.h"
#include "ezlopi_core_devices.h"
#include "ezlopi_core_errors.h"

/* 3.3V operating voltage */
//-----------------------------------------------------------------------------------------
//          Uncomment the message format you require from below
//
//   *NOTE:- Other message [except for GGA] need code modification
//-----------------------------------------------------------------------------------------
// #define RMC_MESSAGE_ENABLE
// #define VTG_MESSAGE_ENABLE
#define GGA_MESSAGE_ENABLE
// #define GSA_MESSAGE_ENABLE
// #define GSV_MESSAGE_ENABLE
// #define GLL_MESSAGE_ENABLE
//-----------------------------------------------------------------------------------------
#define MAX_GPGGA_SENTENCE_SIZE 85 // (:'$GPGGA') + (:',') + (:'message')
// #define MAX_GPRMC_SENTENCE_SIZE 70 // (:'$GPRMC') + (:',') + (:'message')
// #define MAX_GPVTG_SENTENCE_SIZE 70 // (:'$GPGGA') + (:',') + (:'message')
// #define MAX_GPGSA_SENTENCE_SIZE 70 // (:'$GPGGA') + (:',') + (:'message')
// #define MAX_GPGSV_SENTENCE_SIZE 40 // (:'$GPGGA') + (:',') + (:'message')
// #define MAX_GPGLL_SENTENCE_SIZE 40 // (:'$GPGGA') + (:',') + (:'message')

//-----------------------------------------------------------------------------------------
/**
 * circular buffer to store the complete message
 */
#define CIR_BUFSIZE 768 // choose ( buf_size >= 768) to avoid faulty message

/**
 * link: https://www.rfwireless-world.com/Terminology/GPS-sentences-or-NMEA-sentences.html
 *
 * GPS standard language : default :- NMEA
 *
 * Output parameter list:-
 *
 * $GPRMC,,V,,,,,,,,,,N*53                          // Time, date, position, course and speed data
 * $GPVTG,,,,,,,,,N*30                              // Course and speed information relative to the ground
 * $GPGGA,,,,,,0,00,99.99,,,,,,*48                  // Global positioning system fix data (time, position, fix type data)
 * $GPGSA,A,1,,,,,,,,,,,,,99.99,99.99,99.99*30      // GPS receiver operating mode, satellites used in the position solution, and DOP values.
 * $GPGSV,1,1,03,18,,,21,20,,,20,28,,,20*78         // The number of GPS satellites in view satellite ID numbers, elevation, azimuth and SNR values
 * $GPGLL,,,,,,V,N*64                               // Geographic position, latitude, longitude
 *
 * */

typedef struct hms_t
{
    char hour[3];
    char min[3];
    char sec[6];
} hms_t;

typedef struct latitude_t
{
    char lat_degree[3];
    char lat_min[9];
} latitude_t;

typedef struct longitude_t
{
    char long_degree[4];
    char long_min[9];
} longitude_t;

#ifdef RMC_MESSAGE_ENABLE
// 1.  Time, date, position, course and speed data
typedef struct GPRMC_t
{
    // $GPRMC -> RMC protocol header
    char GPRMC_sentence[MAX_GPRMC_SENTENCE_SIZE]; // this stores contents of GPRMC_data line
    hms_t UTC_time;                               // XXXXXX.XXX       //161229.487 -> hhmmss.sss
    char Status;                                  // X                // A/V ; A = data valid or V = data not valid
    latitude_t Latitude;                          // XXXX.XXXX        // 3723.2475 -> ddmm.mmmm
    char N_S_indicator;                           // X                // N/S (N = North, S = South)
    longitude_t Longitude;                        // XXXXX.XXXX       // 12158.3416 (121 degrees, 58.3416 minutes) -> dddmm.mmmm
    char E_W_indicator;                           // X                // E/W (E = East or W = West)
    char Speed_ovr_gnd[7];                        // XX.XXX           // 0.0013-> knots per hr
    char Course_ovr_gnd[7];                       // XX.XXX           // 309.62 -> direction in degrees
    char Date[7];                                 // XXXXXX           // 120598 -> ddmmyy
    char Magnetic_variation_val[7];               // XXX.XX           //Degrees [102.01]
    char Magnetic_variation_dir;                  // X                // (E= East or W = West)
    char mode;                                    // X                //  A-Autonomous ; D-Differential ; E-Estimated (dead reckoning) mode; M-Manual input; N-Data not valid
    char Checksum[3];                             // XX               // *53
} GPRMC_t;
#endif

#ifdef VTG_MESSAGE_ENABLE
// 2. Course and speed information relative to the ground
typedef struct GPVTG_t
{
    // $GPVTG -> VTG protocol header
    char GPVTG_sentence[MAX_GPVTG_SENTENCE_SIZE]; // this stores contents of GPVTG_data line
    char Course_deg[7];                           // XXX.XX               //309.62 [degree]
    char Track_indicator;                         // X                    // T-Track type
    char Magnetic_variation[7];                   // XX.XXX               // 24.168 [degree]
    char Magnetic_indicatior;                     // X                    // M-Magnetic type
    char Speed_in_knots[7];                       // X.XXXX               // 0.0013-> [knots] ,Measured horizontal speed
    char Knot_Unit;                               // X                    // N-Knots
    char Speed_in_km_hr[7];                       // X.XXXX               // 0.0013-> [Km/Hr], Measured horizontal speed
    char Km_hr_Unit;                              // X                    // K-Kilometers per hour
    char mode;                                    // X                    // A-Autonomous ; D-Differential ; E-Estimated (dead reckoning) mode; M-Manual input; N-Data not valid
    char Checksum[3];                             // XX                   // *30
} GPVTG_t;
#endif

#ifdef GGA_MESSAGE_ENABLE
// 3. Global positioning system fix data (time, position, fix type data)
typedef struct GPGGA_t
{
    // $GPGGA -> GGA protocol header
    char GPGGA_sentence[MAX_GPGGA_SENTENCE_SIZE]; // this stores contents of GPGGA_data line
    hms_t UTC_time;                               // XXXXXX.XX            // 061732.00 -> hhmmss.ss
    latitude_t Latitude;                          // XXXX.XXXXX           // 2740.52768 (27 degrees, 40.52768 minutes) -> ddmm.mmmmm
    char N_S_indicator;                           // X                    // (N = North, S = South)
    longitude_t Longitude;                        // XXXXX.XXXXX          // 08518.22110 (085 degrees, 18.22110 minutes) -> dddmm.mmmmm
    char E_W_indicator;                           // X                    // (E = East or W = West)
    char Positon_fix_quality;                     // X                    // 0-invalid ; 1-valid ; 2-Differential GPS ; 3_5 -not supported ; 6-Dead reckoning mode
    char Satellites_used[3];                      // XX                   // num of satelites -> Range: 0-12
    char HDOP[5];                                 // XX.XX                // 3.78 // Horizontal Dilution of Precision [eg. 1.0]
    char Mean_sea_level[7];                       // XXXX.X               // 1300.8 // [Antenna altitude above/below mean_sea_level in Meters]
    char MSL_Unit;                                // X                    // eg. M -> meters
    char Geoid_Separation[7];                     // XXX.XX               // -41.3 // [in Meters]The distance from the surface of an ellipsoid to the surface of the geoid
    char GS_Unit;                                 // X                    // eg. M -> meters
    char age_from_last[3];                        // XX     //empty field // – time in seconds since last DGPS update
    char station_ID[5];                           // XXXX   //empty field // – DGPS station ID number
    char Checksum[3];                             // XX                   // *48
} GPGGA_t;

#endif

#ifdef GSA_MESSAGE_ENABLE
// 4. GPS receiver operating mode, satellites used in the position solution, and DOP values.
typedef struct GPGSA_t
{
    char GPGSA_sentence[MAX_GPGSA_SENTENCE_SIZE]; // this stores contents of GPGSA_data line
    char mode_A_M;                                // X                      //A-Automatic 2D/3D ; M-Manual, forced to operate in 2D or 3D
    char mode_123;                                // X                      //Mode: 1 = Fix not available ; 2 = 2D ; 3 = 3D
    char PRN_num_1[4];                            // X.X                    // PRN numbers of satellites used in solution (null for unused fields),
    char PRN_num_2[4];                            // X.X                    //                   - GPS = 1 to 32
    char PRN_num_3[4];                            // X.X                    //                   - SBAS = 33 to 64 (add 87 for PRN number)
    char PRN_num_4[4];                            // X.X                    //                   - GLO = 65 to 96
    char PRN_num_5[4];                            // X.X                    //
    char PRN_num_6[4];                            // X.X                    //
    char PRN_num_7[4];                            // X.X                    //
    char PRN_num_8[4];                            // X.X                    //
    char PRN_num_9[4];                            // X.X                    //
    char PRN_num_10[4];                           // X.X                    //
    char PRN_num_11[4];                           // X.X                    //
    char PRN_num_12[4];                           // X.X                    //
    char PDOP[4];                                 // X.X                    // Position dilution of precision
    char HDOP[4];                                 // X.X                    // Horizontal dilution of precision
    char VDOP[4];                                 // X.X                    // Vertical dilution of precision
    char Checksum[3];                             // XX                     // *30
} GPGSA_t;
#endif

#ifdef GSV_MESSAGE_ENABLE
// 5. The number of GPS satellites in view satellite ID numbers, elevation, azimuth and SNR values
typedef struct GPGSV_t
{
    char GPGSV_sentence[MAX_GPGSV_SENTENCE_SIZE]; // this stores contents of GPGSV_data line
    char total_messages;                          // X                    // Total number of messages (1-9)
    char message_num;                             // X                    //	Message number (1-9)
    char Satellites_inView[3];                    // XX                   // Total number of satellites in view

    char Satellite_PRN[3]; // XX                   // Satellite PRN number
    char Elevation[3];     // XX                   // Elevation, [0 - 90]
    char Azimuth[4];       // XXX                  // Azimuth, [000 to 359]
    char SNR[3];           // XX                   // SNR (C/No) 00-99 dB, null when not tracking

    char Satellite_PRN_next[3]; // XX                   // Next Satellite PRN number
    char Elevation_next[3];     // XX                   // Next Elevation, [0 - 90]
    char Azimuth_next[4];       // XXX                  // Next Azimuth, [000 to 359]
    char SNR_next[3];           // XX                   // Next SNR (C/No) 00-99 dB, null when not tracking

    char Satellite_PRN_last[3]; // XX                   // Last Satellite PRN number
    char Elevation_last[3];     // XX                   // Last Elevation, [0 - 90]
    char Azimuth_last[4];       // XXX                  // Last Azimuth, [000 to 359]
    char SNR_last[3];           // XX                   // Last SNR (C/No) 00-99 dB, null when not tracking

    char Checksum[3]; // XX                   // *78
} GPGSV_t;
#endif

#ifdef GLL_MESSAGE_ENABLE
// 6. Geographic position, latitude, longitude
typedef struct GPGLL_t
{
    char GPGLL_sentence[MAX_GPGLL_SENTENCE_SIZE]; // this stores contents of GPGLL_data line
    latitude_t Latitude;                          // XXXX.XXXX           // Latitude (ddmm.mmmm)
    char N_S_indicator;                           // X                   // Latitude direction (N = North, S = South)
    longitude_t Longitude;                        // XXXXX.XXXX          // Longitude (dddmm.mmmm)
    char E_W_indicator;                           // X                   // (E = East or W = West)
    hms_t UTC_time;                               // XXXXXX.XX           // 161229.487	hhmmss.sss
    char Data_status;                             // X                   // A = data valid or V = data not valid
    char mode;                                    // X                   // A-Autonomous ; D-Differential ; E-Estimated (dead reckoning) mode; M-Manual input; N-Data not valid
    char Checksum[3];                             // XX                  // *64
} GPGLL_t;
#endif

/**
 * collective structure of individual data_sentences
 */
typedef struct GPS6MV2_t
{
#ifdef RMC_MESSAGE_ENABLE
    GPRMC_t GPRMC_data_structure;
#endif
#ifdef VTG_MESSAGE_ENABLE
    GPVTG_t GPVTG_data_structure;
#endif
#ifdef GGA_MESSAGE_ENABLE
    GPGGA_t GPGGA_data_structure;
#endif
#ifdef GSA_MESSAGE_ENABLE
    GPGSA_t GPGSA_data_structure;
#endif
#ifdef GSV_MESSAGE_ENABLE
    GPGSV_t GPGSV_data_structure;
#endif
#ifdef GLL_MESSAGE_ENABLE
    GPGLL_t GPGLL_data_structure;
#endif

    /*The global variables for operations of GPSmodule*/
    uint32_t Latitude_item_id;
    uint32_t Longitude_item_id;
    uint32_t Fix_item_id;
    uint32_t Sea_level_item_id;
    uint32_t Geoid_item_id;

    char gps_sentence[MAX_GPGGA_SENTENCE_SIZE + 30];
    char gps_cir_buf[CIR_BUFSIZE];

    bool gps_message_guard;
    bool prev_GPS_FIX;
    float prev_lat_angle_val;
    float prev_long_angle_val;
    float prev_antenna_alti;
    float prev_geoid;

} GPS6MV2_t;

//--------------------------------------------------------------------------------------------------------------------
ezlopi_error_t sensor_0053_UART_GYGPS6MV2(e_ezlopi_actions_t action, l_ezlopi_item_t *item, void *arg, void *user_arg);

#endif //_SENSOR_0053_UART_GYGPS6MV2_H_