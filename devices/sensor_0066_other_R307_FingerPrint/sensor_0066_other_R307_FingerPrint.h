#ifndef _0066_SENSOR_UART_R307_FINGERPRINT_H_
#define _0066_SENSOR_UART_R307_FINGERPRINT_H_

#include "ezlopi_actions.h"
#include "ezlopi_devices.h"
#include "ezlopi_gpio.h"

#include "freertos/FreeRTOS.h"
#include "freertos/task.h"

#include "stdbool.h"
#include "stdint.h"
/**
 * @brief tasks such as configuring peripherals , setting flags and controlling IO
 *
 * @paragraph 1. Set the least significant bit(LSB) of 8-bit reg without  effecting other
 * @paragraph uint8_t reg = 0xA5;  mask = 0x01                  => reg |= mask // set the LSB and assign
 *
 * @paragraph 2. Clear the Most significant bit(MSB) of 16-bit reg without  effecting other
 * @paragraph uint16_t var = 0xBEEF;  mask = 0x7FFF             => var &= mask // clear the MSB and assign
 *
 * @paragraph 3. check if bit4 is clear in 16-bit reg and return a boolean result
 * @paragraph uint16_t reg = 0x3704;  mask = 0x10                 => bool res = (reg & mask == 0); // check if bit4 is clear and assign bool val
 *
 * @paragraph 4. toggle all even bits (0,2,4,6) of 8-bit reg without effecting other
 * @paragraph uint8_t reg = 0xAA;  mask = 0x55                  => reg ^= mask // toggles even number bits only
 * @paragraph uint8_t reg = 0xAA;                               => reg ~= reg // toggles all bits
 *
 * @paragraph 5. extract 8-12bit from 16-bit variable
 * @paragraph uint16_t var = 0xFEDC; mask = 0x1F00              => uint16_t extracted_var = (var & mask >> 8); // check if bit4 is clear and assign bool val
 *
 * @paragraph 7. filter 3 LSB bits of 8-bit reg and rest is reset
 * @paragraph uint8_t reg = 0xA5;  mask = 0x07                  => reg &= mask // filter 3 LSB bits and assign
 *
 * @paragraph 8. swap two 8bits without using extra temp variable
 * @paragraph uin8_t a = 0x5A; b = 0x3C                         => 1st. a^=b  |  2nd.b^=a  |   3rd.a^=b
 *
 * @paragraph 9. replace the 4-7bits of 16-bit register with specific values
 * @paragraph uin16_t var = 0x8A35;  uint16_t mask = 0x00F0     => var = (var & ~mask) | (newValue << 4) ; //Set upper 4 bits to 'newValue'
 *
 * @paragraph 10. Check if exactly one of the two LSB in 8-bit reg is set (either bit-0 or bit-1, but not both) and return a boolean result
 * @paragraph uint8_t reg = 0x03;  mask = 0x03                  => bool res = ( [(reg & mask)==1] || [(reg & mask)==2] ) // check and convert to bool
 *
 */

//----------------------------------------------------------------------------------------------
/**
 * @brief The structure of data packets sent to R307 FingerPrint sensor
 */
//                    original                          functions
//          +--------+---------+------------+---------+-----------------------------+----------+
//          | Header  0xffffff      PID        P-Len     a.INS/b.Data/c.Para            CHK    |
//          +--------+---------+------------+---------+-----------------------------+----------+
//          |                                                                                  |
//          |                                         1._a._0x13H [VfyPwd]                     |
//          |                                         1._a._0x12H [SetPwd]                     |
//          |                                         1._a._0x15H [SetAdder]                   |
//          |                                         1._a._0x0EH [SetSysPara]                 |=
//          |                                         1._a._0x17H [Port Control]               |=
//          |                                         1._a._0x0FH [ReadSysPara]                |=
//          |                                         1._a._0x1DH [TempleteNum]                |=
//          |                                                                                  |
//          |                                         1._a._0x01H [GenImg]                     |
//          |                                         1._a._0x0AH [UpImage]                    |
//          |                                         1._a._0x0BH [DownImage]                  |
//          |                                         1._a._0x02H [Img2Tz]                     |
//          |                    1.cmd  - 01H         1._a._0x05H [RegModel]                   |
//          |  0xEF01     XX     2.data - 02H   0x__                                  sum      |
//          |                    3.end  - 08H         1._a._0x08H [UpChar]                     |
//          |                                         1._a._0x09H [DownChar]                   |
//          |                                         1._a._0x06H [Store]                      |
//          |                                         1._a._0x07H [LoadChar]                   |
//          |                                         1._a._0x0CH [DeletChar]                  |
//          |                                         1._a._0x0DH [Empty]                      |
//          |                                         1._a._0x03H [Match]                      |
//          |                                         1._a._0x04H [Search]                     |
//          +--------+---------+------------+---------+-----------------------------+----------+
//
//----------------------------------------------------------------------------------------------
/**
 * @brief The structure of data packets sent to R307 FingerPrint sensor
 */
//          +--------+---------+------------+-------+-------------------+-----+
//            Header  0xffffff      PID       P-Len   confirmation code   CHK
//          +--------+---------+------------+-------+-------------------+-----+
//            0xEF01     XX      1.Ack-07H
//          +--------+---------+------------+-------+-------------------+-----+
//
//
//          +--------+---------+-----+------+---------------+-----+
//---------------------------------------------------------------------

//----------------------------------------------------------------------------------------------------------------
#define MODE_DEFAULT FINGERPRINT_ERASE_WITH_IDS_MODE
#define USERID_DEFAULT 1
#define IDCOUNT_DEFAULT 10

// !< Setting the max quantity of fingerprints allowed to be stored >
#define FINGERPRINT_MAX_CAPACITY_LIMIT 100
// !< Setting the starting USER/PAGE ID >
#define FINGERPRINT_STARTING_USER_PAGE_ID 1
// !< Setting the max length of the transferring data package >
#define MAX_PACKET_LENGTH_VAL 64
// !< Setting Baudrate for transferring data via uart >
#define FINGERPRINT_UART_BAUDRATE ((int)FINGERPRINT_BAUDRATE_57600 * 9600)

//----------------------------------------------------------------------------------------------------------------

//---------------------------------------------------------------------
// Step 1: List the instructions cmds
#define FINGERPRINT_GETIMAGE 0x01       //!< Collect finger image
#define FINGERPRINT_IMAGE2TZ 0x02       //!< Generate character file from image
#define FINGERPRINT_MATCH 0x03          //!< Carry out precise matching of two templates
#define FINGERPRINT_SEARCH 0x04         //!< Search for fingerprint in slot/library
#define FINGERPRINT_REGMODEL 0x05       //!< Combine character files and generate template
#define FINGERPRINT_STORE 0x06          //!< Store template
#define FINGERPRINT_LOAD 0x07           //!< Read/load template
#define FINGERPRINT_UPLOAD_TEMP 0x08    //!< Upload template
#define FINGERPRINT_DOWNLOAD_TEMP 0x09  //!< Download template
#define FINGERPRINT_UPLOAD_IMG 0x0A     //!< Upload image
#define FINGERPRINT_DOWNLOAD_IMG 0x0B   //!< Download image
#define FINGERPRINT_DELETE 0x0C         //!< Delete templates
#define FINGERPRINT_EMPTY 0x0D          //!< Empty library
#define FINGERPRINT_SETSYSPARAM 0x0E    //!< Set system parameters [status register]
#define FINGERPRINT_READSYSPARAM 0x0F   //!< Read system parameters [status register]
#define FINGERPRINT_SETPASSWORD 0x12    //!< Sets passwords
#define FINGERPRINT_VERIFYPASSWORD 0x13 //!< Verifies the password
#define FINGERPRINT_SETADDRESS 0x15     //!< Set Address of the module [change 0xffffff]
#define FINGERPRINT_PORTCONTROL 0x17    //!< Control Port [On / Off]
#define FINGERPRINT_HISPEEDSEARCH 0x1B  //!< Asks the sensor to search [fastly] for a matching fingerprint template to the last model generated
#define FINGERPRINT_TEMPLATENUM 0x1D    //!< Read finger template numbers
#define FINGERPRINT_GR_IDENTIFY 0x34    //!< Automatic collect fingerprint, match captured fingerprint with fingerprint library

/*Fingerprint_LED CONFIGURATIONS*/
#define FINGERPRINT_AURALEDCONFIG 0x35 //!< Aura LED control
#define FINGERPRINT_LED_BREATHING 0x01 //!< Breathing light
#define FINGERPRINT_LED_BLUE 0x02      //!< Blue LED
#define FINGERPRINT_LED_OFF 0x04       //!< Always off
//---------------------------------------------------------------------
//---------------------------------------------------------------------
// Step 2: Ack CMDS
#define ACK_OK 0x00               //: commad execution complete;
#define ACK_ERR_RECV 0x01         //: error when receiving data package;
#define ACK_ERR_DETECT_FP 0x02    //: no finger on the sensor;
#define ACK_ERR_ENROLL_FP 0x03    //: fail to enroll the finger;
#define ACK_ERR_DIS_FP 0x06       //: fail to generate character file due to the over-disorderly fingerprint image;
#define ACK_ERR_SMALL_FP 0x07     //: fail to generate character file due to lackness of character point or over-smallness of fingerprint image
#define ACK_ERR_MATCH 0x08        //: finger doesn’t match;
#define ACK_ERR_NO_LIB_MATCH 0x09 //: fail to find the matching finger in library (both the PageID and matching score are 0);
#define ACK_ERR_CMB_CHRFILE 0x0A  //: fail to combine the character files;
#define ACK_ERR_ID_BYND_LIB 0x0B  //: Addressed PageID is beyond the finger library;
#define ACK_ERR_LOAD_CHR 0x0C     //: error when reading template from flash library to => character_file/template buffer [CharBuffer1/CharBuffer2];
#define ACK_ERR_UP_CHR 0x0D       //: error when uploading character_file or template to computer;
#define ACK_ERR_DWN_IMG_CHR 0x0E  //: error when downloading image/template from computer;
#define ACK_ERR_UP_IMG 0x0F       //: error when uploading image to computer;
#define ACK_ERR_DEL 0x10          //: fail to delete the template;
#define ACK_ERR_CLR 0x11          //: fail to clear finger library;
#define ACK_ERR_WRONG_PASS 0x13   //: wrong password!
#define ACK_ERR_PRIM_IMG 0x15     //: fail to generate the image for the lackness of valid primary image;
#define ACK_ERR_STORE 0x18        //: error when writing/storing template into flash library;
#define ACK_ERR_WRNG_REGS 0x1A    //: wrong resgister number;
#define ACK_ERR_OP_FAIL 0x1D      //: fail to operate the communication port;

// 0x1B //: incorrect configuration of register;
// 0x1C //: wrong notepad page number;
// 0x41 //: No finger on sensor when add fingerprint on second time.
// 0x42 //: fail to enroll the finger for second fingerprint add.
// 0x43 //: fail to generate character file due to lackness of character point or over-smallness of fingerprint image for second fingerprint add
// 0x44 //: fail to generate character file due to the over-disorderly fingerprint image for second fingerprint add;
// 0x45 //: Duplicate fingerprint
//---------------------------------------------------------------------

//---------------------------------------------------------------------
// # BAUDRATE control register
#define FINGERPRINT_BAUDRATE_CONTROL 0x04 //!< Targets the baudrate control register

// # BAUDRATE configuration cmd
// #define FINGERPRINT_BAUDRATE_9600 0x01 //!< UART baud 9600
#define FINGERPRINT_BAUDRATE_57600 0x06 //!< UART baud 57600
#if 0
// #define FINGERPRINT_BAUDRATE_19200 0x02 //!< UART baud 19200
// #define FINGERPRINT_BAUDRATE_28800 0x03  //!< UART baud 28800
// #define FINGERPRINT_BAUDRATE_38400 0x04  //!< UART baud 38400
// #define FINGERPRINT_BAUDRATE_48000 0x05 //!< UART baud 48000
// #define FINGERPRINT_BAUDRATE_67200 0x07  //!< UART baud 67200
// #define FINGERPRINT_BAUDRATE_76800 0x08  //!< UART baud 76800
// #define FINGERPRINT_BAUDRATE_86400 0x09  //!< UART baud 86400
// #define FINGERPRINT_BAUDRATE_96000 0x0A  //!< UART baud 96000
// #define FINGERPRINT_BAUDRATE_105600 0x0B //!< UART baud 105600
// #define FINGERPRINT_BAUDRATE_115200 0x0C //!< UART baud 115200
#endif
//----------------------------------------------------------------------------------------------------------------
// # BAUDRATE control register
#define FINGERPRINT_SECURITY_LEVEL 0x05 //!< Targets the security level register

// # SECURITY_LEVEL configuration cmd
#define FINGERPRINT_SECURITY_1 0x01 //!< Searching and matching security level 1
#define FINGERPRINT_SECURITY_2 0x02 //!< Searching and matching security level 2
#define FINGERPRINT_SECURITY_3 0x03 //!< Searching and matching security level 3
#define FINGERPRINT_SECURITY_4 0x04 //!< Searching and matching security level 4
#define FINGERPRINT_SECURITY_5 0x05 //!< Searching and matching security level 5
//----------------------------------------------------------------------------------------------------------------
//----------------------------------------------------------------------------------------------------------------
// # BAUDRATE control register
#define FINGERPRINT_DATA_PACKAGE_LENGTH 0x06 //!< Targets the Package length register

// # DATA_PACKET_LENGTH configuration cmd
#define FINGERPRINT_DATA_LENGTH_32 0x00  //!< Max length of the transmitted data package = 32 bytes
#define FINGERPRINT_DATA_LENGTH_64 0x01  //!< Max length of the transmitted data package = 64 bytes
#define FINGERPRINT_DATA_LENGTH_128 0x02 //!< Max length of the transmitted data package = 128 bytes
// #define FINGERPRINT_DATA_LENGTH_256 0x03 //!< Max length of the transmitted data package = 256 bytes

//----------------------------------------------------------------------------------------------------------------
// !< PACKET FORMAT
//---------------------------------------------------------------------
// !< Packet Header Value >
#define FINGERPRINT_HEADER_MSB 0xEF //!< Fixed falue of EF01H; High byte transferred first
#define FINGERPRINT_HEADER_LSB 0x01 //!< Fixed falue of EF01H; High byte transferred first
//---------------------------------------------------------------------
// !< Packet Scanner Module Address [Default Value] >
#define FINGERPRINT_DEVICE_ADDR_BIT 0xFF //!< Fixed falue of 0xFFH; High byte transferred first
//---------------------------------------------------------------------
// !< PACKET TYPES:- [Packet Identifier codes] >
#define FINGERPRINT_PID_COMMANDPACKET 0x01 //!< Command packet
#define FINGERPRINT_PID_DATAPACKET 0x02    //!< Data packet, must follow command packet or acknowledge packet
#define FINGERPRINT_PID_ACKPACKET 0x07     //!< Acknowledge packet
#define FINGERPRINT_PID_ENDDATAPACKET 0x08 //!< End of data packet
//----------------------------------------------------------------------------------------------------------------

//----------------------------------------------------------------------------------------------------------------
// !< SYSTEM STATUS/CONDITIONS
//---------------------------------------------------------------------
#define SYSTEM_FREE (uint16_t)(0 << 0)             //!< system is free
#define SYSTEM_BUSY (uint16_t)(1 << 0)             //!< system is busy
#define SYSTEM_FINGERPRINT_PASS (uint16_t)(1 << 1) //!< matching fingerprint found
#define SYSTEM_HNDSHK_VERIFIED (uint16_t)(1 << 2)  //!< [0xFF] handshaking password verified
#define SYSTEM_IMGBUF_VALID (uint16_t)(1 << 3)     //!< imagebuffer doesnot have a valid image
//----------------------------------------------------------------------------------------------------------------
#define UART_PORT_ON (uint8_t)1  //!< Uart port is ON
#define UART_PORT_OFF (uint8_t)0 //!< Uart port is OFF
//----------------------------------------------------------------------------------------------------------------

//----------------------------------------------------------------------------------------------------------------
// !< Custom tx-packet >
typedef struct fingerprint_packet_t
{
    uint8_t header_code[2];
    uint8_t device_address[4];
    uint8_t PID;                         /* Identifier : cmd, data, ack or end*/
    uint8_t Packet_len[2];               /* [PID + P_LEN + (data_fields)] should not exceed 256bytes*/
    uint8_t data[MAX_PACKET_LENGTH_VAL]; /*memcpy the (packet_len-2) num of bytes which should be <256 */
    uint8_t chk_sum[2];
} fingerprint_packet_t;
//----------------------------------------------------------------------------------------------------------------
// !< Custom enum for status response after executing a command >
typedef enum FINGERPRINT_STATUS_t
{
    FINGERPRINT_FAIL = 0,
    FINGERPRINT_OK,
} FINGERPRINT_STATUS_t; // FINGERPRINT_TIMEOUT = -1,
//----------------------------------------------------------------------------------------------------------------
// !< Custom intr_pin addition>
typedef struct fp_interface_t
{
    uint32_t intr_pin;
    bool fp_enable;
} fp_interface_t;

//----------------------------------------------------------------------------------------------------------------
// !< Custom enum indicating the current operation phase >
typedef enum e_FINGERPRINT_OP_MODE_t
{
    FINGERPRINT_MATCH_MODE = 0,
    FINGERPRINT_ENROLLMENT_MODE,
    FINGERPRINT_LIST_IDS_MODE,
    FINGERPRINT_ERASE_WITH_IDS_MODE,
    FINGERPRINT_ERASE_ALL_MODE,
    FINGERPRINT_MODE_MAX
} e_FINGERPRINT_OP_MODE_t;
//----------------------------------------------------------------------------------------------------------------
// !< Custom structure to send as a reply to server (@ MATCH phase) >
typedef struct server_packet_t
{
    fp_interface_t fp_interface;
    e_FINGERPRINT_OP_MODE_t opmode;                    /*Hold current operation mode*/
    uint16_t id_counts;                                /*This is used as an information for list and erase operations*/
    uint16_t user_id;                                  /*Stores: Template or character_page ID (0~999) [Also, used as starting ID in 'EraseID_mode'] */
    uint16_t confidence_level;                         /*0~100*/
    uint8_t recieved_buffer[MAX_PACKET_LENGTH_VAL];    /*This array store incomming uart message*/
    bool validity[FINGERPRINT_MAX_CAPACITY_LIMIT + 1]; /*status of each ID [1~500]*/
    bool __busy_guard;                                 /*Gaurd_flag used during notification actions*/
    TaskHandle_t notifyHandler;
    TaskHandle_t timerHandle;
} server_packet_t;

//---------- FUNCTIONS Defination for Fingerprint Library ------------------------------------------------------------------------------------------------------
#if 0
// bool UpImage(int uart_channel_num, uint8_t *recieved_buffer, uint32_t timeout);
// bool DownImage(int uart_channel_num, uint8_t *recieved_buffer, uint32_t timeout);

// bool SetPwd(int uart_channel_num, uint32_t new_password, uint8_t *recieved_buffer, uint32_t timeout);

// bool UpChar(int uart_channel_num, uint8_t CharBufferID, uint8_t *recieved_buffer, uint32_t timeout);
// bool DownChar(int uart_channel_num, uint8_t CharBufferID, uint8_t *recieved_buffer, uint32_t timeout);
#endif

bool Empty(int uart_channel_num, uint8_t *recieved_buffer, uint32_t timeout);
bool GenImg(int uart_channel_num, uint8_t *recieved_buffer, uint32_t timeout);
bool RegModel(int uart_channel_num, uint8_t *recieved_buffer, uint32_t timeout);

bool LedControl(int uart_channel_num, bool LED_state, uint8_t *recieved_buffer, uint32_t timeout);

bool Img2Tz(int uart_channel_num, uint8_t CharBufferID, uint8_t *recieved_buffer, uint32_t timeout);
bool PortControl(int uart_channel_num, uint8_t Control_code, uint8_t *recieved_buffer, uint32_t timeout);

bool ReadTempNum(int uart_channel_num, uint16_t *TempNum, uint8_t *recieved_buffer, uint32_t timeout);
bool Match(int uart_channel_num, uint16_t *InspectionScore, uint8_t *recieved_buffer, uint32_t timeout);
bool ReadSysPara(int uart_channel_num, uint16_t *Status_bits, uint8_t *recieved_buffer, uint32_t timeout);

bool SetAdder(int uart_channel_num, uint32_t new_address, uint8_t *recieved_buffer, uint32_t timeout);
bool VerifyPwd(int uart_channel_num, uint32_t the_password, uint8_t *recieved_buffer, uint32_t timeout);

bool Store(int uart_channel_num, uint8_t CharBufferID, uint16_t PageID, uint8_t *recieved_buffer, uint32_t timeout);
bool Load(int uart_channel_num, uint8_t CharBufferID, uint16_t PageID, uint8_t *recieved_buffer, uint32_t timeout);

bool Delete(int uart_channel_num, uint16_t PageID, uint16_t TempCount, uint8_t *recieved_buffer, uint32_t timeout);
bool GR_Identify(int uart_channel_num, uint16_t *PageID_ptr, uint16_t *MatchScore_ptr, uint8_t *recieved_buffer, uint32_t timeout);

bool SetSysPara(int uart_channel_num, uint8_t Parameter_Number, uint8_t Parameter_content, uint8_t *recieved_buffer, uint32_t timeout);
bool Search(int uart_channel_num, uint8_t CharBufferID, uint16_t StartPage, uint16_t PageNum, uint16_t *PageID_ptr, uint16_t *MatchScore_ptr, uint8_t *recieved_buffer, uint32_t timeout);

//---------- FUNCTIONS Defination for Operation modes ------------------------------------------------------------------------------------------------------

bool Wait_till_system_free(l_ezlopi_item_t *item, uint32_t timeout);
uint16_t Find_immediate_vaccant_ID(l_ezlopi_item_t *item);
bool Check_PAGEID_Empty(l_ezlopi_item_t *item);
bool Update_ID_status_list(l_ezlopi_item_t *item);
bool Match_ID(l_ezlopi_item_t *item);
bool Erase_all_ID(l_ezlopi_item_t *item);
bool Erase_Specified_ID(l_ezlopi_item_t *item);
uint16_t Enroll_Fingerprint(l_ezlopi_item_t *item);
FINGERPRINT_STATUS_t fingerprint_config(l_ezlopi_item_t *item);

//----------------------------------------------------------------------------------------------------------------
int sensor_0066_other_R307_FingerPrint(e_ezlopi_actions_t action, l_ezlopi_item_t *item, void *arg, void *user_arg);

#endif