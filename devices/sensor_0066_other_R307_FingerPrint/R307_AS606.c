#include "string.h"
#include "trace.h"
#include "ezlopi_uart.h"
#include "sensor_0066_other_R307_FingerPrint.h"

//---------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
// Function for Communication-Packet (Generation and Response).
//---------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------

/**
 * @brief This is a function generates/prepares appropriate transmission packets actions.
 *
 * @param txPacket(fingerprint_packet_t*): Address to a buffer, that stores the generated transmission packet
 * @param PID(uint8_t): Packet Identifier Type
 * @param length(uint16_t): Packet_length value (Combined_data only)
 * @param Combined_data(uint8_t*): Address to buffer containing, [instruction code + data values (if any)]
 *
 * @return Success=[>0] / Fail=[0].
 */
static void generate_packet(fingerprint_packet_t *txPacket, uint8_t PID, uint16_t length, uint8_t *Combined_data)
{
    //------------ PID ------------------------------------------------------------------------------------------------------
    txPacket->PID = PID;
    //------------ Packet length -------------------------------------------------------------------------------------------
    length = length + 2;
    txPacket->Packet_len[0] = (uint8_t)(length >> 8);   /*MSB [PID + P_LEN + (data_fields)] should not exceed 256bytes*/
    txPacket->Packet_len[1] = (uint8_t)(length & 0xFF); /*LSB [PID + P_LEN + (data_fields)] should not exceed 256bytes*/
    length = length - 2;
    //------------ Inst_code + Data_content  --------------------------------------------------------------------------------
    if (length < MAX_PACKET_LENGTH_VAL)
    {
        memcpy(&(txPacket->data), Combined_data, ((int)length)); // Copying only data section
    }
    else
    {
        memcpy(&(txPacket->data), Combined_data, (MAX_PACKET_LENGTH_VAL - 2));
    }
    //------------ Checksum -------------------------------------------------------------------------------------------------
    // CHK_SUM calculation
    uint16_t sum = (uint16_t)PID + ((uint16_t)(txPacket->Packet_len[0] << 8) + (uint16_t)(txPacket->Packet_len[1] & 0xFF)); // adding
    for (uint8_t i = 0; i < (int)length; i++)
    {
        sum += (uint16_t)(txPacket->data[i]);
    }
    txPacket->chk_sum[0] = (uint8_t)(sum >> 8);   // MSB
    txPacket->chk_sum[1] = (uint8_t)(sum & 0xFF); // LSB
    //----------------------------------------------------------------------------------------------------------------------
}
/**
 * @brief This is a function to execute uart_packet transfer actions :- through UART_buffer
 *
 * @param uart_channel_num(int): uart_channel number
 * @param txPacket(fingerprint_packet_t*): Address to the buffer holding, Transmission packet
 *
 * @return Success=[>0] / Fail=[0].
 */
static int send_uart_packets(int uart_channel_num, fingerprint_packet_t *txPacket)
{
    int ret = 0;
    int len = ((int)((txPacket->Packet_len[0] << 8) + (txPacket->Packet_len[1] & 0xFF)) - 2);

    // 1. Header seciton
    ret = uart_write_bytes(uart_channel_num, txPacket->header_code, 2);
    ret = uart_write_bytes(uart_channel_num, txPacket->device_address, 4);
    ret = uart_write_bytes(uart_channel_num, &(txPacket->PID), 1);
    ret = uart_write_bytes(uart_channel_num, txPacket->Packet_len, 2);
    // 2. data_section (Inst_code + Data_content)
    ret = uart_write_bytes(uart_channel_num, &(txPacket->data), len);
    // 3. Checksum
    ret = uart_write_bytes(uart_channel_num, txPacket->chk_sum, 2);

    //----------------------------------------------------------------------------------------------------------------------
    uart_wait_tx_done(uart_channel_num, 100); // wait timeout is 100 RTOS ticks (TickType_t)
    return ret;
}

/**
 * @brief This is a function to execute sequential actions :- generate packet and send_packet them through UART_buffer
 *
 * @param uart_channel_num(int): uart_channel number
 * @param txPacket(fingerprint_packet_t*): Address to the buffer holding, Transmission packet
 * @param PID(uint8_t): Packet Identifier Type
 * @param length(uint16_t): Packet_length value (Combined_data + chk_sum)
 * @param Combined_data(uint8_t*): Address to buffer containing, [instruction code + data values (if any)]
 *
 * @return Success=[true] / Fail=[false].
 */
static bool SEND_PACKET(int uart_channel_num, fingerprint_packet_t *txPacket, uint8_t PID, uint16_t length, uint8_t *Combined_data)
{
    generate_packet(txPacket, PID, length, Combined_data);
    if (FINGERPRINT_FAIL != send_uart_packets(uart_channel_num, txPacket))
    {
        return true;
    }
    return false;
}

/**
 * @brief Additional function that waits  for recieve_buffer to fill up, (utill 'time-out')  and return appropriate responses.
 *
 * @param (uint8_t*)recieved_buffer: Holds the address to a uart_buffer with recieved and filtered message
 * @param (uint32_t)timeout: Timeout(N*1ms) for uart message polling
 *
 * @return [FINGERPRINT_OK : Successful] // [FINGERPRINT_FAIL : Error]
 */
static fingerprint_status_t __Response_function(uint8_t *recieved_buffer, uint32_t timeout)
{
    fingerprint_status_t F_res = FINGERPRINT_FAIL;
    uint32_t start_time = esp_timer_get_time() / 1000;
    uint32_t dummy_timer = 0;
    while (dummy_timer <= timeout)
    {
        //------------ check if the recieved PID is ack packet  --------------------------------------------------------------------------------
        if (FINGERPRINT_PID_ACKPACKET == recieved_buffer[0])
        {
            // TRACE_W(" ----> Response : PID : %d", recieved_buffer[0]);
            break; // break away from while() , if we have the correct buffer values
        }
        dummy_timer = (esp_timer_get_time() / 1000) - start_time;
        vTaskDelay(10 / portTICK_PERIOD_MS); // 200ms
    }
    //------------ Check 'Confirmation code' of the ack packet: And give response --------------------------------------------------------------------------------
    if (!(dummy_timer > timeout) && (FINGERPRINT_PID_ACKPACKET == recieved_buffer[0]))
    {
        /**
         * recieved_buffer[0] => PID
         * recieved_buffer[1] => Package_len [MSB]
         * recieved_buffer[2] => Package_len [LSB]
         * recieved_buffer[3] => Confirmation_code  (1byte)
         * recieved_buffer[4+0 ; {0 to (p_len-4)}] => data1
         * recieved_buffer[4+1 ; {0 to (p_len-4)}] => data2
         * ...
         * recieved_buffer[N] = Checksum  (2byte)
         */
        /*check confirmation code*/
        switch ((uint8_t)recieved_buffer[3])
        {
        case ACK_OK:
            TRACE_I("[.....Commad execution SUCCESS.....] ");
            F_res = FINGERPRINT_OK;
            break;
        case ACK_ERR_RECV:
            TRACE_E(".... ERR in recieving Package ... ");
            break;
        case ACK_ERR_DETECT_FP:
            TRACE_E(".... ERR: Can't detect finger ... ");
            break;
        case ACK_ERR_ENROLL_FP:
            TRACE_E(".... ERR: Failed to Collect/Enroll finger ... ");
            break;
        case ACK_ERR_DIS_FP:
            TRACE_E(".... ERR: Failed to generate character file (distorted fingerprint-img) ... ");
            break;
        case ACK_ERR_SMALL_FP:
            TRACE_E(".... ERR: Failed to generate character file (lackness of character point or over-smallness of fingerprint) ... ");
            break;
        case ACK_ERR_MATCH:
            TRACE_E(".... ERR: Templates from both Charbuffers(1 & 2) arenot matching ... ");
            break;
        case ACK_ERR_NO_LIB_MATCH:
            // TRACE_E(".... ERR: Not matching with the library (both the PageID and matching score are 0) ... ");
            break;
        case ACK_ERR_CMB_CHRFILE:
            TRACE_E(".... ERR: Failed to combine the character files (character files donot belong to same finger) ... ");
            break;
        case ACK_ERR_ID_BYND_LIB:
            TRACE_E(".... ERR: Addressed PageID is beyond the finger library ... ");
            break;
        case ACK_ERR_LOAD_CHR:
            // TRACE_E(".... ERR: Failed to load temp/chr_file from finger library ... ");
            break;
        case ACK_ERR_UP_CHR:
            TRACE_E(".... ERR: Failed to upload character_file (uploading template to computer) ... ");
            break;
        case ACK_ERR_DWN_IMG_CHR:
            TRACE_E(".... ERR: Failed to recieve data packet (downloading image or character) ... ");
            break;
        case ACK_ERR_UP_IMG:
            TRACE_E(".... ERR: Failed to upload image (uploading image to computer) ... ");
            break;
        case ACK_ERR_DEL:
            TRACE_E(".... ERR: Failed to Delete (N)Templates From PageID ... ");
            break;
        case ACK_ERR_CLR:
            TRACE_E(".... ERR: Failed to Clear all Templates From finger library ... ");
            break;
        case ACK_ERR_WRONG_PASS:
            TRACE_E(".... ERR in Password .. ");
            break;
        case ACK_ERR_PRIM_IMG:
            TRACE_E(".... ERR: Failed to generate IMG (lack of valid primary image) ... ");
            break;
        case ACK_ERR_STORE:
            TRACE_E(".... ERR: When writing/storing template into flash library ... ");
            break;
        case ACK_ERR_WRNG_REGS:
            TRACE_E(".... ERR: wrong register number .. ");
            break;
        case ACK_ERR_OP_FAIL:
            TRACE_E(".... ERR: Failed to operate communication ... ");
            break;
        default:
            TRACE_E(".... ERR: Unknown ... ");
            break;
        }
    }
    else
    {
        TRACE_W("SEND_PACKET: ....Time-out");
    }
    return F_res;
}

//---------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
// Function for Fingerprint Library
//---------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------

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
bool VerifyPwd(int uart_channel_num, uint32_t the_password, uint8_t *recieved_buffer, uint32_t timeout)
{
    // Reset the recieving buffer before new data is to be extracted.
    memset(recieved_buffer, 0, MAX_PACKET_LENGTH_VAL);
    // create the packet here
    fingerprint_status_t F_res = FINGERPRINT_FAIL;

    fingerprint_packet_t *txPacket = (fingerprint_packet_t *)malloc(sizeof(fingerprint_packet_t));
    if (txPacket)
    {
        txPacket->header_code[0] = FINGERPRINT_HEADER_MSB;
        txPacket->header_code[1] = FINGERPRINT_HEADER_LSB;
        txPacket->device_address[0] = txPacket->device_address[1] = txPacket->device_address[2] = txPacket->device_address[3] = FINGERPRINT_DEVICE_ADDR_BIT;

        //------------ Inst_code + Data_content  --------------------------------------------------------------------------------
        uint8_t Combined_data[5] = {
            FINGERPRINT_VERIFYPASSWORD,     /*INS CODE [1Byte]*/
            (uint8_t)(the_password >> 24),  /*MSB sent first*/
            (uint8_t)(the_password >> 16),  /*MSB sent first*/
            (uint8_t)(the_password >> 8),   /*MSB sent first*/
            (uint8_t)(the_password & 0xFF), /*LSB sent last*/
        };

        // TRACE_B("                          -------- VerifyPwd -------");
        bool res = SEND_PACKET(uart_channel_num,                  /* UART CHANNEL NUMBER */
                               txPacket,                          /* Address of packet container */
                               FINGERPRINT_PID_COMMANDPACKET,     /* Packet Identifier CMD*/
                               (uint16_t)(sizeof(Combined_data)), /* length <= combined_data*/
                               Combined_data);                    /* Inst_code + Data_content*/
        //------------ Check of the appropriate responce  --------------------------------------------------------------------------------
        if (res)
        {
            TRACE_W("--------------- 'VerifyPwd' Response ----------------");
            F_res = __Response_function(recieved_buffer, timeout);
            if (FINGERPRINT_OK == F_res)
            {
                uint16_t Checksum = ((uint16_t)recieved_buffer[4] << 8) + ((uint16_t)recieved_buffer[5] & 0xFF); //(1~12)
                TRACE_D("VerifyPwd =>Checksum [4]: %#x", Checksum);
            }
            // TRACE_W("---------------------------------------------------");
        }
        // TRACE_B("                          -------- XXXX --------");
        free(txPacket);
    } //----------------------------------------------------------------------------------------------------------------------
    return (bool)F_res;
}

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
bool SetAdder(int uart_channel_num, uint32_t new_address, uint8_t *recieved_buffer, uint32_t timeout)
{
    // Reset the recieving buffer before new data is to be extracted.
    memset(recieved_buffer, 0, MAX_PACKET_LENGTH_VAL);
    // create the packet here
    fingerprint_status_t F_res = FINGERPRINT_FAIL;

    fingerprint_packet_t *txPacket = (fingerprint_packet_t *)malloc(sizeof(fingerprint_packet_t));
    if (txPacket)
    {
        txPacket->header_code[0] = FINGERPRINT_HEADER_MSB;
        txPacket->header_code[1] = FINGERPRINT_HEADER_LSB;
        txPacket->device_address[0] = txPacket->device_address[1] = txPacket->device_address[2] = txPacket->device_address[3] = FINGERPRINT_DEVICE_ADDR_BIT;

        //------------ Inst_code + Data_content  --------------------------------------------------------------------------------
        uint8_t Combined_data[5] = {
            FINGERPRINT_SETADDRESS,        /*INS CODE [1Byte]*/
            (uint8_t)(new_address >> 24),  /*MSB sent first*/
            (uint8_t)(new_address >> 16),  /*MSB sent first*/
            (uint8_t)(new_address >> 8),   /*MSB sent first*/
            (uint8_t)(new_address & 0xFF), /*LSB sent last*/
        };

        // TRACE_B("                          -------- SetAdder --------");
        bool res = SEND_PACKET(uart_channel_num,                  /* UART CHANNEL NUMBER */
                               txPacket,                          /* Address of packet container */
                               FINGERPRINT_PID_COMMANDPACKET,     /* Packet Identifier CMD*/
                               (uint16_t)(sizeof(Combined_data)), /* length <= combined_data*/
                               Combined_data);                    /* Inst_code + Data_content*/
        //------------ Check of the appropriate responce  --------------------------------------------------------------------------------
        if (res)
        {
            TRACE_W("--------------- 'SetAdder' Response ----------------");
            F_res = __Response_function(recieved_buffer, timeout);
            if (FINGERPRINT_OK == F_res)
            {
                uint16_t Checksum = ((uint16_t)recieved_buffer[4] << 8) + ((uint16_t)recieved_buffer[5] & 0xFF); //(1~12)
                TRACE_D("SetAdder =>Checksum [4]: %#x", Checksum);
            }
            // TRACE_W("---------------------------------------------------");
        }
        // TRACE_B("                          -------- XXXX --------");
        free(txPacket);
    }
    //----------------------------------------------------------------------------------------------------------------------
    return (bool)F_res;
}

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
bool SetSysPara(int uart_channel_num, uint8_t Parameter_Number, uint8_t Parameter_content, uint8_t *recieved_buffer, uint32_t timeout)
{
    // Reset the recieving buffer before new data is to be extracted.
    memset(recieved_buffer, 0, MAX_PACKET_LENGTH_VAL);
    // create the packet here
    fingerprint_status_t F_res = FINGERPRINT_FAIL;

    fingerprint_packet_t *txPacket = (fingerprint_packet_t *)malloc(sizeof(fingerprint_packet_t));
    if (txPacket)
    {
        txPacket->header_code[0] = FINGERPRINT_HEADER_MSB;
        txPacket->header_code[1] = FINGERPRINT_HEADER_LSB;
        txPacket->device_address[0] = txPacket->device_address[1] = txPacket->device_address[2] = txPacket->device_address[3] = FINGERPRINT_DEVICE_ADDR_BIT;

        //------------ Inst_code + Data_content  --------------------------------------------------------------------------------
        uint8_t Combined_data[3] = {
            FINGERPRINT_SETSYSPARAM, /*INS CODE [1Byte]*/
            Parameter_Number,        /* Paramter Number */
            Parameter_content,       /* Paramter Content */
        };

        // TRACE_B("                          -------- SetSysPara -------");
        bool res = SEND_PACKET(uart_channel_num,                  /* UART CHANNEL NUMBER */
                               txPacket,                          /* Address of packet container */
                               FINGERPRINT_PID_COMMANDPACKET,     /* Packet Identifier CMD*/
                               (uint16_t)(sizeof(Combined_data)), /* length <= combined_data*/
                               Combined_data);                    /* Inst_code + Data_content*/
        //------------ Check of the appropriate responce  --------------------------------------------------------------------------------
        if (res)
        {
            TRACE_W("--------------- 'SetSysPara' Response ----------------");
            F_res = __Response_function(recieved_buffer, timeout);
            if (FINGERPRINT_OK == F_res)
            {
                uint16_t Checksum = ((uint16_t)recieved_buffer[4] << 8) + ((uint16_t)recieved_buffer[5] & 0xFF); //(1~12)
                TRACE_D("SetSysPara =>Checksum [4]: %#x", Checksum);
            }
            // TRACE_W("------------------------------------------------------");
        }
        // TRACE_B("                          -------- XXXX --------");
        free(txPacket);
    }
    //----------------------------------------------------------------------------------------------------------------------
    return (bool)F_res;
}

/**
 * @brief #### This function Sets System Parameters.
 *
 * @param (int)uart_channel_num: The UART channel number
 * @param (uint8_t)Control_code: Choose [ON/OFF] Uart_Port -> [1/0]
 * @param (uint8_t*)recieved_buffer: Holds the address to a uart_buffer with recieved and filtered message
 * @param (uint32_t)timeout: Timeout(N*1ms) for uart message polling
 *
 * @return succcess[>0] or failure[0]
 */
bool PortControl(int uart_channel_num, uint8_t Control_code, uint8_t *recieved_buffer, uint32_t timeout)
{
    // Reset the recieving buffer before new data is to be extracted.
    memset(recieved_buffer, 0, MAX_PACKET_LENGTH_VAL);
    // create the packet here
    fingerprint_status_t F_res = FINGERPRINT_FAIL;

    fingerprint_packet_t *txPacket = (fingerprint_packet_t *)malloc(sizeof(fingerprint_packet_t));
    if (txPacket)
    {
        txPacket->header_code[0] = FINGERPRINT_HEADER_MSB;
        txPacket->header_code[1] = FINGERPRINT_HEADER_LSB;
        txPacket->device_address[0] = txPacket->device_address[1] = txPacket->device_address[2] = txPacket->device_address[3] = FINGERPRINT_DEVICE_ADDR_BIT;

        //------------ Inst_code + Data_content  --------------------------------------------------------------------------------
        uint8_t Combined_data[2] = {
            FINGERPRINT_PORTCONTROL, /*INS CODE [1Byte]*/
            Control_code,            /* ON / OFF */
        };

        // TRACE_B("                          -------- PortControl ------");
        bool res = SEND_PACKET(uart_channel_num,                  /* UART CHANNEL NUMBER */
                               txPacket,                          /* Address of packet container */
                               FINGERPRINT_PID_COMMANDPACKET,     /* Packet Identifier CMD*/
                               (uint16_t)(sizeof(Combined_data)), /* length <= combined_data*/
                               Combined_data);                    /* Inst_code + Data_content*/
        //------------ Check of the appropriate responce  --------------------------------------------------------------------------------
        if (res)
        {
            TRACE_W("--------------- 'PortControl' Response ----------------");
            F_res = __Response_function(recieved_buffer, timeout);
            if (FINGERPRINT_OK == F_res)
            {
                uint16_t Checksum = ((uint16_t)recieved_buffer[4] << 8) + ((uint16_t)recieved_buffer[5] & 0xFF); //(1~12)
                TRACE_D("PortControl =>Checksum [4]: %#x", Checksum);
            }
            // TRACE_W("---------------------------------------------------");
        }
        // TRACE_B("                          -------- XXXX -------");
        free(txPacket);
    }
    //----------------------------------------------------------------------------------------------------------------------
    return (bool)F_res;
}

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
bool ReadSysPara(int uart_channel_num, uint16_t *Status_bits, uint8_t *recieved_buffer, uint32_t timeout)
{
    uint16_t Status_register = *Status_bits;
    // Reset the recieving buffer before new data is to be extracted.
    memset(recieved_buffer, 0, MAX_PACKET_LENGTH_VAL);
    // create the packet here
    fingerprint_status_t F_res = FINGERPRINT_FAIL;

    fingerprint_packet_t *txPacket = (fingerprint_packet_t *)malloc(sizeof(fingerprint_packet_t));
    if (txPacket)
    {
        txPacket->header_code[0] = FINGERPRINT_HEADER_MSB;
        txPacket->header_code[1] = FINGERPRINT_HEADER_LSB;
        txPacket->device_address[0] = txPacket->device_address[1] = txPacket->device_address[2] = txPacket->device_address[3] = FINGERPRINT_DEVICE_ADDR_BIT;

        //------------ Inst_code + Data_content  --------------------------------------------------------------------------------
        uint8_t Combined_data[1] = {
            FINGERPRINT_READSYSPARAM, /*INS CODE [1Byte]*/
        };

        //------------ Fill the packet container and send it via uart  --------------------------------------------------------------------------------
        bool res = SEND_PACKET(uart_channel_num,                  /* UART CHANNEL NUMBER */
                               txPacket,                          /* Address of packet container */
                               FINGERPRINT_PID_COMMANDPACKET,     /* Packet Identifier CMD*/
                               (uint16_t)(sizeof(Combined_data)), /* length <= combined_data*/
                               Combined_data);                    /* Inst_code + Data_content*/
        //------------ Check of the appropriate responce  --------------------------------------------------------------------------------
        if (res)
        {
            TRACE_W("--------------- 'ReadSysPara' Response ----------------");
            F_res = __Response_function(recieved_buffer, timeout);
            if (FINGERPRINT_OK == F_res)
            {
                Status_register = ((uint16_t)recieved_buffer[4] << 8) + ((uint16_t)recieved_buffer[5] & 0xFF);
                uint16_t System_identifier_code = ((uint16_t)recieved_buffer[6] << 8) + ((uint16_t)recieved_buffer[7] & 0xFF); // 0x0009
                uint16_t Finger_library_size = ((uint16_t)recieved_buffer[8] << 8) + ((uint16_t)recieved_buffer[9] & 0xFF);    // (0~999)
                uint16_t Security_level = ((uint16_t)recieved_buffer[10] << 8) + ((uint16_t)recieved_buffer[11] & 0xFF);       // (1~5)
                uint16_t Data_packet_size = ((uint16_t)recieved_buffer[16] << 8) + ((uint16_t)recieved_buffer[17] & 0xFF);     // (0~3)
                uint16_t Baud_setting = ((uint16_t)recieved_buffer[18] << 8) + ((uint16_t)recieved_buffer[19] & 0xFF);         //(1~12)
                uint16_t Checksum = ((uint16_t)recieved_buffer[20] << 8) + ((uint16_t)recieved_buffer[21] & 0xFF);             //(1~12)

                TRACE_D("Status_register [4]: %#x", Status_register);
                TRACE_D("System_identifier_code [6]: %#x", System_identifier_code);
                TRACE_D("Finger_library_size [8]: %d", (int)Finger_library_size);
                TRACE_D("Security_level [10]: %#x", Security_level);
                TRACE_D("Data_packet_size [16]: %#x", Data_packet_size);
                TRACE_D("Baud_setting [18]: %#x", Baud_setting);
                TRACE_D("ReadSysPara =>Checksum [20]: %#x", Checksum);
            }
        }
        // TRACE_B(" < -------- XXXX");
        free(txPacket);
    }
    *Status_bits = Status_register;
    //----------------------------------------------------------------------------------------------------------------------
    return (bool)F_res;
}

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
bool ReadTempNum(int uart_channel_num, uint16_t *TempNum, uint8_t *recieved_buffer, uint32_t timeout)
{
    uint16_t TempleteNum = 0;
    // Reset the recieving buffer before new data is to be extracted.
    memset(recieved_buffer, 0, MAX_PACKET_LENGTH_VAL);
    // create the packet here
    fingerprint_status_t F_res = FINGERPRINT_FAIL;

    fingerprint_packet_t *txPacket = (fingerprint_packet_t *)malloc(sizeof(fingerprint_packet_t));
    if (txPacket)
    {
        txPacket->header_code[0] = FINGERPRINT_HEADER_MSB;
        txPacket->header_code[1] = FINGERPRINT_HEADER_LSB;
        txPacket->device_address[0] = txPacket->device_address[1] = txPacket->device_address[2] = txPacket->device_address[3] = FINGERPRINT_DEVICE_ADDR_BIT;

        //------------ Inst_code + Data_content  --------------------------------------------------------------------------------
        uint8_t Combined_data[1] = {
            FINGERPRINT_TEMPLATENUM, /*INS CODE [1Byte]*/
        };

        // TRACE_B("                          -------- ReadTempNum --------");
        bool res = SEND_PACKET(uart_channel_num,                  /* UART CHANNEL NUMBER */
                               txPacket,                          /* Address of packet container */
                               FINGERPRINT_PID_COMMANDPACKET,     /* Packet Identifier CMD*/
                               (uint16_t)(sizeof(Combined_data)), /* length <= combined_data*/
                               Combined_data);                    /* Inst_code + Data_content*/
        //------------ Check of the appropriate responce  --------------------------------------------------------------------------------
        if (res)
        {
            TRACE_W("--------------- 'ReadTempNum' Response ----------------");
            F_res = __Response_function(recieved_buffer, timeout);
            if (FINGERPRINT_OK == F_res)
            {
                TempleteNum = ((uint16_t)recieved_buffer[4] << 8) + ((uint16_t)recieved_buffer[5] & 0xFF);
                uint16_t Checksum = ((uint16_t)recieved_buffer[6] << 8) + ((uint16_t)recieved_buffer[7] & 0xFF); //(1~12)
                TRACE_D("TempleteNum [4]: %#x", TempleteNum);
                TRACE_D("ReadTempNum =>Checksum [6]: %#x", Checksum);
            }
            // TRACE_W("---------------------------------------------------");
        }
        // TRACE_B("                          -------- XXXX --------");
        free(txPacket);
    }
    *TempNum = TempleteNum;
    //----------------------------------------------------------------------------------------------------------------------
    return (bool)F_res;
}

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
bool GR_Identify(int uart_channel_num, uint16_t *PageID_ptr, uint16_t *MatchScore_ptr, uint8_t *recieved_buffer, uint32_t timeout)
{
    uint16_t Page_num = 0;
    uint16_t Match_score = 0;
    // Reset the recieving buffer before new data is to be extracted.
    memset(recieved_buffer, 0, MAX_PACKET_LENGTH_VAL);
    // create the packet here
    fingerprint_status_t F_res = FINGERPRINT_FAIL;

    fingerprint_packet_t *txPacket = (fingerprint_packet_t *)malloc(sizeof(fingerprint_packet_t));
    if (txPacket)
    {
        txPacket->header_code[0] = FINGERPRINT_HEADER_MSB;
        txPacket->header_code[1] = FINGERPRINT_HEADER_LSB;
        txPacket->device_address[0] = txPacket->device_address[1] = txPacket->device_address[2] = txPacket->device_address[3] = FINGERPRINT_DEVICE_ADDR_BIT;

        //------------ Inst_code + Data_content  --------------------------------------------------------------------------------
        uint8_t Combined_data[1] = {
            FINGERPRINT_GR_IDENTIFY, /*INS CODE [1Byte]*/
        };

        // TRACE_B("                          -------- GR_Identify --------");
        bool res = SEND_PACKET(uart_channel_num,                  /* UART CHANNEL NUMBER */
                               txPacket,                          /* Address of packet container */
                               FINGERPRINT_PID_COMMANDPACKET,     /* Packet Identifier CMD*/
                               (uint16_t)(sizeof(Combined_data)), /* length <= combined_data*/
                               Combined_data);                    /* Inst_code + Data_content*/
        //------------ Check of the appropriate responce  --------------------------------------------------------------------------------
        if (res)
        {
            TRACE_W("--------------- 'GR_Identify' Response ----------------");
            F_res = __Response_function(recieved_buffer, timeout);
            if (FINGERPRINT_OK == F_res)
            {
                Page_num = ((uint16_t)recieved_buffer[4] << 8) + ((uint16_t)recieved_buffer[5] & 0xFF);
                Match_score = ((uint16_t)recieved_buffer[6] << 8) + ((uint16_t)recieved_buffer[7] & 0xFF);       // 0x0009
                uint16_t Checksum = ((uint16_t)recieved_buffer[8] << 8) + ((uint16_t)recieved_buffer[9] & 0xFF); //(1~12)
                TRACE_D("GR_Identify : Page_id [4]: %d", Page_num);
                TRACE_D("GR_Identify : Match_score [6]: %d", Match_score);
                TRACE_D("GR_Identify =>Checksum [8]: %#x", Checksum);
            }
            // TRACE_W("---------------------------------------------------");
        }
        // TRACE_B("                          -------- XXXX --------");
        free(txPacket);
    }
    *PageID_ptr = Page_num;
    *MatchScore_ptr = Match_score;
    //----------------------------------------------------------------------------------------------------------------------
    return (bool)F_res;
}

/**
 * @brief #### This function mannually detecting finger and store the detected finger image in ImageBuffer.
 *
 * @param (int)uart_channel_num: The UART channel number
 * @param (uint8_t*)recieved_buffer: Holds the address to a uart_buffer with recieved and filtered message
 * @param (uint32_t)timeout: Timeout(N*1ms) for uart message polling
 *
 * @return succcess[>0] or failure[0]
 */
bool GenImg(int uart_channel_num, uint8_t *recieved_buffer, uint32_t timeout)
{
    // Reset the recieving buffer before new data is to be extracted.
    memset(recieved_buffer, 0, MAX_PACKET_LENGTH_VAL);
    // create the packet here
    fingerprint_status_t F_res = FINGERPRINT_FAIL;

    fingerprint_packet_t *txPacket = (fingerprint_packet_t *)malloc(sizeof(fingerprint_packet_t));
    if (txPacket)
    {
        txPacket->header_code[0] = FINGERPRINT_HEADER_MSB;
        txPacket->header_code[1] = FINGERPRINT_HEADER_LSB;
        txPacket->device_address[0] = txPacket->device_address[1] = txPacket->device_address[2] = txPacket->device_address[3] = FINGERPRINT_DEVICE_ADDR_BIT;

        //------------ Inst_code + Data_content  --------------------------------------------------------------------------------
        uint8_t Combined_data[1] = {
            FINGERPRINT_GETIMAGE, /*INS CODE [1Byte]*/
        };

        // TRACE_B("                          -------- GenImg --------");
        bool res = SEND_PACKET(uart_channel_num,                  /* UART CHANNEL NUMBER */
                               txPacket,                          /* Address of packet container */
                               FINGERPRINT_PID_COMMANDPACKET,     /* Packet Identifier CMD*/
                               (uint16_t)(sizeof(Combined_data)), /* length <= combined_data*/
                               Combined_data);                    /* Inst_code + Data_content*/
        //------------ Check of the appropriate responce  --------------------------------------------------------------------------------
        if (res)
        {
            TRACE_W("--------------- 'GenImg' Response ----------------");
            F_res = __Response_function(recieved_buffer, timeout);
            if (FINGERPRINT_OK == F_res)
            {
                uint16_t Checksum = ((uint16_t)recieved_buffer[4] << 8) + ((uint16_t)recieved_buffer[5] & 0xFF); //(1~12)
                TRACE_D("GenImg =>Checksum [4]: %#x", Checksum);
            }
            // TRACE_W("---------------------------------------------------");
        }
        // TRACE_B("                          -------- XXXX --------");
        free(txPacket);
    }
    //----------------------------------------------------------------------------------------------------------------------
    return (bool)F_res;
}

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
bool Img2Tz(int uart_channel_num, uint8_t CharBufferID, uint8_t *recieved_buffer, uint32_t timeout)
{
    // Reset the recieving buffer before new data is to be extracted.
    memset(recieved_buffer, 0, MAX_PACKET_LENGTH_VAL);
    // create the packet here
    fingerprint_status_t F_res = FINGERPRINT_FAIL;

    fingerprint_packet_t *txPacket = (fingerprint_packet_t *)malloc(sizeof(fingerprint_packet_t));
    if (txPacket)
    {
        txPacket->header_code[0] = FINGERPRINT_HEADER_MSB;
        txPacket->header_code[1] = FINGERPRINT_HEADER_LSB;
        txPacket->device_address[0] = txPacket->device_address[1] = txPacket->device_address[2] = txPacket->device_address[3] = FINGERPRINT_DEVICE_ADDR_BIT;

        //------------ Inst_code + Data_content  --------------------------------------------------------------------------------
        uint8_t Combined_data[2] = {
            FINGERPRINT_IMAGE2TZ, /*INS CODE [1Byte]*/
            CharBufferID,         /*BufferId = CharBuf1 or CharBuf2*/
        };

        // TRACE_B("                          -------- Img2Tz --------");
        bool res = SEND_PACKET(uart_channel_num,                  /* UART CHANNEL NUMBER */
                               txPacket,                          /* Address of packet container */
                               FINGERPRINT_PID_COMMANDPACKET,     /* Packet Identifier CMD*/
                               (uint16_t)(sizeof(Combined_data)), /* length <= combined_data*/
                               Combined_data);                    /* Inst_code + Data_content*/
        //------------ Check of the appropriate responce  --------------------------------------------------------------------------------
        if (res)
        {
            TRACE_W("--------------- 'Img2Tz' Response ----------------");
            F_res = __Response_function(recieved_buffer, timeout);
            if (FINGERPRINT_OK == F_res)
            {
                uint16_t Checksum = ((uint16_t)recieved_buffer[4] << 8) + ((uint16_t)recieved_buffer[5] & 0xFF); //(1~12)
                TRACE_D("Img2Tz =>Checksum [4]: %#x", Checksum);
            }
            // TRACE_W("---------------------------------------------------");
        }
        // TRACE_B("                          -------- XXXX --------");
        free(txPacket);
    }
    //----------------------------------------------------------------------------------------------------------------------
    return (bool)F_res;
}

/**
 * @brief #### This function mannually combine information of character files from CharBuffer1 and CharBuffer2 ; Then generate a template which is stored back in both CharBuffer1 and CharBuffer2.
 *
 * @param (int)uart_channel_num: The UART channel number
 * @param (uint8_t*)recieved_buffer: Holds the address to a uart_buffer with recieved and filtered message
 * @param (uint32_t)timeout: Timeout(N*1ms) for uart message polling
 *
 * @return succcess[>0] or failure[0]
 */
bool RegModel(int uart_channel_num, uint8_t *recieved_buffer, uint32_t timeout)
{
    // Reset the recieving buffer before new data is to be extracted.
    memset(recieved_buffer, 0, MAX_PACKET_LENGTH_VAL);
    // create the packet here
    fingerprint_status_t F_res = FINGERPRINT_FAIL;

    fingerprint_packet_t *txPacket = (fingerprint_packet_t *)malloc(sizeof(fingerprint_packet_t));
    if (txPacket)
    {
        txPacket->header_code[0] = FINGERPRINT_HEADER_MSB;
        txPacket->header_code[1] = FINGERPRINT_HEADER_LSB;
        txPacket->device_address[0] = txPacket->device_address[1] = txPacket->device_address[2] = txPacket->device_address[3] = FINGERPRINT_DEVICE_ADDR_BIT;

        //------------ Inst_code + Data_content  --------------------------------------------------------------------------------
        uint8_t Combined_data[1] = {
            FINGERPRINT_REGMODEL, /*INS CODE [1Byte]*/
        };

        // TRACE_B("                          -------- RegModel -------");
        bool res = SEND_PACKET(uart_channel_num,                  /* UART CHANNEL NUMBER */
                               txPacket,                          /* Address of packet container */
                               FINGERPRINT_PID_COMMANDPACKET,     /* Packet Identifier CMD*/
                               (uint16_t)(sizeof(Combined_data)), /* length <= combined_data*/
                               Combined_data);                    /* Inst_code + Data_content*/
        //------------ Check of the appropriate responce  --------------------------------------------------------------------------------
        if (res)
        {
            TRACE_W("--------------- 'RegModel' Response ----------------");
            F_res = __Response_function(recieved_buffer, timeout);
            if (FINGERPRINT_OK == F_res)
            {
                uint16_t Checksum = ((uint16_t)recieved_buffer[4] << 8) + ((uint16_t)recieved_buffer[5] & 0xFF); //(1~12)
                TRACE_D("RegModel =>Checksum [4]: %#x", Checksum);
            }
            // TRACE_W("---------------------------------------------------");
        }
        // TRACE_B("                          -------- XXXX --------");
        free(txPacket);
    }
    //----------------------------------------------------------------------------------------------------------------------
    return (bool)F_res;
}

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
bool Store(int uart_channel_num, uint8_t CharBufferID, uint16_t PageID, uint8_t *recieved_buffer, uint32_t timeout)
{
    // Reset the recieving buffer before new data is to be extracted.
    memset(recieved_buffer, 0, MAX_PACKET_LENGTH_VAL);
    // create the packet here
    fingerprint_status_t F_res = FINGERPRINT_FAIL;

    fingerprint_packet_t *txPacket = (fingerprint_packet_t *)malloc(sizeof(fingerprint_packet_t));
    if (txPacket)
    {
        txPacket->header_code[0] = FINGERPRINT_HEADER_MSB;
        txPacket->header_code[1] = FINGERPRINT_HEADER_LSB;
        txPacket->device_address[0] = txPacket->device_address[1] = txPacket->device_address[2] = txPacket->device_address[3] = FINGERPRINT_DEVICE_ADDR_BIT;

        //------------ Inst_code + Data_content  --------------------------------------------------------------------------------
        uint8_t Combined_data[4] = {
            FINGERPRINT_STORE,        /*INS CODE [1Byte]*/
            CharBufferID,             /*BufferId = CharBuf1 or CharBuf2*/
            (uint8_t)(PageID >> 8),   /*MSB sent first*/
            (uint8_t)(PageID & 0xFF), /*LSB sent last*/
        };

        // TRACE_B("                          -------- Store --------");
        bool res = SEND_PACKET(uart_channel_num,                  /* UART CHANNEL NUMBER */
                               txPacket,                          /* Address of packet container */
                               FINGERPRINT_PID_COMMANDPACKET,     /* Packet Identifier CMD*/
                               (uint16_t)(sizeof(Combined_data)), /* length <= combined_data*/
                               Combined_data);                    /* Inst_code + Data_content*/
        //------------ Check of the appropriate responce  --------------------------------------------------------------------------------
        if (res)
        {
            TRACE_W("--------------- 'Store' Response ----------------");
            F_res = __Response_function(recieved_buffer, timeout);
            if (FINGERPRINT_OK == F_res)
            {
                uint16_t Checksum = ((uint16_t)recieved_buffer[4] << 8) + ((uint16_t)recieved_buffer[5] & 0xFF); //(1~12)
                TRACE_D("Store =>Checksum [4]: %#x", Checksum);
            }
            // TRACE_W("---------------------------------------------------");
        }
        // TRACE_B("                          -------- XXXX --------");
        free(txPacket);
    }
    //----------------------------------------------------------------------------------------------------------------------
    return (bool)F_res;
}

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
bool Load(int uart_channel_num, uint8_t CharBufferID, uint16_t PageID, uint8_t *recieved_buffer, uint32_t timeout)
{
    // Reset the recieving buffer before new data is to be extracted.
    memset(recieved_buffer, 0, MAX_PACKET_LENGTH_VAL);
    // create the packet here
    fingerprint_status_t F_res = FINGERPRINT_FAIL;

    fingerprint_packet_t *txPacket = (fingerprint_packet_t *)malloc(sizeof(fingerprint_packet_t));
    if (txPacket)
    {
        txPacket->header_code[0] = FINGERPRINT_HEADER_MSB;
        txPacket->header_code[1] = FINGERPRINT_HEADER_LSB;
        txPacket->device_address[0] = txPacket->device_address[1] = txPacket->device_address[2] = txPacket->device_address[3] = FINGERPRINT_DEVICE_ADDR_BIT;

        //------------ Inst_code + Data_content  --------------------------------------------------------------------------------
        uint8_t Combined_data[4] = {
            FINGERPRINT_LOAD,         /*INS CODE [1Byte]*/
            CharBufferID,             /*BufferId = CharBuf1 or CharBuf2*/
            (uint8_t)(PageID >> 8),   /*MSB sent first*/
            (uint8_t)(PageID & 0xFF), /*LSB sent last*/
        };

        // TRACE_B("                          -------- Load -------");
        bool res = SEND_PACKET(uart_channel_num,                  /* UART CHANNEL NUMBER */
                               txPacket,                          /* Address of packet container */
                               FINGERPRINT_PID_COMMANDPACKET,     /* Packet Identifier CMD*/
                               (uint16_t)(sizeof(Combined_data)), /* length <= combined_data*/
                               Combined_data);                    /* Inst_code + Data_content*/
        //------------ Check of the appropriate responce  --------------------------------------------------------------------------------
        if (res)
        {
            // TRACE_W("--------------- 'Load' Response ----------------");
            F_res = __Response_function(recieved_buffer, timeout);
            if (FINGERPRINT_OK == F_res)
            {
                uint16_t Checksum = ((uint16_t)recieved_buffer[4] << 8) + ((uint16_t)recieved_buffer[5] & 0xFF); //(1~12)
                // TRACE_D("Load =>Checksum [4]: %#x", Checksum);
            }
            // TRACE_W("---------------------------------------------------");
        }
        // TRACE_B("                          -------- XXXX --------");
        free(txPacket);
    }
    //----------------------------------------------------------------------------------------------------------------------
    return (bool)F_res;
}

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
bool Delete(int uart_channel_num, uint16_t PageID, uint16_t TempCount, uint8_t *recieved_buffer, uint32_t timeout)
{
    // Reset the recieving buffer before new data is to be extracted.
    memset(recieved_buffer, 0, MAX_PACKET_LENGTH_VAL);
    // create the packet here
    fingerprint_status_t F_res = FINGERPRINT_FAIL;

    fingerprint_packet_t *txPacket = (fingerprint_packet_t *)malloc(sizeof(fingerprint_packet_t));
    if (txPacket)
    {
        txPacket->header_code[0] = FINGERPRINT_HEADER_MSB;
        txPacket->header_code[1] = FINGERPRINT_HEADER_LSB;
        txPacket->device_address[0] = txPacket->device_address[1] = txPacket->device_address[2] = txPacket->device_address[3] = FINGERPRINT_DEVICE_ADDR_BIT;

        //------------ Inst_code + Data_content  --------------------------------------------------------------------------------
        uint8_t Combined_data[5] = {
            FINGERPRINT_DELETE,          /*INS CODE [1Byte]*/
            (uint8_t)(PageID >> 8),      /*MSB sent first*/
            (uint8_t)(PageID & 0xFF),    /*LSB sent last*/
            (uint8_t)(TempCount >> 8),   /*MSB sent first*/
            (uint8_t)(TempCount & 0xFF), /*LSB sent last*/
        };

        // TRACE_B("                          -------- Delete ---------");
        // -- -- -- -- -- --Fill the packet container and send it via uart-- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- --
        bool res = SEND_PACKET(uart_channel_num,                  /* UART CHANNEL NUMBER */
                               txPacket,                          /* Address of packet container */
                               FINGERPRINT_PID_COMMANDPACKET,     /* Packet Identifier CMD*/
                               (uint16_t)(sizeof(Combined_data)), /* length <= combined_data*/
                               Combined_data);                    /* Inst_code + Data_content*/
        //------------ Check of the appropriate responce  --------------------------------------------------------------------------------
        if (res)
        {
            TRACE_W("--------------- 'Delete' Response ----------------");
            F_res = __Response_function(recieved_buffer, timeout);
            if (FINGERPRINT_OK == F_res)
            {
                uint16_t Checksum = ((uint16_t)recieved_buffer[4] << 8) + ((uint16_t)recieved_buffer[5] & 0xFF); //(1~12)
                TRACE_D("Delete =>Checksum [4]: %#x", Checksum);
            }
            // TRACE_W("---------------------------------------------------");
        }
        // TRACE_B("                          -------- XXXX --------");
        free(txPacket);
    }
    //----------------------------------------------------------------------------------------------------------------------
    return (bool)F_res;
}

/**
 * @brief #### This function delete all the templates in the Flash library.
 *
 * @param (int)uart_channel_num: The UART channel number
 * @param (uint8_t*)recieved_buffer: Holds the address to a uart_buffer with recieved and filtered message
 * @param (uint32_t)timeout: Timeout(N*1ms) for uart message polling
 *
 * @return succcess[>0] or failure[0]
 */
bool Empty(int uart_channel_num, uint8_t *recieved_buffer, uint32_t timeout)
{
    // Reset the recieving buffer before new data is to be extracted.
    memset(recieved_buffer, 0, MAX_PACKET_LENGTH_VAL);
    // create the packet here
    fingerprint_status_t F_res = FINGERPRINT_FAIL;

    fingerprint_packet_t *txPacket = (fingerprint_packet_t *)malloc(sizeof(fingerprint_packet_t));
    if (txPacket)
    {
        txPacket->header_code[0] = FINGERPRINT_HEADER_MSB;
        txPacket->header_code[1] = FINGERPRINT_HEADER_LSB;
        txPacket->device_address[0] = txPacket->device_address[1] = txPacket->device_address[2] = txPacket->device_address[3] = FINGERPRINT_DEVICE_ADDR_BIT;

        //------------ Inst_code + Data_content  --------------------------------------------------------------------------------
        uint8_t Combined_data[1] = {
            FINGERPRINT_EMPTY, /*INS CODE [1Byte]*/
        };

        //         TRACE_B("                          -------- Empty ---------");
        bool res = SEND_PACKET(uart_channel_num,                  /* UART CHANNEL NUMBER */
                               txPacket,                          /* Address of packet container */
                               FINGERPRINT_PID_COMMANDPACKET,     /* Packet Identifier CMD*/
                               (uint16_t)(sizeof(Combined_data)), /* length <= combined_data*/
                               Combined_data);                    /* Inst_code + Data_content*/
        //------------ Check of the appropriate responce  --------------------------------------------------------------------------------
        if (res)
        {
            TRACE_W("--------------- 'Empty' Response ----------------");
            F_res = __Response_function(recieved_buffer, timeout);
            if (FINGERPRINT_OK == F_res)
            {
                uint16_t Checksum = ((uint16_t)recieved_buffer[4] << 8) + ((uint16_t)recieved_buffer[5] & 0xFF); //(1~12)
                TRACE_D("Empty =>Checksum [4]: %#x", Checksum);
            }
            // TRACE_W("---------------------------------------------------");
        }
        //         TRACE_B("                          -------- XXXX --------");
        free(txPacket);
    }
    //----------------------------------------------------------------------------------------------------------------------
    return (bool)F_res;
}
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
bool LedControl(int uart_channel_num, bool LED_state, uint8_t *recieved_buffer, uint32_t timeout)
{
    // Reset the recieving buffer before new data is to be extracted.
    memset(recieved_buffer, 0, MAX_PACKET_LENGTH_VAL);
    // create the packet here
    fingerprint_status_t F_res = FINGERPRINT_FAIL;

    fingerprint_packet_t *txPacket = (fingerprint_packet_t *)malloc(sizeof(fingerprint_packet_t));
    if (txPacket)
    {
        txPacket->header_code[0] = FINGERPRINT_HEADER_MSB;
        txPacket->header_code[1] = FINGERPRINT_HEADER_LSB;
        txPacket->device_address[0] = txPacket->device_address[1] = txPacket->device_address[2] = txPacket->device_address[3] = FINGERPRINT_DEVICE_ADDR_BIT;

        //------------ Inst_code + Data_content  --------------------------------------------------------------------------------
        uint8_t Combined_data[5] = {
            FINGERPRINT_AURALEDCONFIG,
            FINGERPRINT_LED_BREATHING + ((LED_state) ? 0 : 3), /*control*/
            100,                                               /*speed*/
            FINGERPRINT_LED_BLUE,                              /*coloridx*/
            0,                                                 /*count*/
        };

        // TRACE_B("                          -------- LedControl ---------");
        bool res = SEND_PACKET(uart_channel_num,                  /* UART CHANNEL NUMBER */
                               txPacket,                          /* Address of packet container */
                               FINGERPRINT_PID_COMMANDPACKET,     /* Packet Identifier CMD*/
                               (uint16_t)(sizeof(Combined_data)), /* length <= combined_data*/
                               Combined_data);                    /* Inst_code + Data_content*/
        //------------ Check of the appropriate responce  --------------------------------------------------------------------------------
        if (res)
        {
            // TRACE_W("--------------- 'LedControl' Response ----------------");
            F_res = __Response_function(recieved_buffer, timeout);
            if (FINGERPRINT_OK == F_res)
            {
                uint16_t Checksum = ((uint16_t)recieved_buffer[4] << 8) + ((uint16_t)recieved_buffer[5] & 0xFF); //(1~12)
                // TRACE_D("LedControl =>Checksum [4]: %#x", Checksum);
            }
            // TRACE_W("---------------------------------------------------");
        }
        // TRACE_B("                          -------- XXXX --------");
        free(txPacket);
    }
    //----------------------------------------------------------------------------------------------------------------------
    return (bool)F_res;
}

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
bool Match(int uart_channel_num, uint16_t *InspectionScore, uint8_t *recieved_buffer, uint32_t timeout)
{
    uint16_t Score = 0;
    // Reset the recieving buffer before new data is to be extracted.
    memset(recieved_buffer, 0, MAX_PACKET_LENGTH_VAL);
    // create the packet here
    fingerprint_status_t F_res = FINGERPRINT_FAIL;

    fingerprint_packet_t *txPacket = (fingerprint_packet_t *)malloc(sizeof(fingerprint_packet_t));
    if (txPacket)
    {
        txPacket->header_code[0] = FINGERPRINT_HEADER_MSB;
        txPacket->header_code[1] = FINGERPRINT_HEADER_LSB;
        txPacket->device_address[0] = txPacket->device_address[1] = txPacket->device_address[2] = txPacket->device_address[3] = FINGERPRINT_DEVICE_ADDR_BIT;

        //------------ Inst_code + Data_content  --------------------------------------------------------------------------------
        uint8_t Combined_data[1] = {
            FINGERPRINT_MATCH, /*INS CODE [1Byte]*/
        };

        // TRACE_B("                          -------- Match --------");
        // -- -- -- -- -- --Fill the packet container and send it via uart-- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- --
        bool res = SEND_PACKET(uart_channel_num,                  /* UART CHANNEL NUMBER */
                               txPacket,                          /* Address of packet container */
                               FINGERPRINT_PID_COMMANDPACKET,     /* Packet Identifier CMD*/
                               (uint16_t)(sizeof(Combined_data)), /* length <= combined_data*/
                               Combined_data);                    /* Inst_code + Data_content*/
        //------------ Check of the appropriate responce  --------------------------------------------------------------------------------
        if (res)
        {
            TRACE_W("--------------- 'Match' Response ----------------");
            F_res = __Response_function(recieved_buffer, timeout);
            if (FINGERPRINT_OK == F_res)
            {
                Score = ((uint16_t)recieved_buffer[4] << 8) + ((uint16_t)recieved_buffer[5] & 0xFF);
                uint16_t Checksum = ((uint16_t)recieved_buffer[6] << 8) + ((uint16_t)recieved_buffer[7] & 0xFF); //(1~12)
                TRACE_D("Match : Inspection_Score [4]: %#x", Score);
                TRACE_D("Match =>Checksum [6]: %#x", Checksum);
            }
            // TRACE_W("---------------------------------------------------");
        }
        // TRACE_B("                          -------- XXXX --------");
        free(txPacket);
    }
    *InspectionScore = Score;
    //----------------------------------------------------------------------------------------------------------------------
    return (bool)F_res;
}

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
bool Search(int uart_channel_num, uint8_t CharBufferID, uint16_t StartPage, uint16_t PageNum, uint16_t *PageID_ptr, uint16_t *MatchScore_ptr, uint8_t *recieved_buffer, uint32_t timeout)
{
    // use dummy variables
    uint16_t Page_num = *PageID_ptr;
    uint16_t Match_score = *MatchScore_ptr;
    // Reset the recieving buffer before new data is to be extracted.
    memset(recieved_buffer, 0, MAX_PACKET_LENGTH_VAL);
    // create the packet here
    fingerprint_status_t F_res = FINGERPRINT_FAIL;

    fingerprint_packet_t *txPacket = (fingerprint_packet_t *)malloc(sizeof(fingerprint_packet_t));
    if (txPacket)
    {
        txPacket->header_code[0] = FINGERPRINT_HEADER_MSB;
        txPacket->header_code[1] = FINGERPRINT_HEADER_LSB;
        txPacket->device_address[0] = txPacket->device_address[1] = txPacket->device_address[2] = txPacket->device_address[3] = FINGERPRINT_DEVICE_ADDR_BIT;

        //------------ Inst_code + Data_content  --------------------------------------------------------------------------------
        uint8_t Combined_data[6] = {
            FINGERPRINT_SEARCH,          /*INS CODE [1Byte]*/
            CharBufferID,                /*BufferId = CharBuf1 or CharBuf2*/
            (uint8_t)(StartPage >> 8),   /*MSB sent first*/
            (uint8_t)(StartPage & 0xFF), /*LSB sent last*/
            (uint8_t)(PageNum >> 8),     /*MSB sent first*/
            (uint8_t)(PageNum & 0xFF),   /*LSB sent last*/
        };

        // TRACE_B("                          --------- Search ---------");
        bool res = SEND_PACKET(uart_channel_num,                  /* UART CHANNEL NUMBER */
                               txPacket,                          /* Address of packet container */
                               FINGERPRINT_PID_COMMANDPACKET,     /* Packet Identifier CMD*/
                               (uint16_t)(sizeof(Combined_data)), /* length <= combined_data*/
                               Combined_data);                    /* Inst_code + Data_content*/
        //------------ Check of the appropriate responce  --------------------------------------------------------------------------------
        if (res)
        {
            TRACE_W(" >> 'Search' Response <<");
            F_res = __Response_function(recieved_buffer, timeout);
            if (FINGERPRINT_OK == F_res)
            {
                Page_num = ((uint16_t)recieved_buffer[4] << 8) + ((uint16_t)recieved_buffer[5] & 0xFF);
                Match_score = ((uint16_t)recieved_buffer[6] << 8) + ((uint16_t)recieved_buffer[7] & 0xFF);       // 0x0009
                uint16_t Checksum = ((uint16_t)recieved_buffer[8] << 8) + ((uint16_t)recieved_buffer[9] & 0xFF); //(1~12)
                TRACE_D("Search : Page_id [4]: %#x", Page_num);
                TRACE_D("Search : Match_score [6]: %#x", Match_score);
                TRACE_D("Search =>Checksum [8]: %#x", Checksum);
            }
        }
        // only if the search operation is successful, the value at 'PageID_ptr'&'MatchScore_ptr' are to be replaced
        if ((0 != Page_num) && (*PageID_ptr != Page_num) && (0 < Match_score))
        {
            /*Entering here means, duplicate is found*/
            *PageID_ptr = Page_num;
            *MatchScore_ptr = Match_score;
            // TRACE_W("---------------------------------------------------");
        }
        // TRACE_B("                          -------- XXXX --------");
        free(txPacket);
    }
    //----------------------------------------------------------------------------------------------------------------------
    return (bool)F_res;
}

//-----------------------------------------------------------------------------------------------------------------------------------------------
// Function for Operation modes
//-----------------------------------------------------------------------------------------------------------------------------------------------

//----------------------------------------------------------------------------------------------------------------------
/**
 * @brief #### This function checks if system is in free.
 *
 * @param (int)uart_channel_num: The uart channel number
 * @param (uint32_t)timeout_ms: Timeout(N*1ms) for uart message polling
 *
 * @return [succcess='true'] & [failure='false']
 */
bool Wait_till_system_free(l_ezlopi_item_t *item, uint32_t timeout_ms) // wait_
{
    fingerprint_status_t F_res = FINGERPRINT_FAIL;
    if (NULL != item)
    {
        server_packet_t *user_data = (server_packet_t *)item->user_arg;
        static uint16_t __FPreg_status;
        //<! us time
        // TRACE_B("  > ENTER - system_status");
        uint32_t start_time = esp_timer_get_time();
        while (((esp_timer_get_time() - start_time) / 1000) <= timeout_ms) // ms
        {
            // TRACE_D("Checking:.....");
            F_res = ReadSysPara(item->interface.uart.channel, &__FPreg_status, (user_data->recieved_buffer), 250); // 200ms polling
            if ((FINGERPRINT_OK == F_res) && (__FPreg_status == (SYSTEM_FREE)))                                    //(SYSTEM_HNDSHK_VERIFIED | SYSTEM_FREE)
            {
                TRACE_D(".................... System -> FREE , status_reg [%#x]", __FPreg_status);
                break;
            }
        }
        // TRACE_B("   < EXIT - system_status");
    }
    return (bool)F_res;
}

//----------------------------- Function to check if Specified ID is occupied or not -----------------------------------------------
/**
 * @brief #### This function checks the perticular USER/PAGE_ID .
 * @return {true ==> empty} / {false == not empty}
 */
bool Check_PAGEID_Empty(l_ezlopi_item_t *item)
{
    bool ret = false;
    if (NULL != item)
    {
        // TRACE_D("                  <<< Check_PAGEID_Empty >>> ");
        server_packet_t *user_data = (server_packet_t *)item->user_arg;
        fingerprint_status_t p = FINGERPRINT_FAIL; // status checker
        uint32_t start_time = 0, dummy_timer = 0;
        start_time = esp_timer_get_time() / 1000;  //  !< ms
        dummy_timer = esp_timer_get_time() / 1000; //  !< ms
        while ((p != FINGERPRINT_OK) && ((dummy_timer - start_time) < 600))
        {
            p = Load(item->interface.uart.channel, 1, (user_data->user_id), (user_data->recieved_buffer), 300);
            dummy_timer = esp_timer_get_time() / 1000;
        }
        if ((p == FINGERPRINT_OK))
        {
            TRACE_W(" Valid USER_ID present in ID->[%d]", user_data->user_id);
            ret = false;
        }
        else
        {
            TRACE_W(" USER_ID [%d] is empty", user_data->user_id);
            ret = true;
        }
        // TRACE_D("                       <<< XXXX >>>");
    }
    // Result: OK - Internal Empty!
    return (ret);
}

//---------------------------------- Function returns immediate vaccant ID -----------------------------------------
/**
 * @brief #### This function finds immediate vaccant ID.
 *
 * @return {0 => vaccant IDs not found}
 */
uint16_t Find_immediate_vaccant_ID(l_ezlopi_item_t *item)
{
    if (NULL != item)
    {
        server_packet_t *user_data = (server_packet_t *)item->user_arg;
        for (uint16_t ids = 1; ids <= FINGERPRINT_MAX_CAPACITY_LIMIT; ids++)
        {
            // First update the ID occupancy status in item->user_arg
            user_data->user_id = ids; // place the new ID to check
            if (Check_PAGEID_Empty(item))
            {                 // when  empty
                return (ids); // return index
            }
            vTaskDelay(50 / portTICK_PERIOD_MS);
        }
    }
    return 0; // if not found
}

//---------------------------------- Function that updates validity status of internal PAGEID to append the new -----------------------------------------
/**
 * @brief #### This function Scans and update validity status of [1~500(max defined)] PAGE_IDs.
 */
bool Update_ID_status_list(l_ezlopi_item_t *item)
{
    bool ret = false;
    if (NULL != item)
    {
        // TRACE_D("---------------------- ENTER: .[MODE:-2]. Update_ID_list ------------------------");
        server_packet_t *user_data = (server_packet_t *)item->user_arg;
        uint16_t Temp_ID = user_data->user_id;
        for (uint16_t ids = 1; ids <= FINGERPRINT_MAX_CAPACITY_LIMIT; ids++)
        {
            // First update the ID occupancy status in item->user_arg
            user_data->user_id = ids;
            if (Check_PAGEID_Empty(item))
            {
                user_data->validity[ids] = false; // when empty
            }
            else
            {
                user_data->validity[ids] = true; // when occupied
            }
            vTaskDelay(5 / portTICK_PERIOD_MS);
        }
        user_data->user_id = Temp_ID;
        ret = true;
        // TRACE_D("--------------------------- EXIT: .[MODE:-2]. ----------------------------");
    }
    return ret;
}
//---------------------------------- Function that searches, internal library to return PAGE_ID and confidence_level -----------------------------------------
/**
 * @brief #### This function extracts fingerprint after interrupt signal and then
 */
bool Match_ID(l_ezlopi_item_t *item)
{
    bool ret = true;
    if (NULL != item)
    {
        TRACE_D("---------------------- ENTER: .[MODE:-0]. Match_ID ------------------------");
        uint32_t start_time = 0, dummy_timer = 0;
        server_packet_t *user_data = (server_packet_t *)item->user_arg;
        uint16_t custom_USER_ID = user_data->user_id;
        //-------------------- 4. Search for all the library and store only if no duplicates found -----------------------
        fingerprint_status_t p = FINGERPRINT_FAIL;
        int uart_channel_num = item->interface.uart.channel;
        start_time = esp_timer_get_time() / 1000;  //  !< ms
        dummy_timer = esp_timer_get_time() / 1000; //  !< ms
        while ((p != FINGERPRINT_OK) && ((dummy_timer - start_time) < 2000))
        {
            LedControl(uart_channel_num, 1, (user_data->recieved_buffer), 200);
            p = GR_Identify(item->interface.uart.channel,   /*user_channel*/
                            &(user_data->user_id),          /*same_ID => */
                            &(user_data->confidence_level), /*[value=!0]=> */
                            (user_data->recieved_buffer),   /*Uart_buffer address*/
                            1000);
            LedControl(uart_channel_num, 0, (user_data->recieved_buffer), 200);
            dummy_timer = esp_timer_get_time() / 1000;
            if ((p != FINGERPRINT_OK) && (dummy_timer - start_time) >= 2000)
            {
                TRACE_E(" Failed Match Operation ........... The Fingerpint, Doesnot match with : USER_ID[#%d]", custom_USER_ID);
                user_data->user_id = custom_USER_ID;
                ret = false;
                break;
            }
        }
        TRACE_D("--------------------------- EXIT: .[MODE:-0]. ----------------------------");
    }
    return ret;
}

//---------------------------------- Function that searches and erases specified range of IDs -----------------------------------------
/**
 * @brief #### This Function searches and erases specified range of IDs only.
 */
bool Erase_Specified_ID(l_ezlopi_item_t *item)
{
    bool ret = true;
    if (NULL != item)
    {
        TRACE_D("----------------------  ENTER: .[MODE:-3]. Erase_Specified_ID ------------------------");
        uint32_t start_time = 0, dummy_timer = 0;
        server_packet_t *user_data = (server_packet_t *)item->user_arg;
        //-------------------- 4. Search for all the library and store only if no duplicates found ------------------------------------------------
        fingerprint_status_t p = FINGERPRINT_FAIL;
        start_time = esp_timer_get_time() / 1000; //  !< ms
        dummy_timer = esp_timer_get_time() / 1000;
        while ((p != FINGERPRINT_OK) && ((dummy_timer - start_time) < 2000))
        {
            p = Delete(item->interface.uart.channel, /*user_channel*/
                       (user_data->user_id),         /*Starting_point*/
                       1,                            //    (user_data->id_counts),       /*Quantity*/
                       (user_data->recieved_buffer), /*Uart_buffer address*/
                       1000);
            dummy_timer = esp_timer_get_time() / 1000;
            if ((p != FINGERPRINT_OK) && (dummy_timer - start_time) >= 2000)
            {
                TRACE_W(" Failed to Delete (Specified ID-Range) from internal Library . Try again ...........");
                ret = false;
                break;
            }
        }
        TRACE_D("DELETED from id:[#%d]", (user_data->user_id));
        TRACE_D("--------------------------- EXIT: .[MODE:-3]. ----------------------------");
    }
    return ret;
}

//---------------------------------- Function erases all fingerprints from internal library  -----------------------------------------
/**
 * @brief #### This function erases fingerprint from internal libraray
 *
 */
bool Erase_all_ID(l_ezlopi_item_t *item)
{
    bool ret = true;
    if (NULL != item)
    {
        TRACE_D("----------------------  ENTER: .[MODE:-4]. Erase_all_ID ------------------------");
        uint32_t start_time = 0, dummy_timer = 0;
        server_packet_t *user_data = (server_packet_t *)item->user_arg;
        //-------------------- 4. Search for all the library and store only if no duplicates found ------------------------------------------------
        fingerprint_status_t p = FINGERPRINT_FAIL;
        start_time = esp_timer_get_time() / 1000; //  !< ms
        dummy_timer = esp_timer_get_time() / 1000;
        while ((p != FINGERPRINT_OK) && ((dummy_timer - start_time) < 1000))
        {
            p = Empty(item->interface.uart.channel, /*user_channel*/
                      (user_data->recieved_buffer), /*Uart_buffer address*/
                      800);
            dummy_timer = esp_timer_get_time() / 1000;
            if ((p != FINGERPRINT_OK) && (dummy_timer - start_time) >= 1000)
            {
                TRACE_W(" Failed to Erase all the IDs from internal fingerprint library  . Try again ...........");
                ret = false;
                break;
            }
        }
        TRACE_D("--------------------------- EXIT: .[MODE:-4]. ----------------------------");
    }
    return ret;
}

//----------------------------------- Function to store only valid fingerprint, in vacant PAGEID -----------------------------------------
/**
 * @brief This function store only valid fingerprint, in vacant PAGEID
 * @paragraph IF match_% < 20 ; storing takes place
 * @paragraph IF match_% > 20 ; Duplicate_ID is returened *
 * @return {0} => Unsucessful_cmds ; {same_id} => Successfully_stored ; {different_id} => Duplicate_ID
 */
uint16_t Enroll_Fingerprint(l_ezlopi_item_t *item)
{
    uint16_t res_ID = 0; /* initially, set variable value to a invalid id*/
    if (NULL != item)
    {
        TRACE_D("----------------------  ENTER: .[MODE:-1]. Enroll_Fingerprint ------------------------");
        int uart_channel_num = item->interface.uart.channel;
        server_packet_t *user_data = (server_packet_t *)item->user_arg;
        uint16_t custom_USER_ID = user_data->user_id;
        uint32_t start_time = 0, dummy_timer = 0;

        //----------------------- 1. Collecting First fingerprint -----------------------------------------------
        fingerprint_status_t p = FINGERPRINT_FAIL; // status checker
        TRACE_I(" [Phase:-1]...ENROLL..Place a finger ......");

        start_time = esp_timer_get_time() / 1000; //  !< ms
        dummy_timer = esp_timer_get_time() / 1000;
        while ((p != FINGERPRINT_OK) && ((dummy_timer - start_time) < 2000))
        {
            LedControl(uart_channel_num, 1, (user_data->recieved_buffer), 200);
            TRACE_D("GenImg generation [1]");
            p = GenImg(uart_channel_num, (user_data->recieved_buffer), 1000);
            LedControl(uart_channel_num, 0, (user_data->recieved_buffer), 200);
            dummy_timer = esp_timer_get_time() / 1000;
            if ((p != FINGERPRINT_OK) && (dummy_timer - start_time) > 2000)
            {
                TRACE_D("Image generation [1].... failed .. Retry by placing the finger again... after 3 seconds");
            }
        }
        vTaskDelay(100 / portTICK_PERIOD_MS);

        p = Img2Tz(uart_channel_num, 1, (user_data->recieved_buffer), 1000);
        if (p)
        {
            TRACE_I(".........DON'T Remove finger.... wait 1 seconds");
            vTaskDelay(500 / portTICK_PERIOD_MS);
        }
        else
        {
            TRACE_W("Character Image generation [1] failed .. try again... after 3 seconds");
            return (0);
        }
        // 1. OK success!
        vTaskDelay(500 / portTICK_PERIOD_MS);

        //----------------------- 2. Collecting Second fingerprint -----------------------------------------------
        p = FINGERPRINT_FAIL;
        TRACE_I("[Phase:-2] ...ENROLL ... Place same finger again......");
        start_time = esp_timer_get_time() / 1000; //  !< ms
        dummy_timer = esp_timer_get_time() / 1000;
        while ((p != FINGERPRINT_OK) && ((dummy_timer - start_time) < 2000))
        {
            LedControl(uart_channel_num, 1, (user_data->recieved_buffer), 200);
            TRACE_D("GenImg generation [2]");
            p = GenImg(uart_channel_num, (user_data->recieved_buffer), 1000);
            LedControl(uart_channel_num, 0, (user_data->recieved_buffer), 200);
            dummy_timer = esp_timer_get_time() / 1000;
            if ((p != FINGERPRINT_OK) && (dummy_timer - start_time) > 2000)
            {
                TRACE_D("Image generation [2].... failed .. Retry by placing the finger again... after 3 seconds");
            }
        }
        vTaskDelay(100 / portTICK_PERIOD_MS);

        // generate character file from the original finger image in ImageBuffer and store the file in CharBuffer2
        p = Img2Tz(uart_channel_num, 2, (user_data->recieved_buffer), 1000);
        if (p)
        {
            TRACE_I(".........DON'T Remove finger.... wait 1 seconds");
            vTaskDelay(500 / portTICK_PERIOD_MS);
        }
        else
        {
            TRACE_W("Character Image generation [2] failed .. try again... after 3 seconds");
            return (0);
        }
        vTaskDelay(500 / portTICK_PERIOD_MS);
        // 2. OK success!

        //--------------------- 3. Generate the template after combining ChBuffer-1&2 , and store the result in both ChBuffer-1&2 ------------------
        TRACE_I("Creating model for user_ID: #%d..", custom_USER_ID);
        p = FINGERPRINT_FAIL;
        TRACE_I("[Phase:-3] ... Create Model form ChrBuffer-1&2 .........");
        start_time = esp_timer_get_time() / 1000; //  !< ms
        dummy_timer = esp_timer_get_time() / 1000;
        while ((p != FINGERPRINT_OK) && ((dummy_timer - start_time) < 2000))
        {
            p = RegModel(uart_channel_num, (user_data->recieved_buffer), 1000);
            dummy_timer = esp_timer_get_time() / 1000;
            if ((p != FINGERPRINT_OK) && (dummy_timer - start_time) > 2000)
            {
                TRACE_W(" Template Generation failed .......... Retry Again... after 3 seconds");
                vTaskDelay(1000 / portTICK_PERIOD_MS);
                return (0);
            }
        }
        vTaskDelay(500 / portTICK_PERIOD_MS);
        // 3. OK converted!

        //-------------------- 4. Search for all the library and store only if no duplicates found ------------------------------------------------
        p = FINGERPRINT_FAIL;
        TRACE_D("[Phase:-4] ... Search Duplicates of user_ID: [#%d]; (i.e. inside fingerprint Lib)", custom_USER_ID);
        start_time = esp_timer_get_time() / 1000; //  !< ms
        dummy_timer = esp_timer_get_time() / 1000;
        while ((p != FINGERPRINT_OK) && ((dummy_timer - start_time) < 1100))
        {
            p = Search(uart_channel_num,
                       1,                                 /*ChrBuffer = 1*/
                       FINGERPRINT_STARTING_USER_PAGE_ID, /*Page_id = starting_id*/
                       FINGERPRINT_MAX_CAPACITY_LIMIT,    /*Max_search_number = 500*/
                       &(user_data->user_id),             /*same_ID => */
                       &(user_data->confidence_level),    /*[value=!0]=> */
                       (user_data->recieved_buffer),
                       1000);
            dummy_timer = esp_timer_get_time() / 1000;
            if ((p != FINGERPRINT_OK) && (dummy_timer - start_time) > 1100)
            {
                if (user_data->confidence_level == 0)
                {
                    TRACE_W(" Duplicate Template Not Found ...........continuing to Store-phase: ");
                    break;
                }
            }
            else if ((p == FINGERPRINT_OK) && (dummy_timer - start_time) <= 1000)
            {
                if ((user_data->confidence_level) > 20)
                {
                    TRACE_W(" Duplicate Template Found => @ user_id [%d]", user_data->user_id);
                    return (0);
                    // dont return here
                }
                else if ((user_data->confidence_level) != 0)
                {
                    TRACE_W(" Duplicate Template Found => @ user_id [%d] ; @percent = [%d]", (user_data->user_id), (user_data->confidence_level));
                    break;
                }
                else
                {
                    TRACE_W(" Duplicate Template check failed/is empty...........");
                    break;
                }
            }
        }
        vTaskDelay(500 / portTICK_PERIOD_MS);
        // 4. OK Searching Succesful!

        //-------------------- 5. Store the template from specified buffer at designated location (PageID) of flash library -------------------------
        // Only if duplcate user is not found proceed, (UserID = 0) then store the fingerprint
        /**
         * Here,
         *  1. custom_USER_ID ......................indicates => Requested_ID from UI.
         *  2. (user_data->user_id) ................indicates => prev_ID we got from internal.
         *  3. (user_data->confidence_level) .......indicates => Match_score we got from internal.
         *
         * */
        if ((20 < (user_data->confidence_level)))
        {
            TRACE_E("The Fingerprint is already present in:-[%d] ; Matched percent = [%d]", (user_data->user_id), (user_data->confidence_level));
            res_ID = (user_data->user_id); // returns
        }
        else /*confidence is less than threshold means its not found*/
        {
            TRACE_I("Storing desired, USER_ID: [%d] -> into PAGE_ID[#%d] ", custom_USER_ID, (user_data->user_id)); // we have made sure that:->  [custom_USER_ID != (user_data->user_id)]
            p = FINGERPRINT_FAIL;
            TRACE_I("[Phase:-5] ...Store the unique fingerprint [Phase:-5]... Place same finger again......");
            // Loop until Second fingerprint collection is successful
            start_time = esp_timer_get_time() / 1000; //  !< ms
            dummy_timer = esp_timer_get_time() / 1000;
            while ((p != FINGERPRINT_OK) && ((dummy_timer - start_time) < 2000))
            {
                p = Store(uart_channel_num,
                          1,                                  /*ChrBuffer = 1*/
                          custom_USER_ID,                     /*store at Desired Page_id*/
                          (user_data->recieved_buffer), 800); /*need to change Page_id [default = 1]*/

                dummy_timer = esp_timer_get_time() / 1000;
                if ((p != FINGERPRINT_OK) && (dummy_timer - start_time) > 2000)
                {
                    TRACE_W(" Template Storage failed .. Retry by placing the finger again... after 3 seconds");
                    return (0);
                }
            }
        }
        // 5. OK Stored!
        res_ID = custom_USER_ID;
        TRACE_D("--------------------------- EXIT: .[MODE:-1]. ----------------------------");
    }
    return res_ID; // return the ID (where operation is taking place)
}

//--------------------------------- Function to configure fingerprint sensor ------------------------------------------------------
fingerprint_status_t fingerprint_config(l_ezlopi_item_t *item)
{
    fingerprint_status_t F_res = FINGERPRINT_FAIL;
    if (NULL != item)
    {
        server_packet_t *user_data = (server_packet_t *)item->user_arg;
        int uart_channel_num = item->interface.uart.channel;
        // waiting for extra 200ms
        vTaskDelay(200 / portTICK_PERIOD_MS);
        if (PortControl(uart_channel_num, UART_PORT_ON, (user_data->recieved_buffer), 1000))
        {
            vTaskDelay(500 / portTICK_PERIOD_MS);
        }

        if (VerifyPwd(uart_channel_num, (0), (user_data->recieved_buffer), 1000))
        {
            vTaskDelay(500 / portTICK_PERIOD_MS);
        }

        if (SetSysPara(uart_channel_num, FINGERPRINT_BAUDRATE_CONTROL, FINGERPRINT_BAUDRATE_57600, (user_data->recieved_buffer), 1000))
        {
            vTaskDelay(500 / portTICK_PERIOD_MS);
        }

        if (SetSysPara(uart_channel_num, FINGERPRINT_SECURITY_LEVEL, FINGERPRINT_SECURITY_4, (user_data->recieved_buffer), 1000))
        {
            vTaskDelay(500 / portTICK_PERIOD_MS);
        }

        if (SetSysPara(uart_channel_num, FINGERPRINT_DATA_PACKAGE_LENGTH, FINGERPRINT_DATA_LENGTH_64, (user_data->recieved_buffer), 1000))
        {
            vTaskDelay(500 / portTICK_PERIOD_MS);
        }

        TRACE_D("------------  >> STARTING THE SYSTEM << -------------------");
        for (uint8_t i = 0; i < 2; i++)
        {
            if (LedControl(uart_channel_num, 0, (user_data->recieved_buffer), 200))
            {
                TRACE_D("           >> LED OFF <<");
                vTaskDelay(200 / portTICK_PERIOD_MS);
            }
            if (LedControl(uart_channel_num, 1, (user_data->recieved_buffer), 200))
            {
                TRACE_D("           >> LED ON <<");
                vTaskDelay(200 / portTICK_PERIOD_MS);
            }
        }

        F_res = FINGERPRINT_OK;
    }
    return F_res;
}
