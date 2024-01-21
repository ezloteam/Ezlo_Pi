#ifndef _0066_SENSOR_UART_R307_FINGERPRINT_H_
#define _0066_SENSOR_UART_R307_FINGERPRINT_H_

#include "ezlopi_core_actions.h"
#include "ezlopi_core_devices.h"
#include "ezlopi_hal_gpio.h"

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

//---------------------------------------------------------------------------------------------------------------
#define FINGERPRINT_MAX_CAPACITY_LIMIT 5                                   // !< Setting the max quantity of fingerprints allowed to be stored >
#define FINGERPRINT_STARTING_USER_PAGE_ID 1                                // !< Setting the starting USER/PAGE ID >
#define MAX_PACKET_LENGTH_VAL 64                                           // !< Setting the max length of the transferring data package >
#define FINGERPRINT_UART_BAUDRATE ((int)FINGERPRINT_BAUDRATE_57600 * 9600) // !< Setting Baudrate for transferring data via uart >
//----------------------------------------------------------------------------------------------------------------
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
#define FINGERPRINT_LED_BREATHING 0x01 //!< Turns On ;Breathing light
#define FINGERPRINT_LED_BLUE 0x02      //!< Blue LED
#define FINGERPRINT_LED_OFF 0x04       //!< Turns OFF ; LED
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
#define ACK_ERR_CONFIG 0x1B       //: incorrect configuration of register;
#define ACK_ERR_PAGE 0x1C         //: wrong notepad page number;
#define ACK_ERR_OP_FAIL 0x1D      //: fail to operate the communication port;
#define ACK_ERR_DUP_FP 0x45       //: Duplicate fingerprint

#if 0
// #define ACK_ERR_ 0x41 //: No finger on sensor when add fingerprint on second time.
// #define ACK_ERR_ 0x42 //: fail to enroll the finger for second fingerprint add.
// #define ACK_ERR_ 0x43 //: fail to generate character file ; lackness of character_point or over-smallness of second fingerprint image
// #define ACK_ERR_ 0x44 //: fail to generate character file due to the over-disorderly fingerprint image for second fingerprint add;
#endif
//---------------------------------------------------------------------
// !< BAUDRATE Baudrate_control_register
#define FINGERPRINT_BAUDRATE_CONTROL 0x04 //!< Targets the baudrate control register
#define FINGERPRINT_BAUDRATE_57600 0x06   //!< UART baud 57600

// !< BAUDRATE Security_control_register
#define FINGERPRINT_SECURITY_LEVEL 0x05 //!< Targets the security level register
#define FINGERPRINT_SECURITY_1 0x01     //!< Searching and matching security level 1
#define FINGERPRINT_SECURITY_2 0x02     //!< Searching and matching security level 2
#define FINGERPRINT_SECURITY_3 0x03     //!< Searching and matching security level 3
#define FINGERPRINT_SECURITY_4 0x04     //!< Searching and matching security level 4
#define FINGERPRINT_SECURITY_5 0x05     //!< Searching and matching security level 5
// !< BAUDRATE Datalength_control register
#define FINGERPRINT_DATA_PACKAGE_LENGTH 0x06 //!< Targets the Package length register
#define FINGERPRINT_DATA_LENGTH_32 0x00      //!< Max length of the transmitted data package = 32 bytes
#define FINGERPRINT_DATA_LENGTH_64 0x01      //!< Max length of the transmitted data package = 64 bytes
#define FINGERPRINT_DATA_LENGTH_128 0x02     //!< Max length of the transmitted data package = 128 bytes
#define FINGERPRINT_DATA_LENGTH_256 0x03     //!< Max length of the transmitted data package = 256 bytes
//----------------------------------------------------------------------------------------------------------------
// !< PACKET FORMAT
//----------------------------------------------------------------------------------------------------------------
// !< Packet Header Value >
#define FINGERPRINT_HEADER_MSB 0xEF //!< Fixed falue of EF01H; High byte transferred first
#define FINGERPRINT_HEADER_LSB 0x01 //!< Fixed falue of EF01H; High byte transferred first
// !< Packet Scanner Module Address [Default Value] >
#define FINGERPRINT_DEVICE_ADDR_BIT 0xFF //!< Fixed falue of 0xFFH; High byte transferred first
// !< PACKET TYPES:- [Packet Identifier codes] >
#define FINGERPRINT_PID_COMMANDPACKET 0x01 //!< Command packet
#define FINGERPRINT_PID_DATAPACKET 0x02    //!< Data packet, must follow command packet or acknowledge packet
#define FINGERPRINT_PID_ACKPACKET 0x07     //!< Acknowledge packet
#define FINGERPRINT_PID_ENDDATAPACKET 0x08 //!< End of data packet
//----------------------------------------------------------------------------------------------------------------
// !< SYSTEM STATUS/CONDITIONS
//----------------------------------------------------------------------------------------------------------------
#define SYSTEM_FREE (uint16_t)(0 << 0)             //!< system is free
#define SYSTEM_BUSY (uint16_t)(1 << 0)             //!< system is busy
#define SYSTEM_FINGERPRINT_PASS (uint16_t)(1 << 1) //!< matching fingerprint found
#define SYSTEM_HNDSHK_VERIFIED (uint16_t)(1 << 2)  //!< [0xFF] handshaking password verified
#define SYSTEM_IMGBUF_VALID (uint16_t)(1 << 3)     //!< imagebuffer doesnot have a valid image
//----------------------------------------------------------------------------------------------------------------
#define UART_PORT_ON (uint8_t)1  //!< Uart port is ON
#define UART_PORT_OFF (uint8_t)0 //!< Uart port is OFF
//----------------------------------------------------------------------------------------------------------------
// !< Custom enum for status response after executing a command >
typedef enum fingerprint_status
{
    FINGERPRINT_FAIL = 0,
    FINGERPRINT_OK,
} fingerprint_status_t;
//----------------------------------------------------------------------------------------------------------------
// !< Custom enum indicating the current operation phase >
typedef enum e_fingerprint_op_mode
{
    FINGERPRINT_MATCH_MODE = 0,
    FINGERPRINT_ENROLLMENT_MODE,
    FINGERPRINT_LIST_IDS_MODE,
    FINGERPRINT_ERASE_WITH_IDS_MODE,
    FINGERPRINT_ERASE_ALL_MODE,
    FINGERPRINT_MODE_MAX
} e_fingerprint_op_mode_t;

//----------------------------------------------------------------------------------------------------------------
// !< Custom item_ids >
typedef enum e_sensor_fp_items
{
    SENSOR_FP_ITEM_ID_ENROLL,
    SENSOR_FP_ITEM_ID_ACTION,
    SENSOR_FP_ITEM_ID_FP_IDS,
    SENSOR_FP_ITEM_ID_MAX,
} e_sensor_fp_items_t;
//----------------------------------------------------------------------------------------------------------------
// !< Custom structure to send as a reply to server (@ MATCH phase) >
typedef struct server_packet_t
{
    volatile e_fingerprint_op_mode_t opmode;                    /* Hold current operation mode*/
    volatile bool __busy_guard;                                 /* Gaurd_flag used during notification actions*/
    volatile bool notify_flag;                                  /* It triggers a reply after set_action*/
    uint8_t intr_pin;                                           /* Stores custom interrupt pin num*/
    uint16_t confidence_level;                                  /* 0~100*/
    uint16_t matched_confidence_level;                          /* Used to store most recently matched confidence*/
    uint16_t user_id;                                           /* Stores: Template or character_page ID (0~999)*/
    uint16_t matched_id;                                        /* Used to store most recently matched ID*/
    uint8_t recieved_buffer[MAX_PACKET_LENGTH_VAL];             /* This array store incomming uart message*/
    volatile bool protect[FINGERPRINT_MAX_CAPACITY_LIMIT + 1];  /* Array indicate which index to protect*/
    volatile bool validity[FINGERPRINT_MAX_CAPACITY_LIMIT + 1]; /* status of each ID {1~500} ; [ true -> occupied]*/
    time_t timeout_start_time;                                  /* Variable to store immediate time value*/
    TaskHandle_t notifyHandler;                                 /* Notify handler*/
    esp_timer_handle_t timerHandler;
    e_sensor_fp_items_t sensor_fp_item_ids[SENSOR_FP_ITEM_ID_MAX];
} server_packet_t;

//-------------------------------------------------------------------------------------------------------------------
//  FUNCTIONS Defination for Fingerprint Library
//-------------------------------------------------------------------------------------------------------------------

#if 0
// bool UpImage(int uart_channel_num, uint8_t *recieved_buffer, uint32_t timeout);
// bool DownImage(int uart_channel_num, uint8_t *recieved_buffer, uint32_t timeout);
// bool SetPwd(int uart_channel_num, uint32_t new_password, uint8_t *recieved_buffer, uint32_t timeout);
// bool UpChar(int uart_channel_num, uint8_t CharBufferID, uint8_t *recieved_buffer, uint32_t timeout);
// bool DownChar(int uart_channel_num, uint8_t CharBufferID, uint8_t *recieved_buffer, uint32_t timeout);
#endif

/**
 * @brief #### This function delete all the templates in the Flash library.
 *
 * @param (int)uart_channel_num: The UART channel number
 * @param (uint8_t*)recieved_buffer: Holds the address to a uart_buffer with recieved and filtered message
 * @param (uint32_t)timeout: Timeout(N*1ms) for uart message polling
 *
 * @return succcess[>0] or failure[0]
 */
bool Empty(int uart_channel_num, uint8_t *recieved_buffer, uint32_t timeout);

/**
 * @brief #### This function mannually detecting finger and store the detected finger image in ImageBuffer.
 *
 * @param (int)uart_channel_num: The UART channel number
 * @param (uint8_t*)recieved_buffer: Holds the address to a uart_buffer with recieved and filtered message
 * @param (uint32_t)timeout: Timeout(N*1ms) for uart message polling
 *
 * @return succcess[>0] or failure[0]
 */
bool GenImg(int uart_channel_num, uint8_t *recieved_buffer, uint32_t timeout);

/**
 * @brief #### This function mannually combine information of character files from CharBuffer1 and CharBuffer2 ; Then generate a template which is stored back in both CharBuffer1 and CharBuffer2.
 *
 * @param (int)uart_channel_num: The UART channel number
 * @param (uint8_t*)recieved_buffer: Holds the address to a uart_buffer with recieved and filtered message
 * @param (uint32_t)timeout: Timeout(N*1ms) for uart message polling
 *
 * @return succcess[>0] or failure[0]
 */
bool RegModel(int uart_channel_num, uint8_t *recieved_buffer, uint32_t timeout);

/**
 * @brief #### This function Turn Led ON/OFF .
 *
 * @param uart_channel_num(int): The UART channel number
 * @param LED_state(bool): 1 => [ON] ; 0 => [0FF]
 * @param recieved_buffer(uint8_t*): Holds the address to a uart_buffer with recieved and filtered message
 * @param timeout(uint32_t): Timeout(N*1ms) for uart message polling
 *
 * @return succcess[>0] or failure[0]
 */
bool LedControl(int uart_channel_num, bool LED_state, uint8_t *recieved_buffer, uint32_t timeout);

/**
 * @brief #### This function mannually generate character file from the original finger image in ImageBuffer & store the file in [CharBuffer1 or CharBuffer2].
 *
 * @param (int)uart_channel_num: The UART channel number
 * @param (uint8_t)CharBufferID: Character file buffer number [Chrbuff1 = 1h ; Chrbuff1 = 2h] // [NOTE : BufferID of CharBuffer1 and CharBuffer2 are 1h and 2h respectively. Other values (except 1h, 2h) would be processed as CharBuffer2]
 * @param (uint8_t*)recieved_buffer: Holds the address to a uart_buffer with recieved and filtered message
 * @param (uint32_t)timeout: Timeout(N*1ms) for uart message polling
 *
 * @return succcess[>0] or failure[0]
 */
bool Img2Tz(int uart_channel_num, uint8_t CharBufferID, uint8_t *recieved_buffer, uint32_t timeout);

/**
 * @brief #### This function return Total valid template number.
 *
 * @param (int)uart_channel_num: The UART channel number
 * @param (uint16_t*)TempNum: Reads and Stores, current valid template number of the Module,in this address
 * @param (uint8_t*)recieved_buffer: Holds the address to a uart_buffer with recieved and filtered message
 * @param (uint32_t)timeout: Timeout(N*1ms) for uart message polling
 *
 * @return succcess[>0] or failure[0] , *TempNum <= 0bxxx.
 */
bool ReadTempNum(int uart_channel_num, uint16_t *TempNum, uint8_t *recieved_buffer, uint32_t timeout);

/**
 * @brief #### This function Sets Module Address.
 *
 * @param (int)uart_channel_num: The UART channel number
 * @param (uint32_t)new_address: New address of module
 * @param (uint8_t*)recieved_buffer: Holds the address to a uart_buffer with recieved and filtered message
 * @param (uint32_t)timeout: Timeout(N*1ms) for uart message polling
 *
 * @return succcess[>0] or failure[0]
 */
bool SetAdder(int uart_channel_num, uint32_t new_address, uint8_t *recieved_buffer, uint32_t timeout);

/**
 * @brief #### This function Verifyies Module handshaking password.
 *
 * @param (int)uart_channel_num: The UART channel number
 * @param (uint32_t)the_password: Value as a password
 * @param (uint8_t*)recieved_buffer: Holds the address to a uart_buffer with recieved and filtered message
 * @param (uint32_t)timeout: Timeout(N*1ms) for uart message polling
 *
 * @return succcess[>0] or failure[0]
 */
bool VerifyPwd(int uart_channel_num, uint32_t the_password, uint8_t *recieved_buffer, uint32_t timeout);

/**
 * @brief #### This function executes precise matching of templates from CharBuffer1 and CharBuffer2.
 *
 * @param (int)uart_channel_num: The UART channel number
 * @param (uint16_t*)InspectionScore: Holds the address to a buffer, which store the score after inpection-matching of templates in ChrBuffer-1&2
 * @param (uint8_t*)recieved_buffer: Holds the address to a uart_buffer with recieved and filtered message
 * @param (uint32_t)timeout: Timeout(N*1ms) for uart message polling
 *
 * @return succcess[>0] or failure[0] , *InspectionScore <= XXXX.
 */
bool Match(int uart_channel_num, uint16_t *InspectionScore, uint8_t *recieved_buffer, uint32_t timeout);

/**
 * @brief #### This function Sets UART Port ON/OFF.
 *
 * @param (int)uart_channel_num: The UART channel number
 * @param (uint8_t)Control_code: Choose [ON/OFF] Uart_Port -> [1/0]
 * @param (uint8_t*)recieved_buffer: Holds the address to a uart_buffer with recieved and filtered message
 * @param (uint32_t)timeout: Timeout(N*1ms) for uart message polling
 *
 * @return succcess[>0] or failure[0]
 */
bool PortControl(int uart_channel_num, uint8_t Control_code, uint8_t *recieved_buffer, uint32_t timeout);

/**
 * @brief #### This function Reads System Parameters.
 *
 * @param (int)uart_channel_num: The UART channel number
 * @param (uint16_t*)Status_bits: Holds the address to buffer, where extracted system's status bits gets stored
 * @param (uint8_t*)recieved_buffer: Holds the address to a uart_buffer with recieved and filtered message
 * @param (uint32_t)timeout: Timeout(N*1ms) for uart message polling
 *
 * @return succcess[>0] or failure[0] ;  *Status_bits <= 0bxxxxxxxx0000.
 */
bool ReadSysPara(int uart_channel_num, uint16_t *Status_bits, uint8_t *recieved_buffer, uint32_t timeout);

/**
 * @brief #### This function mannually loads template at the specified (PageID) of Flash library to => template buffer [CharBuffer1/CharBuffer2].
 *
 * @param (int)uart_channel_num: The UART channel number
 * @param (uint8_t)CharBufferID: Character file buffer number [Chrbuff1 = 1h ; Chrbuff1 = 2h]
 * @param (uint16_t)PageID: PAGE_ID location of the template, inside Library [two bytes :- high byte (MSB) front & low byte (LSB) behind]
 * @param (uint8_t*)recieved_buffer: Holds the address to a uart_buffer with recieved and filtered message
 * @param (uint32_t)timeout: Timeout(N*1ms) for uart message polling
 *
 * @return succcess[>0] or failure[0]
 */
bool Load(int uart_channel_num, uint8_t CharBufferID, uint16_t PageID, uint8_t *recieved_buffer, uint32_t timeout);

/**
 * @brief #### This function deletes a segment:-(N) templates of Flash library started from the specified location (or PageID).
 *
 * @param (int)uart_channel_num: The UART channel number
 * @param (uint16_t)PageID: (Start)Flash location of the template [two bytes :- high byte (MSB) front & low byte (LSB) behind]
 * @param (uint16_t)TempCount: No of templates to delele.
 * @param (uint8_t*)recieved_buffer: Holds the address to a uart_buffer with recieved and filtered message
 * @param (uint32_t)timeout: Timeout(N*1ms) for uart message polling
 *
 * @return succcess[>0] or failure[0]
 */
bool Delete(int uart_channel_num, uint16_t PageID, uint16_t TempCount, uint8_t *recieved_buffer, uint32_t timeout);

/**
 * @brief #### This function mannually stores the template of specified buffer (Buffer1/Buffer2) at the designated location of Flash library.
 *
 * @param (int)uart_channel_num: The UART channel number
 * @param (uint8_t)CharBufferID: Character file buffer number [Chrbuff1 = 1h ; Chrbuff1 = 2h]
 * @param (uint16_t)PageID: Flash location of the template [two bytes :- high byte (MSB) front & low byte (LSB) behind]
 * @param (uint8_t*)recieved_buffer: Holds the address to a uart_buffer with recieved and filtered message
 * @param (uint32_t)timeout: Timeout(N*1ms) for uart message polling
 *
 * @return succcess[>0] or failure[0]
 */
bool Store(int uart_channel_num, uint8_t CharBufferID, uint16_t PageID, uint8_t *recieved_buffer, uint32_t timeout);

/**
 * @brief #### This function Auto-collects fingeprint and matches the captured fingerprint with one stored in library.
 *
 * @param (int)uart_channel_num: The UART channel number
 * @param (uint16_t*)PageID_ptr: This address points to value of Page_value(inside FingerPrint_library) of current matched fingerprint.
 * @param (uint16_t*)MatchScore_ptr: This address points to the confidence_level of current matched fingerprint.
 * @param (uint8_t*)recieved_buffer: Holds the address to a uart_buffer with recieved and filtered message
 * @param (uint32_t)timeout: Timeout(N*1ms) for uart message polling
 *
 * @return succcess[>0] or failure[0] , *PageID_ptr <= XXXX , *MatchScore_ptr <= XXXX.
 */
bool GR_Identify(int uart_channel_num, uint16_t *PageID_ptr, uint16_t *MatchScore_ptr, uint8_t *recieved_buffer, uint32_t timeout);

/**
 * @brief #### This function Sets System Parameters.
 *
 * @param (int)uart_channel_num: The UART channel number
 * @param (uint8_t)Parameter_Number: This value targets the parameter [Baud_rate_control:4 / Security_Level:5 / Data_package_length:6]
 * @param (uint8_t)Parameter_Content: Choose the new setting for designated 'Parameter_Number'
 * @param (uint8_t*)recieved_buffer: Holds the address to a uart_buffer with recieved and filtered message
 * @param (uint32_t)timeout: Timeout(N*1ms) for uart message polling
 *
 * @return succcess[>0] or failure[0]
 */
bool SetSysPara(int uart_channel_num, uint8_t Parameter_Number, uint8_t Parameter_content, uint8_t *recieved_buffer, uint32_t timeout);

/**
 * @brief #### This function searchs the whole finger library for the template that matches the one in CharBuffer1 or CharBuffer2. When found, PageID will be returned.
 *
 * @param (int)uart_channel_num: The UART channel number
 * @param (uint8_t)CharBufferID: Character_file ID [Chrbuff1 = 1h ; Chrbuff1 = 2h], containing the template you want to search
 * @param (uint16_t)StartPage: Searching start address [0~ max fingerprint capacity] (MSB first).
 * @param (uint16_t)PageNum: Searching Quantity [0 ~ (N-1) max fingerprint capacity] (MSB first).
 * @param (uint16_t*)PageID_ptr: This address will point to, value of Page_value(inside FingerPrint_library) of current matched fingerprint.
 * @param (uint16_t*)MatchScore_ptr: This address will point to, the confidence_level of current matched fingerprint.
 * @param (uint8_t*)recieved_buffer: Holds the address to a uart_buffer with recieved and filtered message
 * @param (uint32_t)timeout: Timeout(N*1ms) for uart message polling
 *
 * @return succcess[>0] or failure[0] , *PageID_ptr <= XXXX , *MatchScore_ptr <= XXXX.
 */
bool Search(int uart_channel_num, uint8_t CharBufferID, uint16_t StartPage, uint16_t PageNum, uint16_t *PageID_ptr, uint16_t *MatchScore_ptr, uint8_t *recieved_buffer, uint32_t timeout);

//-------------------------------------------------------------------------------------------------------------------
// FUNCTIONS Defination for Operation modes
//-------------------------------------------------------------------------------------------------------------------

/**
 * @brief #### This function extracts fingerprint after interrupt signal and then
 */
bool r307_as606_match_id(l_ezlopi_item_t *item);

/**
 * @brief #### This function erases fingerprint from internal libraray
 *
 */
bool r307_as606_erase_all_id(l_ezlopi_item_t *item);

/**
 * @brief #### This Function searches and erases specified range of IDs only.
 */
bool r307_as606_erase_specified_id(l_ezlopi_item_t *item);

/**
 * @brief #### This function checks the perticular USER/PAGE_ID .
 * @return {true ==> empty} / {false == not empty}
 */
bool r307_as606_check_pageid_empty(l_ezlopi_item_t *item);

/**
 * @brief #### This function Scans and update validity status of [1~500(max defined)] PAGE_IDs.
 */
bool r307_as606_update_id_status_list(l_ezlopi_item_t *item);

/**
 * @brief This function store only valid fingerprint, in vacant PAGEID
 * @paragraph IF match_% < 20 ; storing takes place
 * @paragraph IF match_% > 20 ; Duplicate_ID is returened *
 * @return {0} => Unsucessful_cmds ; {same_id} => Successfully_stored ; {different_id} => Duplicate_ID
 */
uint16_t r307_as606_enroll_fingerprint(l_ezlopi_item_t *item);

/**
 * @brief #### This function finds immediate vaccant ID.
 *
 * @return {0 => vaccant IDs not found}
 */
uint16_t r307_as606_find_immediate_vaccant_id(l_ezlopi_item_t *item);

/**
 * @brief This function Configure the fingerprint sensors
 * @return {success => FINGERPRINT_OK} ; { fail => FINGERPRINT_FAIL}
 */
fingerprint_status_t r307_as606_fingerprint_config(l_ezlopi_item_t *item);

/**
 * @brief #### This function checks if system is in free.
 *
 * @param (int)uart_channel_num: The uart channel number
 * @param (uint32_t)timeout_ms: Timeout(N*1ms) for uart message polling
 *
 * @return [succcess='true'] & [failure='false']
 */
bool r307_as606_wait_till_system_free(l_ezlopi_item_t *item, uint32_t timeout);

//-------------------------------------------------------------------------------------------------------------------
int sensor_0066_other_R307_FingerPrint(e_ezlopi_actions_t action, l_ezlopi_item_t *item, void *arg, void *user_arg);

#endif