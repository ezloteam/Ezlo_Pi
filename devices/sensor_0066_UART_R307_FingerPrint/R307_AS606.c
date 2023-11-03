#include "trace.h"
#include "ezlopi_uart.h"
#include "sensor_0066_UART_R307_FingerPrint.h"

//---------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
// Function for Communication-Packet (Generation and Response).
//---------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------

/**
 * @brief This is a function generates/prepares appropriate transmission packets actions.
 *
 * @param txPacket(fingerprint_packet_t*): Address to a buffer, that stores the generated transmission packet
 * @param PID(uint8_t): Packet Identifier Type
 * @param length(uint16_t): Packet_length value
 * @param Combined_data(uint8_t*): Address to buffer containing, [instruction code + data values (if any)]
 *
 * @return Success=[>0] / Fail=[0].
 */
static void generate_packet(fingerprint_packet_t *txPacket, uint8_t PID, uint16_t length, uint8_t *Combined_data)
{
    //------------ PID ------------------------------------------------------------------------------------------------------
    txPacket->PID = PID;
    //------------ Packet length -------------------------------------------------------------------------------------------
    txPacket->Packet_len[0] = (uint8_t)(length >> 8);   /*MSB [PID + P_LEN + (data_fields)] should not exceed 256bytes*/
    txPacket->Packet_len[1] = (uint8_t)(length & 0xFF); /*LSB [PID + P_LEN + (data_fields)] should not exceed 256bytes*/
    //------------ Inst_code + Data_content  --------------------------------------------------------------------------------
    if (length < MAX_PACKET_LENGTH_VAL)
    {
        memcpy(txPacket->data, Combined_data, ((int)length) - 2); // Copying only data section
    }
    else
    {
        memcpy(txPacket->data, Combined_data, (MAX_PACKET_LENGTH_VAL - 2));
    }
    //------------ Checksum -------------------------------------------------------------------------------------------------
    uint16_t sum = (uint16_t)FINGERPRINT_PID_COMMANDPACKET + (uint16_t)txPacket->Packet_len[0] + (uint16_t)txPacket->Packet_len[1]; // adding
    for (uint8_t i = 0; i < (int)((txPacket->Packet_len[0] << 8) + txPacket->Packet_len[1]) - 2; i++)
    {
        sum += (uint16_t)txPacket->data[i];
        TRACE_E("0x%x", (uint16_t)txPacket->data[i]);
    };
    txPacket->chk_sum[0] = (uint8_t)(sum >> 8);
    txPacket->chk_sum[1] = (uint8_t)(sum & 0xFF);
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
    //------------------------------------------------------------------------------------------
    // 1. upto  packet_len
    /**
     * int uart_write_bytes(uart_port_t uart_num, const void *src, size_t size)
     *  {
     *      ESP_RETURN_ON_FALSE((uart_num < UART_NUM_MAX), (-1), UART_TAG, "uart_num error");
     *      ESP_RETURN_ON_FALSE((p_uart_obj[uart_num] != NULL), (-1), UART_TAG, "uart driver error");
     *      ESP_RETURN_ON_FALSE(src, (-1), UART_TAG, "buffer null");
     *      return uart_tx_all(uart_num, src, size, 0, 0);
     *  }
     */
    // !< Internal esp_hal uart command
    ret = uart_write_bytes(uart_channel_num, txPacket->header_code, 2);
    ret = uart_write_bytes(uart_channel_num, txPacket->device_address, 4);
    ret = uart_write_bytes(uart_channel_num, &(txPacket->PID), 1);
    ret = uart_write_bytes(uart_channel_num, txPacket->Packet_len, 2);
    // 2. data_section (Inst_code + Data_content)
    uint8_t **ptr_d = &(txPacket->data);
    while (NULL != *ptr_d)
    {
        TRACE_E("0x%x", *ptr_d);
        ret = uart_write_bytes(uart_channel_num, *ptr_d, 1);
        (*ptr_d)++;
    }
    // 3. Checksum
    ret = uart_write_bytes(uart_channel_num, txPacket->chk_sum, 2);
    ESP_ERROR_CHECK(uart_wait_tx_done(uart_channel_num, 100)); // wait timeout is 100 RTOS ticks (TickType_t)
    //----------------------------------------------------------------------------------------------------------------------
    return ret;
}

/**
 * @brief This is a function to execute sequential actions :- generate packet and send_packet them through UART_buffer
 *
 * @param uart_channel_num(int): uart_channel number
 * @param txPacket(fingerprint_packet_t*): Address to the buffer holding, Transmission packet
 * @param PID(uint8_t): Packet Identifier Type
 * @param length(uint16_t): Packet_length value
 * @param Combined_data(uint8_t*): Address to buffer containing, [instruction code + data values (if any)]
 *
 * @return Success=[true] / Fail=[false].
 */
static bool SEND_PACKET(int uart_channel_num, fingerprint_packet_t *txPacket, uint8_t PID, uint16_t length, uint8_t *Combined_data)
{
    /* First :- create a txPacket */
    generate_packet(txPacket, PID, length, Combined_data);
    /* Second :- send created Packet through uart*/
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
static FINGERPRINT_STATUS_t __Response_function(uint8_t *recieved_buffer, uint32_t timeout)
{
    FINGERPRINT_STATUS_t F_res = FINGERPRINT_FAIL;
    // polling to check
    uint32_t timer = 0;
    while (timer <= timeout)
    {
        //------------ check if the recieved PID is ack packet  --------------------------------------------------------------------------------
        if (FINGERPRINT_PID_ACKPACKET == recieved_buffer[0])
        {
            TRACE_W("Time taken : %u_ms", timer);
            break; // break away from while() , if we have the correct buffer values
        }
        vTaskDelay(1 / portTICK_PERIOD_MS);
        timer++;
    }
    //------------ Check 'Confirmation code' of the ack packet: And give response --------------------------------------------------------------------------------
    if (!(timer > timeout) && (FINGERPRINT_PID_ACKPACKET == recieved_buffer[0]))
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
        uint16_t p_len = (((uint16_t)recieved_buffer[1] << 8) + ((uint16_t)recieved_buffer[2] & 0xFF));
        TRACE_E("No of bytes in recieved Packet : [%#x (MSB) , %#x (LSB)] => %u", recieved_buffer[1], recieved_buffer[2], p_len);
        for (int x = 0; x <= (int)p_len; x++)
        {
            TRACE_E("recieved Packets[%d] : %#x ", x, recieved_buffer[x]); // goes upto one extra index . Thus terminating with zero .
        }

        // check confirmation code
        TRACE_E("confirmation_code : %#x ", recieved_buffer[3]);
        switch ((uint8_t)recieved_buffer[3])
        {
        case ACK_OK:
            TRACE_E("[commad execution complete.....] ");
            F_res = FINGERPRINT_OK;
            break;
        case ACK_ERR_RECV:
            TRACE_E(".... ERR in recieving Package ... ");
            // F_res = FINGERPRINT_FAIL;
            break;
        case ACK_ERR_DETECT_FP:
            TRACE_E(".... ERR: Can't detect finger ... ");
            // F_res = FINGERPRINT_FAIL;
            break;
        case ACK_ERR_ENROLL_FP:
            TRACE_E(".... ERR: Failed to Collect/Enroll finger ... ");
            // F_res = FINGERPRINT_FAIL;
            break;
        case ACK_ERR_DIS_FP:
            TRACE_E(".... ERR: Failed to generate character file (distorted fingerprint-img) ... ");
            // F_res = FINGERPRINT_FAIL;
            break;
        case ACK_ERR_SMALL_FP:
            TRACE_E(".... ERR: Failed to generate character file (lackness of character point or over-smallness of fingerprint) ... ");
            // F_res = FINGERPRINT_FAIL;
            break;
        case ACK_ERR_MATCH:
            TRACE_E(".... ERR: Templates from both Charbuffers(1 & 2) arenot matching ... ");
            // F_res = FINGERPRINT_FAIL;
            break;
        case ACK_ERR_NO_LIB_MATCH:
            TRACE_E(".... ERR: Not matching with the library (both the PageID and matching score are 0) ... ");
            // F_res = FINGERPRINT_FAIL;
            break;
        case ACK_ERR_CMB_CHRFILE:
            TRACE_E(".... ERR: Failed to combine the character files (character files donot belong to same finger) ... ");
            // F_res = FINGERPRINT_FAIL;
            break;
        case ACK_ERR_ID_BYND_LIB:
            TRACE_E(".... ERR: Addressed PageID is beyond the finger library ... ");
            // F_res = FINGERPRINT_FAIL;
            break;
        case ACK_ERR_LOAD_CHR:
            TRACE_E(".... ERR: Failed to load template/character_file from finger library (or choosen template is invalid) ... ");
            // F_res = FINGERPRINT_FAIL;
            break;
        case ACK_ERR_UP_CHR:
            TRACE_E(".... ERR: Failed to upload character_file (uploading template to computer) ... ");
            // F_res = FINGERPRINT_FAIL;
            break;
        case ACK_ERR_DWN_IMG_CHR:
            TRACE_E(".... ERR: Failed to recieve data packet (downloading image or character) ... ");
            // F_res = FINGERPRINT_FAIL;
            break;
        case ACK_ERR_UP_IMG:
            TRACE_E(".... ERR: Failed to upload image (uploading image to computer) ... ");
            // F_res = FINGERPRINT_FAIL;
            break;
        case ACK_ERR_DEL:
            TRACE_E(".... ERR: Failed to Delete (N)Templates From PageID ... ");
            // F_res = FINGERPRINT_FAIL;
            break;
        case ACK_ERR_CLR:
            TRACE_E(".... ERR: Failed to Clear all Templates From finger library ... ");
            // F_res = FINGERPRINT_FAIL;
            break;
        case ACK_ERR_WRONG_PASS:
            TRACE_E(".... ERR in Password .. ");
            // F_res = FINGERPRINT_FAIL;
            break;
        case ACK_ERR_PRIM_IMG:
            TRACE_E(".... ERR: Failed to generate IMG (lack of valid primary image) ... ");
            // F_res = FINGERPRINT_FAIL;
            break;
        case ACK_ERR_STORE:
            TRACE_E(".... ERR: When writing/storing template into flash library ... ");
            // F_res = FINGERPRINT_FAIL;
            break;
        case ACK_ERR_WRNG_REGS:
            TRACE_E(".... ERR: wrong register number .. ");
            // F_res = FINGERPRINT_FAIL;
            break;
        case ACK_ERR_OP_FAIL:
            TRACE_E(".... ERR: Failed to operate communication ... ");
            // F_res = FINGERPRINT_FAIL;
            break;
        default:
            TRACE_E(".... ERR: Unknown ... ");
            // F_res = FINGERPRINT_FAIL;
            break;
        }
    }
    else
    {
        TRACE_W("SEND_PACKET: ....Time-out");
        // F_res = FINGERPRINT_FAIL;
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
    FINGERPRINT_STATUS_t F_res = FINGERPRINT_FAIL;

    fingerprint_packet_t *txPacket = (fingerprint_packet_t *)malloc(sizeof(fingerprint_packet_t));
    if (txPacket)
    {
        txPacket->header_code[0] = FINGERPRINT_HEADER_MSB;
        txPacket->header_code[1] = FINGERPRINT_HEADER_LSB;
        txPacket->device_address[0] = txPacket->device_address[1] = txPacket->device_address[2] = txPacket->device_address[3] = FINGERPRINT_DEVICE_ADDR_BIT;

        //------------ Inst_code + Data_content  --------------------------------------------------------------------------------
        uint8_t Combined_data[] = {
            FINGERPRINT_VERIFYPASSWORD,     /*INS CODE [1Byte]*/
            (uint8_t)(the_password >> 24),  /*MSB sent first*/
            (uint8_t)(the_password >> 16),  /*MSB sent first*/
            (uint8_t)(the_password >> 8),   /*MSB sent first*/
            (uint8_t)(the_password & 0xFF), /*LSB sent last*/
        };
        //------------ Fill the packet container and send it via uart  --------------------------------------------------------------------------------
        bool res = SEND_PACKET(uart_channel_num,                      /* UART CHANNEL NUMBER */
                               txPacket,                              /* Address of packet container */
                               FINGERPRINT_PID_COMMANDPACKET,         /* Packet Identifier CMD*/
                               (uint16_t)(sizeof(Combined_data) + 2), /* length <= combined_data + chk_sum */
                               Combined_data);                        /* Inst_code + Data_content*/
        //------------ Check of the appropriate responce  --------------------------------------------------------------------------------
        // If the tx is succesfull ; Read "recieved_buffer" that stores, the incoming message from the uart_ISR_upcall
        if (res)
        {
            F_res = __Response_function(recieved_buffer, timeout);
            if (FINGERPRINT_OK == F_res)
            {
                uint16_t Checksum = ((uint16_t)recieved_buffer[4] << 8) + ((uint16_t)recieved_buffer[5] & 0xFF); //(1~12)
                TRACE_I("VerifyPwd =>Checksum [4]: %#x", Checksum);
            }
        }
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
    FINGERPRINT_STATUS_t F_res = FINGERPRINT_FAIL;

    fingerprint_packet_t *txPacket = (fingerprint_packet_t *)malloc(sizeof(fingerprint_packet_t));
    if (txPacket)
    {
        txPacket->header_code[0] = FINGERPRINT_HEADER_MSB;
        txPacket->header_code[1] = FINGERPRINT_HEADER_LSB;
        txPacket->device_address[0] = txPacket->device_address[1] = txPacket->device_address[2] = txPacket->device_address[3] = FINGERPRINT_DEVICE_ADDR_BIT;

        //------------ Inst_code + Data_content  --------------------------------------------------------------------------------
        uint8_t Combined_data[] = {
            FINGERPRINT_SETADDRESS,        /*INS CODE [1Byte]*/
            (uint8_t)(new_address >> 24),  /*MSB sent first*/
            (uint8_t)(new_address >> 16),  /*MSB sent first*/
            (uint8_t)(new_address >> 8),   /*MSB sent first*/
            (uint8_t)(new_address & 0xFF), /*LSB sent last*/
        };
        //------------ Fill the packet container and send it via uart  --------------------------------------------------------------------------------
        bool res = SEND_PACKET(uart_channel_num,                      /* UART CHANNEL NUMBER */
                               txPacket,                              /* Address of packet container */
                               FINGERPRINT_PID_COMMANDPACKET,         /* Packet Identifier CMD*/
                               (uint16_t)(sizeof(Combined_data) + 2), /* length <= combined_data + chk_sum */
                               Combined_data);                        /* Inst_code + Data_content*/
        //------------ Check of the appropriate responce  --------------------------------------------------------------------------------
        // If the tx is succesfull ; Read "recieved_buffer" that stores, the incoming message from the uart_ISR_upcall
        if (res)
        {
            F_res = __Response_function(recieved_buffer, timeout);
            if (FINGERPRINT_OK == F_res)
            {
                uint16_t Checksum = ((uint16_t)recieved_buffer[4] << 8) + ((uint16_t)recieved_buffer[5] & 0xFF); //(1~12)
                TRACE_I("SetAdder =>Checksum [4]: %#x", Checksum);
            }
        }
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
    FINGERPRINT_STATUS_t F_res = FINGERPRINT_FAIL;

    fingerprint_packet_t *txPacket = (fingerprint_packet_t *)malloc(sizeof(fingerprint_packet_t));
    if (txPacket)
    {
        txPacket->header_code[0] = FINGERPRINT_HEADER_MSB;
        txPacket->header_code[1] = FINGERPRINT_HEADER_LSB;
        txPacket->device_address[0] = txPacket->device_address[1] = txPacket->device_address[2] = txPacket->device_address[3] = FINGERPRINT_DEVICE_ADDR_BIT;

        //------------ Inst_code + Data_content  --------------------------------------------------------------------------------
        uint8_t Combined_data[] = {
            FINGERPRINT_SETSYSPARAM, /*INS CODE [1Byte]*/
            Parameter_Number,        /* Paramter Number */
            Parameter_content,       /* Paramter Content */
        };
        //------------ Fill the packet container and send it via uart  --------------------------------------------------------------------------------
        bool res = SEND_PACKET(uart_channel_num,                      /* UART CHANNEL NUMBER */
                               txPacket,                              /* Address of packet container */
                               FINGERPRINT_PID_COMMANDPACKET,         /* Packet Identifier CMD*/
                               (uint16_t)(sizeof(Combined_data) + 2), /* length <= combined_data + chk_sum */
                               Combined_data);                        /* Inst_code + Data_content*/
        //------------ Check of the appropriate responce  --------------------------------------------------------------------------------
        // If the tx is succesfull ; Read "recieved_buffer" that stores, the incoming message from the uart_ISR_upcall
        if (res)
        {
            F_res = __Response_function(recieved_buffer, timeout);
            if (FINGERPRINT_OK == F_res)
            {
                uint16_t Checksum = ((uint16_t)recieved_buffer[4] << 8) + ((uint16_t)recieved_buffer[5] & 0xFF); //(1~12)
                TRACE_I("SetSysPara =>Checksum [4]: %#x", Checksum);
            }
        }
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
    FINGERPRINT_STATUS_t F_res = FINGERPRINT_FAIL;

    fingerprint_packet_t *txPacket = (fingerprint_packet_t *)malloc(sizeof(fingerprint_packet_t));
    if (txPacket)
    {
        txPacket->header_code[0] = FINGERPRINT_HEADER_MSB;
        txPacket->header_code[1] = FINGERPRINT_HEADER_LSB;
        txPacket->device_address[0] = txPacket->device_address[1] = txPacket->device_address[2] = txPacket->device_address[3] = FINGERPRINT_DEVICE_ADDR_BIT;

        //------------ Inst_code + Data_content  --------------------------------------------------------------------------------
        uint8_t Combined_data[] = {
            FINGERPRINT_PORTCONTROL, /*INS CODE [1Byte]*/
            Control_code,            /* ON / OFF */
        };
        //------------ Fill the packet container and send it via uart  --------------------------------------------------------------------------------
        bool res = SEND_PACKET(uart_channel_num,                      /* UART CHANNEL NUMBER */
                               txPacket,                              /* Address of packet container */
                               FINGERPRINT_PID_COMMANDPACKET,         /* Packet Identifier CMD*/
                               (uint16_t)(sizeof(Combined_data) + 2), /* length <= combined_data + chk_sum */
                               Combined_data);                        /* Inst_code + Data_content*/
        //------------ Check of the appropriate responce  --------------------------------------------------------------------------------
        // If the tx is succesfull ; Read "recieved_buffer" that stores, the incoming message from the uart_ISR_upcall
        if (res)
        {
            F_res = __Response_function(recieved_buffer, timeout);
            if (FINGERPRINT_OK == F_res)
            {
                uint16_t Checksum = ((uint16_t)recieved_buffer[4] << 8) + ((uint16_t)recieved_buffer[5] & 0xFF); //(1~12)
                TRACE_I("PortControl =>Checksum [4]: %#x", Checksum);
            }
        }
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
    uint16_t Status_register = 0;
    // Reset the recieving buffer before new data is to be extracted.
    memset(recieved_buffer, 0, MAX_PACKET_LENGTH_VAL);
    // create the packet here
    FINGERPRINT_STATUS_t F_res = FINGERPRINT_FAIL;

    fingerprint_packet_t *txPacket = (fingerprint_packet_t *)malloc(sizeof(fingerprint_packet_t));
    if (txPacket)
    {
        txPacket->header_code[0] = FINGERPRINT_HEADER_MSB;
        txPacket->header_code[1] = FINGERPRINT_HEADER_LSB;
        txPacket->device_address[0] = txPacket->device_address[1] = txPacket->device_address[2] = txPacket->device_address[3] = FINGERPRINT_DEVICE_ADDR_BIT;

        //------------ Inst_code + Data_content  --------------------------------------------------------------------------------
        uint8_t Combined_data[] = {
            FINGERPRINT_READSYSPARAM, /*INS CODE [1Byte]*/
        };
        //------------ Fill the packet container and send it via uart  --------------------------------------------------------------------------------
        bool res = SEND_PACKET(uart_channel_num,                      /* UART CHANNEL NUMBER */
                               txPacket,                              /* Address of packet container */
                               FINGERPRINT_PID_COMMANDPACKET,         /* Packet Identifier CMD*/
                               (uint16_t)(sizeof(Combined_data) + 2), /* length <= combined_data + chk_sum */
                               Combined_data);                        /* Inst_code + Data_content*/
        //------------ Check of the appropriate responce  --------------------------------------------------------------------------------
        // If the tx is succesfull ; Read "recieved_buffer" that stores, the incoming message from the uart_ISR_upcall
        if (res)
        {
            F_res = __Response_function(recieved_buffer, timeout);
            if (FINGERPRINT_OK == F_res)
            {
                Status_register = ((uint16_t)recieved_buffer[4] << 8) + ((uint16_t)recieved_buffer[5] & 0xFF);
                uint16_t System_identifier_code = ((uint16_t)recieved_buffer[6] << 8) + ((uint16_t)recieved_buffer[7] & 0xFF); // 0x0009
                uint16_t Finger_library_size = ((uint16_t)recieved_buffer[8] << 8) + ((uint16_t)recieved_buffer[9] & 0xFF);    // (0~999)
                uint16_t Security_level = ((uint16_t)recieved_buffer[10] << 8) + ((uint16_t)recieved_buffer[11] & 0xFF);       // (1~5)
                // uint32_t Device_address = (((uint32_t)recieved_buffer[12] << 14) +
                //                            ((uint32_t)recieved_buffer[13] << 16) +
                //                            ((uint32_t)recieved_buffer[14] << 8) +
                //                            ((uint32_t)recieved_buffer[15] & 0xFF));
                uint16_t Data_packet_size = ((uint16_t)recieved_buffer[16] << 8) + ((uint16_t)recieved_buffer[17] & 0xFF); // (0~3)
                uint16_t Baud_setting = ((uint16_t)recieved_buffer[18] << 8) + ((uint16_t)recieved_buffer[19] & 0xFF);     //(1~12)
                uint16_t Checksum = ((uint16_t)recieved_buffer[20] << 8) + ((uint16_t)recieved_buffer[21] & 0xFF);         //(1~12)

                TRACE_I("ReadSysPara : Status_register [4]: %#x", Status_register);
                TRACE_I("ReadSysPara : System_identifier_code [6]: %#x", System_identifier_code);
                TRACE_I("ReadSysPara : Finger_library_size [8]: %#x", Finger_library_size);
                TRACE_I("ReadSysPara : Security_level [10]: %#x", Security_level);
                TRACE_I("ReadSysPara : Data_packet_size [16]: %#x", Data_packet_size);
                TRACE_I("ReadSysPara : Baud_setting [18]: %#x", Baud_setting);
                TRACE_I("ReadSysPara =>Checksum [20]: %#x", Checksum);
            }
        }
        // writing the extracted value to the "Status_bits" buffer
        *Status_bits = Status_register;
        free(txPacket);
    }
    //----------------------------------------------------------------------------------------------------------------------
    return (bool)F_res;
}

/**
 * @brief #### This function Read valid template number.
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
    FINGERPRINT_STATUS_t F_res = FINGERPRINT_FAIL;

    fingerprint_packet_t *txPacket = (fingerprint_packet_t *)malloc(sizeof(fingerprint_packet_t));
    if (txPacket)
    {
        txPacket->header_code[0] = FINGERPRINT_HEADER_MSB;
        txPacket->header_code[1] = FINGERPRINT_HEADER_LSB;
        txPacket->device_address[0] = txPacket->device_address[1] = txPacket->device_address[2] = txPacket->device_address[3] = FINGERPRINT_DEVICE_ADDR_BIT;

        //------------ Inst_code + Data_content  --------------------------------------------------------------------------------
        uint8_t Combined_data[] = {
            FINGERPRINT_TEMPLATENUM, /*INS CODE [1Byte]*/
        };
        //------------ Fill the packet container and send it via uart  --------------------------------------------------------------------------------
        bool res = SEND_PACKET(uart_channel_num,                      /* UART CHANNEL NUMBER */
                               txPacket,                              /* Address of packet container */
                               FINGERPRINT_PID_COMMANDPACKET,         /* Packet Identifier CMD*/
                               (uint16_t)(sizeof(Combined_data) + 2), /* length <= combined_data + chk_sum */
                               Combined_data);                        /* Inst_code + Data_content*/
        //------------ Check of the appropriate responce  --------------------------------------------------------------------------------
        // If the tx is succesfull ; Read "recieved_buffer" that stores, the incoming message from the uart_ISR_upcall
        if (res)
        {
            F_res = __Response_function(recieved_buffer, timeout);
            if (FINGERPRINT_OK == F_res)
            {
                TempleteNum = ((uint16_t)recieved_buffer[4] << 8) + ((uint16_t)recieved_buffer[5] & 0xFF);
                TRACE_I("TempleteNum [4]: %#x", TempleteNum);
                uint16_t Checksum = ((uint16_t)recieved_buffer[6] << 8) + ((uint16_t)recieved_buffer[7] & 0xFF); //(1~12)
                TRACE_I("ReadTempNum =>Checksum [6]: %#x", Checksum);
            }
        }
        free(txPacket);
    }
    // writing the extracted value to the "TempleteNum"
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
    FINGERPRINT_STATUS_t F_res = FINGERPRINT_FAIL;

    fingerprint_packet_t *txPacket = (fingerprint_packet_t *)malloc(sizeof(fingerprint_packet_t));
    if (txPacket)
    {
        txPacket->header_code[0] = FINGERPRINT_HEADER_MSB;
        txPacket->header_code[1] = FINGERPRINT_HEADER_LSB;
        txPacket->device_address[0] = txPacket->device_address[1] = txPacket->device_address[2] = txPacket->device_address[3] = FINGERPRINT_DEVICE_ADDR_BIT;

        //------------ Inst_code + Data_content  --------------------------------------------------------------------------------
        uint8_t Combined_data[] = {
            FINGERPRINT_GR_IDENTIFY, /*INS CODE [1Byte]*/
        };
        //------------ Fill the packet container and send it via uart  --------------------------------------------------------------------------------
        bool res = SEND_PACKET(uart_channel_num,                      /* UART CHANNEL NUMBER */
                               txPacket,                              /* Address of packet container */
                               FINGERPRINT_PID_COMMANDPACKET,         /* Packet Identifier CMD*/
                               (uint16_t)(sizeof(Combined_data) + 2), /* length <= combined_data + chk_sum */
                               Combined_data);                        /* Inst_code + Data_content*/
        //------------ Check of the appropriate responce  --------------------------------------------------------------------------------
        // If the tx is succesfull ; Read "recieved_buffer" that stores, the incoming message from the uart_ISR_upcall
        if (res)
        {
            F_res = __Response_function(recieved_buffer, timeout);
            if (FINGERPRINT_OK == F_res)
            {
                Page_num = ((uint16_t)recieved_buffer[4] << 8) + ((uint16_t)recieved_buffer[5] & 0xFF);
                Match_score = ((uint16_t)recieved_buffer[6] << 8) + ((uint16_t)recieved_buffer[7] & 0xFF);       // 0x0009
                uint16_t Checksum = ((uint16_t)recieved_buffer[8] << 8) + ((uint16_t)recieved_buffer[9] & 0xFF); //(1~12)
                TRACE_I("GR_Identify : Page_id [4]: %#x", Page_num);
                TRACE_I("GR_Identify : Match_score [6]: %#x", Match_score);
                TRACE_I("GR_Identify =>Checksum [8]: %#x", Checksum);
            }
        }
        free(txPacket);
    }
    // writing the extracted value to the "PageID_ptr & MatchScore_ptr" buffer
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
    FINGERPRINT_STATUS_t F_res = FINGERPRINT_FAIL;

    fingerprint_packet_t *txPacket = (fingerprint_packet_t *)malloc(sizeof(fingerprint_packet_t));
    if (txPacket)
    {
        txPacket->header_code[0] = FINGERPRINT_HEADER_MSB;
        txPacket->header_code[1] = FINGERPRINT_HEADER_LSB;
        txPacket->device_address[0] = txPacket->device_address[1] = txPacket->device_address[2] = txPacket->device_address[3] = FINGERPRINT_DEVICE_ADDR_BIT;

        //------------ Inst_code + Data_content  --------------------------------------------------------------------------------
        uint8_t Combined_data[] = {
            FINGERPRINT_GETIMAGE, /*INS CODE [1Byte]*/
        };
        //------------ Fill the packet container and send it via uart  --------------------------------------------------------------------------------
        bool res = SEND_PACKET(uart_channel_num,                      /* UART CHANNEL NUMBER */
                               txPacket,                              /* Address of packet container */
                               FINGERPRINT_PID_COMMANDPACKET,         /* Packet Identifier CMD*/
                               (uint16_t)(sizeof(Combined_data) + 2), /* length <= combined_data + chk_sum */
                               Combined_data);                        /* Inst_code + Data_content*/
        //------------ Check of the appropriate responce  --------------------------------------------------------------------------------
        // If the tx is succesfull ; Read "recieved_buffer" that stores, the incoming message from the uart_ISR_upcall
        if (res)
        {
            F_res = __Response_function(recieved_buffer, timeout);
            if (FINGERPRINT_OK == F_res)
            {
                uint16_t Checksum = ((uint16_t)recieved_buffer[4] << 8) + ((uint16_t)recieved_buffer[5] & 0xFF); //(1~12)
                TRACE_I("GenImg =>Checksum [4]: %#x", Checksum);
            }
        }
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
    FINGERPRINT_STATUS_t F_res = FINGERPRINT_FAIL;

    fingerprint_packet_t *txPacket = (fingerprint_packet_t *)malloc(sizeof(fingerprint_packet_t));
    if (txPacket)
    {
        txPacket->header_code[0] = FINGERPRINT_HEADER_MSB;
        txPacket->header_code[1] = FINGERPRINT_HEADER_LSB;
        txPacket->device_address[0] = txPacket->device_address[1] = txPacket->device_address[2] = txPacket->device_address[3] = FINGERPRINT_DEVICE_ADDR_BIT;

        //------------ Inst_code + Data_content  --------------------------------------------------------------------------------
        uint8_t Combined_data[] = {
            FINGERPRINT_IMAGE2TZ, /*INS CODE [1Byte]*/
            CharBufferID,         /*BufferId = CharBuf1 or CharBuf2*/
        };
        //------------ Fill the packet container and send it via uart  --------------------------------------------------------------------------------
        bool res = SEND_PACKET(uart_channel_num,                      /* UART CHANNEL NUMBER */
                               txPacket,                              /* Address of packet container */
                               FINGERPRINT_PID_COMMANDPACKET,         /* Packet Identifier CMD*/
                               (uint16_t)(sizeof(Combined_data) + 2), /* length <= combined_data + chk_sum */
                               Combined_data);                        /* Inst_code + Data_content*/
        //------------ Check of the appropriate responce  --------------------------------------------------------------------------------
        // If the tx is succesfull ; Read "recieved_buffer" that stores, the incoming message from the uart_ISR_upcall
        if (res)
        {
            F_res = __Response_function(recieved_buffer, timeout);
            if (FINGERPRINT_OK == F_res)
            {
                uint16_t Checksum = ((uint16_t)recieved_buffer[4] << 8) + ((uint16_t)recieved_buffer[5] & 0xFF); //(1~12)
                TRACE_I("Img2Tz =>Checksum [4]: %#x", Checksum);
            }
        }
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
    FINGERPRINT_STATUS_t F_res = FINGERPRINT_FAIL;

    fingerprint_packet_t *txPacket = (fingerprint_packet_t *)malloc(sizeof(fingerprint_packet_t));
    if (txPacket)
    {
        txPacket->header_code[0] = FINGERPRINT_HEADER_MSB;
        txPacket->header_code[1] = FINGERPRINT_HEADER_LSB;
        txPacket->device_address[0] = txPacket->device_address[1] = txPacket->device_address[2] = txPacket->device_address[3] = FINGERPRINT_DEVICE_ADDR_BIT;

        //------------ Inst_code + Data_content  --------------------------------------------------------------------------------
        uint8_t Combined_data[] = {
            FINGERPRINT_REGMODEL, /*INS CODE [1Byte]*/
        };
        //------------ Fill the packet container and send it via uart  --------------------------------------------------------------------------------
        bool res = SEND_PACKET(uart_channel_num,                      /* UART CHANNEL NUMBER */
                               txPacket,                              /* Address of packet container */
                               FINGERPRINT_PID_COMMANDPACKET,         /* Packet Identifier CMD*/
                               (uint16_t)(sizeof(Combined_data) + 2), /* length <= combined_data + chk_sum */
                               Combined_data);                        /* Inst_code + Data_content*/
        //------------ Check of the appropriate responce  --------------------------------------------------------------------------------
        // If the tx is succesfull ; Read "recieved_buffer" that stores, the incoming message from the uart_ISR_upcall
        if (res)
        {
            F_res = __Response_function(recieved_buffer, timeout);
            if (FINGERPRINT_OK == F_res)
            {
                uint16_t Checksum = ((uint16_t)recieved_buffer[4] << 8) + ((uint16_t)recieved_buffer[5] & 0xFF); //(1~12)
                TRACE_I("RegModel =>Checksum [4]: %#x", Checksum);
            }
        }
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
    FINGERPRINT_STATUS_t F_res = FINGERPRINT_FAIL;

    fingerprint_packet_t *txPacket = (fingerprint_packet_t *)malloc(sizeof(fingerprint_packet_t));
    if (txPacket)
    {
        txPacket->header_code[0] = FINGERPRINT_HEADER_MSB;
        txPacket->header_code[1] = FINGERPRINT_HEADER_LSB;
        txPacket->device_address[0] = txPacket->device_address[1] = txPacket->device_address[2] = txPacket->device_address[3] = FINGERPRINT_DEVICE_ADDR_BIT;

        //------------ Inst_code + Data_content  --------------------------------------------------------------------------------
        uint8_t Combined_data[] = {
            FINGERPRINT_STORE,        /*INS CODE [1Byte]*/
            CharBufferID,             /*BufferId = CharBuf1 or CharBuf2*/
            (uint8_t)(PageID >> 8),   /*MSB sent first*/
            (uint8_t)(PageID & 0xFF), /*LSB sent last*/
        };
        //------------ Fill the packet container and send it via uart  --------------------------------------------------------------------------------
        bool res = SEND_PACKET(uart_channel_num,                      /* UART CHANNEL NUMBER */
                               txPacket,                              /* Address of packet container */
                               FINGERPRINT_PID_COMMANDPACKET,         /* Packet Identifier CMD*/
                               (uint16_t)(sizeof(Combined_data) + 2), /* length <= combined_data + chk_sum */
                               Combined_data);                        /* Inst_code + Data_content*/
        //------------ Check of the appropriate responce  --------------------------------------------------------------------------------
        // If the tx is succesfull ; Read "recieved_buffer" that stores, the incoming message from the uart_ISR_upcall
        if (res)
        {
            F_res = __Response_function(recieved_buffer, timeout);
            if (FINGERPRINT_OK == F_res)
            {
                uint16_t Checksum = ((uint16_t)recieved_buffer[4] << 8) + ((uint16_t)recieved_buffer[5] & 0xFF); //(1~12)
                TRACE_I("Store =>Checksum [4]: %#x", Checksum);
            }
        }
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
    FINGERPRINT_STATUS_t F_res = FINGERPRINT_FAIL;

    fingerprint_packet_t *txPacket = (fingerprint_packet_t *)malloc(sizeof(fingerprint_packet_t));
    if (txPacket)
    {
        txPacket->header_code[0] = FINGERPRINT_HEADER_MSB;
        txPacket->header_code[1] = FINGERPRINT_HEADER_LSB;
        txPacket->device_address[0] = txPacket->device_address[1] = txPacket->device_address[2] = txPacket->device_address[3] = FINGERPRINT_DEVICE_ADDR_BIT;

        //------------ Inst_code + Data_content  --------------------------------------------------------------------------------
        uint8_t Combined_data[] = {
            FINGERPRINT_LOAD,         /*INS CODE [1Byte]*/
            CharBufferID,             /*BufferId = CharBuf1 or CharBuf2*/
            (uint8_t)(PageID >> 8),   /*MSB sent first*/
            (uint8_t)(PageID & 0xFF), /*LSB sent last*/
        };
        //------------ Fill the packet container and send it via uart  --------------------------------------------------------------------------------
        bool res = SEND_PACKET(uart_channel_num,                      /* UART CHANNEL NUMBER */
                               txPacket,                              /* Address of packet container */
                               FINGERPRINT_PID_COMMANDPACKET,         /* Packet Identifier CMD*/
                               (uint16_t)(sizeof(Combined_data) + 2), /* length <= combined_data + chk_sum */
                               Combined_data);                        /* Inst_code + Data_content*/
        //------------ Check of the appropriate responce  --------------------------------------------------------------------------------
        // If the tx is succesfull ; Read "recieved_buffer" that stores, the incoming message from the uart_ISR_upcall
        if (res)
        {
            F_res = __Response_function(recieved_buffer, timeout);
            if (FINGERPRINT_OK == F_res)
            {
                uint16_t Checksum = ((uint16_t)recieved_buffer[4] << 8) + ((uint16_t)recieved_buffer[5] & 0xFF); //(1~12)
                TRACE_I("Load =>Checksum [4]: %#x", Checksum);
            }
        }
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
    FINGERPRINT_STATUS_t F_res = FINGERPRINT_FAIL;

    fingerprint_packet_t *txPacket = (fingerprint_packet_t *)malloc(sizeof(fingerprint_packet_t));
    if (txPacket)
    {
        txPacket->header_code[0] = FINGERPRINT_HEADER_MSB;
        txPacket->header_code[1] = FINGERPRINT_HEADER_LSB;
        txPacket->device_address[0] = txPacket->device_address[1] = txPacket->device_address[2] = txPacket->device_address[3] = FINGERPRINT_DEVICE_ADDR_BIT;

        //------------ Inst_code + Data_content  --------------------------------------------------------------------------------
        uint8_t Combined_data[] = {
            FINGERPRINT_DELETE,          /*INS CODE [1Byte]*/
            (uint8_t)(PageID >> 8),      /*MSB sent first*/
            (uint8_t)(PageID & 0xFF),    /*LSB sent last*/
            (uint8_t)(TempCount >> 8),   /*MSB sent first*/
            (uint8_t)(TempCount & 0xFF), /*LSB sent last*/
        };
        //------------ Fill the packet container and send it via uart  --------------------------------------------------------------------------------
        bool res = SEND_PACKET(uart_channel_num,                      /* UART CHANNEL NUMBER */
                               txPacket,                              /* Address of packet container */
                               FINGERPRINT_PID_COMMANDPACKET,         /* Packet Identifier CMD*/
                               (uint16_t)(sizeof(Combined_data) + 2), /* length <= combined_data + chk_sum */
                               Combined_data);                        /* Inst_code + Data_content*/
        //------------ Check of the appropriate responce  --------------------------------------------------------------------------------
        // If the tx is succesfull ; Read "recieved_buffer" that stores, the incoming message from the uart_ISR_upcall
        if (res)
        {
            F_res = __Response_function(recieved_buffer, timeout);
            if (FINGERPRINT_OK == F_res)
            {
                uint16_t Checksum = ((uint16_t)recieved_buffer[4] << 8) + ((uint16_t)recieved_buffer[5] & 0xFF); //(1~12)
                TRACE_I("Delete =>Checksum [4]: %#x", Checksum);
            }
        }
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
    FINGERPRINT_STATUS_t F_res = FINGERPRINT_FAIL;

    fingerprint_packet_t *txPacket = (fingerprint_packet_t *)malloc(sizeof(fingerprint_packet_t));
    if (txPacket)
    {
        txPacket->header_code[0] = FINGERPRINT_HEADER_MSB;
        txPacket->header_code[1] = FINGERPRINT_HEADER_LSB;
        txPacket->device_address[0] = txPacket->device_address[1] = txPacket->device_address[2] = txPacket->device_address[3] = FINGERPRINT_DEVICE_ADDR_BIT;

        //------------ Inst_code + Data_content  --------------------------------------------------------------------------------
        uint8_t Combined_data[] = {
            FINGERPRINT_EMPTY, /*INS CODE [1Byte]*/
        };
        //------------ Fill the packet container and send it via uart  --------------------------------------------------------------------------------
        bool res = SEND_PACKET(uart_channel_num,                      /* UART CHANNEL NUMBER */
                               txPacket,                              /* Address of packet container */
                               FINGERPRINT_PID_COMMANDPACKET,         /* Packet Identifier CMD*/
                               (uint16_t)(sizeof(Combined_data) + 2), /* length <= combined_data + chk_sum */
                               Combined_data);                        /* Inst_code + Data_content*/
        //------------ Check of the appropriate responce  --------------------------------------------------------------------------------
        // If the tx is succesfull ; Read "recieved_buffer" that stores, the incoming message from the uart_ISR_upcall
        if (res)
        {
            F_res = __Response_function(recieved_buffer, timeout);
            if (FINGERPRINT_OK == F_res)
            {
                uint16_t Checksum = ((uint16_t)recieved_buffer[4] << 8) + ((uint16_t)recieved_buffer[5] & 0xFF); //(1~12)
                TRACE_I("Empty =>Checksum [4]: %#x", Checksum);
            }
        }
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
    FINGERPRINT_STATUS_t F_res = FINGERPRINT_FAIL;

    fingerprint_packet_t *txPacket = (fingerprint_packet_t *)malloc(sizeof(fingerprint_packet_t));
    if (txPacket)
    {
        txPacket->header_code[0] = FINGERPRINT_HEADER_MSB;
        txPacket->header_code[1] = FINGERPRINT_HEADER_LSB;
        txPacket->device_address[0] = txPacket->device_address[1] = txPacket->device_address[2] = txPacket->device_address[3] = FINGERPRINT_DEVICE_ADDR_BIT;

        //------------ Inst_code + Data_content  --------------------------------------------------------------------------------
        uint8_t Combined_data[] = {
            FINGERPRINT_MATCH, /*INS CODE [1Byte]*/
        };
        //------------ Fill the packet container and send it via uart  --------------------------------------------------------------------------------
        bool res = SEND_PACKET(uart_channel_num,                      /* UART CHANNEL NUMBER */
                               txPacket,                              /* Address of packet container */
                               FINGERPRINT_PID_COMMANDPACKET,         /* Packet Identifier CMD*/
                               (uint16_t)(sizeof(Combined_data) + 2), /* length <= combined_data + chk_sum */
                               Combined_data);                        /* Inst_code + Data_content*/
        //------------ Check of the appropriate responce  --------------------------------------------------------------------------------
        // If the tx is succesfull ; Read "recieved_buffer" that stores, the incoming message from the uart_ISR_upcall
        if (res)
        {
            F_res = __Response_function(recieved_buffer, timeout);
            if (FINGERPRINT_OK == F_res)
            {
                Score = ((uint16_t)recieved_buffer[4] << 8) + ((uint16_t)recieved_buffer[5] & 0xFF);
                uint16_t Checksum = ((uint16_t)recieved_buffer[6] << 8) + ((uint16_t)recieved_buffer[7] & 0xFF); //(1~12)
                TRACE_I("Match : Inspection_Score [4]: %#x", Score);
                TRACE_I("Match =>Checksum [6]: %#x", Checksum);
            }
        }
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
    FINGERPRINT_STATUS_t F_res = FINGERPRINT_FAIL;

    fingerprint_packet_t *txPacket = (fingerprint_packet_t *)malloc(sizeof(fingerprint_packet_t));
    if (txPacket)
    {
        txPacket->header_code[0] = FINGERPRINT_HEADER_MSB;
        txPacket->header_code[1] = FINGERPRINT_HEADER_LSB;
        txPacket->device_address[0] = txPacket->device_address[1] = txPacket->device_address[2] = txPacket->device_address[3] = FINGERPRINT_DEVICE_ADDR_BIT;

        //------------ Inst_code + Data_content  --------------------------------------------------------------------------------
        uint8_t Combined_data[] = {
            FINGERPRINT_SEARCH,          /*INS CODE [1Byte]*/
            CharBufferID,                /*BufferId = CharBuf1 or CharBuf2*/
            (uint8_t)(StartPage >> 8),   /*MSB sent first*/
            (uint8_t)(StartPage & 0xFF), /*LSB sent last*/
            (uint8_t)(PageNum >> 8),     /*MSB sent first*/
            (uint8_t)(PageNum & 0xFF),   /*LSB sent last*/
        };
        //------------ Fill the packet container and send it via uart  --------------------------------------------------------------------------------
        bool res = SEND_PACKET(uart_channel_num,                      /* UART CHANNEL NUMBER */
                               txPacket,                              /* Address of packet container */
                               FINGERPRINT_PID_COMMANDPACKET,         /* Packet Identifier CMD*/
                               (uint16_t)(sizeof(Combined_data) + 2), /* length <= combined_data + chk_sum */
                               Combined_data);                        /* Inst_code + Data_content*/
        //------------ Check of the appropriate responce  --------------------------------------------------------------------------------
        // If the tx is succesfull ; Read "recieved_buffer" that stores, the incoming message from the uart_ISR_upcall
        if (res)
        {
            F_res = __Response_function(recieved_buffer, timeout);
            if (FINGERPRINT_OK == F_res)
            {
                Page_num = ((uint16_t)recieved_buffer[4] << 8) + ((uint16_t)recieved_buffer[5] & 0xFF);
                Match_score = ((uint16_t)recieved_buffer[6] << 8) + ((uint16_t)recieved_buffer[7] & 0xFF);       // 0x0009
                uint16_t Checksum = ((uint16_t)recieved_buffer[8] << 8) + ((uint16_t)recieved_buffer[9] & 0xFF); //(1~12)
                TRACE_I("Search : Page_id [4]: %#x", Page_num);
                TRACE_I("Search : Match_score [6]: %#x", Match_score);
                TRACE_I("Search =>Checksum [8]: %#x", Checksum);
            }
        }
        // only if the search operation is successful, the value at 'PageID_ptr'&'MatchScore_ptr' are to be replaced
        if ((0 != Page_num) && (*PageID_ptr != Page_num) && (0 < Match_score))
        {
            /*Entering here means, duplicate is found*/
            // writing the extracted value to the "PageID_ptr & MatchScore_ptr" buffer
            *PageID_ptr = Page_num;
            *MatchScore_ptr = Match_score;
        }
        free(txPacket);
    }
    //----------------------------------------------------------------------------------------------------------------------
    return (bool)F_res;
}

//---------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
// Function for Operation modes
//---------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------

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
    FINGERPRINT_STATUS_t F_res = FINGERPRINT_FAIL;
    if (NULL != item)
    {
        server_packet_t *user_data = (server_packet_t *)item->user_arg;
        uint16_t __status = 0;
        //<! us time
        uint32_t start_time = esp_timer_get_time();
        while ((((uint32_t)esp_timer_get_time() - start_time) / 1000) <= timeout_ms) // ms
        {
            F_res = ReadSysPara(item->interface.uart.channel, &__status, (user_data->recieved_buffer), 300); // 300ms polling
            if ((FINGERPRINT_OK == F_res) && ((__status & (SYSTEM_HNDSHK_VERIFIED | SYSTEM_FREE)) != 0))
            {
                TRACE_I("... Operation Complete ... System is Free");
                break;
            }
            TRACE_W("Checking:.....System busy");
        }
    }
    return (bool)F_res;
}

//----------------------------- Function to check if Specified ID is occupied or not -----------------------------------------------
/**
 * @brief ### This function checks the perticular USER/PAGE_ID .
 * @return {true ==> empty} / {false == not empty}
 */
bool Check_PAGEID_Empty(l_ezlopi_item_t *item)
{
    if (NULL != item)
    {
        server_packet_t *user_data = (server_packet_t *)item->user_arg;
        uint32_t start_time = 0, dummy_timer = 0;
        FINGERPRINT_STATUS_t p = FINGERPRINT_FAIL; // status checker
        TRACE_I("Checking if user_ID: #%d.. is occupied", user_data->user_id);
        TRACE_I("... Loading the Model to ChrBuffer-1&2 [Phase:-3].........");
        start_time = esp_timer_get_time() / 1000; //  !< ms
        while ((p != FINGERPRINT_OK) && ((dummy_timer - start_time) < 6000))
        {
            // Try loading to into ChrBuffer1
            p = Load(item->interface.uart.channel, 1, (user_data->user_id), (user_data->recieved_buffer), 800);
            vTaskDelay(100 / portTICK_PERIOD_MS);
            dummy_timer = esp_timer_get_time() / 1000;
            if ((p != FINGERPRINT_OK) && (dummy_timer - start_time) > 4000)
            {
                // Result: OK - Internal Occupied!
                TRACE_W("...........Internal PAGE_ID/user_id is empty/invalid")
                return (false);
            }
        }
        // Check if the system is free!
        p = Wait_till_system_free(item, 1000);
        /*might need to add some code incase of system busy*/
    }
    // Result: OK - Internal Empty!
    return (true);
}
//---------------------------------- Function that updates validity status of internal PAGEID to append the new -----------------------------------------
/**
 * @brief ### This function Scans and update validity status of [1~500(max defined)] PAGE_IDs.
 */
void Update_ID_status_list(l_ezlopi_item_t *item)
{
    if (NULL != item)
    {
        server_packet_t *user_data = (server_packet_t *)item->user_arg;
        // Temporay variable
        uint16_t Temp_ID = user_data->user_id;
        for (uint16_t i = 1; i <= FINGERPRINT_MAX_CAPACITY_LIMIT; i++)
        {
            // First update the ID occupancy status in item->user_arg
            user_data->user_id = i;
            if (Check_PAGEID_Empty(item))
            {
                user_data->validity[i] = 1; /*(0) | [1~500]*/
            }
            else
            {
                user_data->validity[i] = 0;
            }
        }
        // return the original user_id
        user_data->user_id = Temp_ID;
    }
}
//---------------------------------- Function that searches, internal library to return PAGE_ID and confidence_level -----------------------------------------
/**
 * @brief ### This function extracts fingerprint after interrupt signal and then
 */
bool Match_ID(l_ezlopi_item_t *item)
{
    bool ret = true;
    if (NULL != item)
    {
        uint32_t start_time = 0, dummy_timer = 0;
        server_packet_t *user_data = (server_packet_t *)item->user_arg;
        //-------------------- 4. Search for all the library and store only if no duplicates found ------------------------------------------------
        FINGERPRINT_STATUS_t p = FINGERPRINT_FAIL;
        start_time = esp_timer_get_time() / 1000; //  !< ms
        while ((p != FINGERPRINT_OK) && ((dummy_timer - start_time) < 4000))
        {
            p = GR_Identify(item->interface.uart.channel,   /*user_channel*/
                            &(user_data->user_id),          /*same_ID => */
                            &(user_data->confidence_level), /*[value=!0]=> */
                            (user_data->recieved_buffer),   /*Uart_buffer address*/
                            800);
            vTaskDelay(100 / portTICK_PERIOD_MS);
            dummy_timer = esp_timer_get_time() / 1000;
            // we use the error[0x0Ch] here, to understand that, matching ID is not present.
            if ((p != FINGERPRINT_OK) && (dummy_timer - start_time) > 1999)
            {
                // return from this task if image genereation failed
                TRACE_W(" Failed to execute the Match Operation . Try again ...........");
                ret = false;
                break;
            }
        }
    }
    return ret;
}

//---------------------------------- Function that searches and erases specified range of IDs -----------------------------------------
/**
 * @brief ### This Function searches and erases specified range of IDs only.
 */
bool Erase_Specified_ID(l_ezlopi_item_t *item)
{
    bool ret = true;
    if (NULL != item)
    {
        uint32_t start_time = 0, dummy_timer = 0;
        server_packet_t *user_data = (server_packet_t *)item->user_arg;
        //-------------------- 4. Search for all the library and store only if no duplicates found ------------------------------------------------
        FINGERPRINT_STATUS_t p = FINGERPRINT_FAIL;
        start_time = esp_timer_get_time() / 1000; //  !< ms
        while ((p != FINGERPRINT_OK) && ((dummy_timer - start_time) < 4000))
        {
            p = Delete(item->interface.uart.channel, /*user_channel*/
                       (user_data->user_id),         /*Starting_point*/
                       (user_data->id_counts),       /*Quantity*/
                       (user_data->recieved_buffer), /*Uart_buffer address*/
                       800);
            vTaskDelay(100 / portTICK_PERIOD_MS);
            dummy_timer = esp_timer_get_time() / 1000;
            // we use the error[0x0Ch] here, to understand that, matching ID is not present.
            if ((p != FINGERPRINT_OK) && (dummy_timer - start_time) > 1999)
            {
                // return from this task if image genereation failed
                TRACE_W(" Failed to Delete (Specified ID-Range) from internal Library . Try again ...........");
                ret = false;
                break;
            }
        }
    }
    return ret;
}

//---------------------------------- Function erases all fingerprints from internal library  -----------------------------------------
/**
 * @brief ### This function erases fingerprint from internal libraray
 *
 */
bool Erase_all_ID(l_ezlopi_item_t *item)
{
    bool ret = true;
    if (NULL != item)
    {
        uint32_t start_time = 0, dummy_timer = 0;
        server_packet_t *user_data = (server_packet_t *)item->user_arg;
        //-------------------- 4. Search for all the library and store only if no duplicates found ------------------------------------------------
        FINGERPRINT_STATUS_t p = FINGERPRINT_FAIL;
        start_time = esp_timer_get_time() / 1000; //  !< ms
        while ((p != FINGERPRINT_OK) && ((dummy_timer - start_time) < 4000))
        {
            p = Empty(item->interface.uart.channel, /*user_channel*/
                      (user_data->recieved_buffer), /*Uart_buffer address*/
                      800);
            vTaskDelay(100 / portTICK_PERIOD_MS);
            dummy_timer = esp_timer_get_time() / 1000;
            // we use the error[0x0Ch] here, to understand that, matching ID is not present.
            if ((p != FINGERPRINT_OK) && (dummy_timer - start_time) > 1999)
            {
                // return from this task if image genereation failed
                TRACE_W(" Failed to Erase all the IDs from internal fingerprint library  . Try again ...........");
                ret = false;
                break;
            }
        }
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
    uint16_t res_ID = 0; /* set to a invalid id*/
    if (NULL != item)
    {
        int uart_channel_num = item->interface.uart.channel;
        server_packet_t *user_data = (server_packet_t *)item->user_arg; // If all conditions are satisfied, then write into this structure.
        uint16_t custom_USER_ID = user_data->user_id;                   // Desired Page_id to be stored
        uint16_t custom_Confidence_level = user_data->confidence_level;

        uint32_t start_time = 0, dummy_timer = 0;

        //----------------------- 1. Collecting First fingerprint -----------------------------------------------
        FINGERPRINT_STATUS_t p = FINGERPRINT_FAIL; // status checker
        TRACE_I("...ENROLL [Phase:-1]..Place a finger ......");
        // Loop until first fingerprint collection is successful
        start_time = esp_timer_get_time() / 1000; //  !< ms
        while ((p != FINGERPRINT_OK) && ((dummy_timer - start_time) < 6000))
        {
            p = GenImg(uart_channel_num, (user_data->recieved_buffer), 800);
            vTaskDelay(100 / portTICK_PERIOD_MS);
            dummy_timer = esp_timer_get_time() / 1000;
            if ((p != FINGERPRINT_OK) && (dummy_timer - start_time) > 4000)
            {
                // return from this task if image genereation failed
                TRACE_W("Image generation [1].... failed .. Retry by placing the finger again... after 3 seconds")
                return (0);
            }
        }
        // Check if the system is free!
        p = Wait_till_system_free(item, 1000);
        /*might need to add some code incase of system busy*/

        // generate character file from the original finger image in ImageBuffer and store the file in CharBuffer1
        p = Img2Tz(uart_channel_num, 1, (user_data->recieved_buffer), 1000);
        if (p)
        {
            TRACE_I(".........Remove finger .... wait 3 seconds");
            vTaskDelay(2000 / portTICK_PERIOD_MS);
        }
        else
        { // return from this task if image genereation failed
            TRACE_W("Character Image generation [1] failed .. try again... after 3 seconds")
            return (0);
        }
        // Check if the system is free!
        p = Wait_till_system_free(item, 1000);
        /*might need to add some code incase of system busy*/
        // 1. OK success!

        //----------------------- 2. Collecting Second fingerprint -----------------------------------------------
        p = FINGERPRINT_FAIL;
        TRACE_I("...ENROLL [Phase:-2]... Place same finger again......");
        // Loop until Second fingerprint collection is successful
        start_time = esp_timer_get_time() / 1000; //  !< ms
        while ((p != FINGERPRINT_OK) && ((dummy_timer - start_time) < 6000))
        {
            p = GenImg(uart_channel_num, (user_data->recieved_buffer), 800);
            vTaskDelay(100 / portTICK_PERIOD_MS);
            dummy_timer = esp_timer_get_time() / 1000;
            if ((p != FINGERPRINT_OK) && (dummy_timer - start_time) > 4000)
            {
                // return from this task if image genereation failed
                TRACE_W("Image generation [2].... failed .. Retry by placing the finger again... after 3 seconds")
                return (0);
            }
        }
        // Check if the system is free!
        p = Wait_till_system_free(item, 1000);
        /*might need to add some code incase of system busy*/

        // generate character file from the original finger image in ImageBuffer and store the file in CharBuffer2
        p = Img2Tz(uart_channel_num, 2, (user_data->recieved_buffer), 1000);
        if (p)
        {
            TRACE_I(".........Remove finger .... wait 3 seconds");
            vTaskDelay(2000 / portTICK_PERIOD_MS);
        }
        else
        { // return from this task if image genereation failed
            TRACE_W("Character Image generation [2] failed .. try again... after 3 seconds")
            return (0);
        }
        // Check if the system is free!
        p = Wait_till_system_free(item, 1000);
        /*might need to add some code incase of system busy*/
        // 2. OK success!

        //--------------------- 3. Generate the template after combining ChBuffer-1&2 , and store the result in both ChBuffer-1&2 ------------------
        TRACE_I("Creating model for user_ID: #%d..", custom_USER_ID);
        p = FINGERPRINT_FAIL;
        TRACE_I("... Create Model form ChrBuffer-1&2 [Phase:-3].........");
        // Loop until Second fingerprint collection is successful
        start_time = esp_timer_get_time() / 1000; //  !< ms
        while ((p != FINGERPRINT_OK) && ((dummy_timer - start_time) < 6000))
        {
            p = RegModel(uart_channel_num, (user_data->recieved_buffer), 800);
            vTaskDelay(100 / portTICK_PERIOD_MS);
            dummy_timer = esp_timer_get_time() / 1000;
            if ((p != FINGERPRINT_OK) && (dummy_timer - start_time) > 4000)
            {
                // return from this task if image genereation failed
                TRACE_W(" Template Generation failed .. Retry by placing the finger again... after 3 seconds")
                return (0);
            }
        }
        // Check if the system is free!
        p = Wait_till_system_free(item, 1000);
        /*might need to add some code incase of system busy*/
        // 3. OK converted!

        //-------------------- 4. Search for all the library and store only if no duplicates found ------------------------------------------------
        TRACE_I("Search model for user_ID: #%d inside the fingerprint library", custom_USER_ID);
        p = FINGERPRINT_FAIL;
        TRACE_I("... Search for duplicates (i.e. inside fingerprint Lib)  [Phase:-4]...");
        // Loop until Second fingerprint collection is successful
        start_time = esp_timer_get_time() / 1000; //  !< ms
        while ((p != FINGERPRINT_OK) && ((dummy_timer - start_time) < 4000))
        {
            p = Search(uart_channel_num,
                       1,                                 /*ChrBuffer = 1*/
                       FINGERPRINT_STARTING_USER_PAGE_ID, /*Page_id = starting_id*/
                       FINGERPRINT_MAX_CAPACITY_LIMIT,    /*Max_search_number = 500*/
                       &(user_data->user_id),             /*same_ID => */
                       &(user_data->confidence_level),    /*[value=!0]=> */
                       (user_data->recieved_buffer),
                       800);
            vTaskDelay(100 / portTICK_PERIOD_MS);
            dummy_timer = esp_timer_get_time() / 1000;
            // we use the error[0x0Ch] here, to understand that, matching ID is not present.
            if ((p != FINGERPRINT_OK) && (dummy_timer - start_time) > 1999)
            {
                // return from this task if image genereation failed
                TRACE_W(" Duplicate Template check failed/is empty...........");
                return (0);
            }
        }
        // Check if the system is free!
        p = Wait_till_system_free(item, 1000);
        /*might need to add some code incase of system busy*/
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
            TRACE_E("The Fingerprint is already present in:-[%d] ; Matched percent = [%d]", (user_data->user_id), (user_data->confidence_level))
            res_ID = (user_data->user_id); // returns
        }
        else /*confidence is less than threshold means its not found*/
        {
            TRACE_I("Storing user_ID: #%d .. in ", custom_USER_ID, custom_USER_ID);
            p = FINGERPRINT_FAIL;
            TRACE_I("...Store the unique fingerprint [Phase:-5]... Place same finger again......");
            // Loop until Second fingerprint collection is successful
            start_time = esp_timer_get_time() / 1000; //  !< ms
            while ((p != FINGERPRINT_OK) && ((dummy_timer - start_time) < 6000))
            {
                p = Store(uart_channel_num,
                          1,                                  /*ChrBuffer = 1*/
                          custom_USER_ID,                     /*store at Desired Page_id*/
                          (user_data->recieved_buffer), 800); /*need to change Page_id [default = 1]*/
                vTaskDelay(100 / portTICK_PERIOD_MS);
                dummy_timer = esp_timer_get_time() / 1000;
                if ((p != FINGERPRINT_OK) && (dummy_timer - start_time) > 4000)
                {
                    // return from this task if image genereation failed
                    TRACE_W(" Template Storage failed .. Retry by placing the finger again... after 3 seconds")
                    return (0);
                }
            }
        }
        // Check if the system is free!
        p = Wait_till_system_free(item, 1000);
        /*might need to add some code incase of system busy*/
        // 5. OK Stored!
        res_ID = custom_USER_ID;
    }
    // return the ID (where operation is taking place)
    return res_ID;
}

//--------------------------------- Function to configure fingerprint sensor ------------------------------------------------------
FINGERPRINT_STATUS_t fingerprint_config(l_ezlopi_item_t *item)
{
    FINGERPRINT_STATUS_t F_res = FINGERPRINT_FAIL;
    if (NULL != item)
    {
        server_packet_t *user_data = (server_packet_t *)item->user_arg;
        int uart_channel_num = item->interface.uart.channel;
        // waiting for extra 200ms
        vTaskDelay(200 / portTICK_PERIOD_MS);
        // check the system status and wait_till the system is free
        TRACE_E("Booting ..... Checking the system status");
        if (PortControl(uart_channel_num, UART_PORT_ON, (user_data->recieved_buffer), 1000))
        {
            F_res = Wait_till_system_free(item, 1000);
        }
        // First set the baudrate
        if (SetSysPara(uart_channel_num, FINGERPRINT_BAUDRATE_CONTROL, FINGERPRINT_BAUDRATE_9600, (user_data->recieved_buffer), 2000)) // timeout = 2000ms = 2sec
        {
            F_res = Wait_till_system_free(item, 1000);
        }
        // Second set the security level
        if (SetSysPara(uart_channel_num, FINGERPRINT_SECURITY_LEVEL, FINGERPRINT_SECURITY_4, (user_data->recieved_buffer), 2000)) // timeout = 2000ms = 2sec
        {
            F_res = Wait_till_system_free(item, 1000);
        }
        // Third set the max data package length
        if (SetSysPara(uart_channel_num, FINGERPRINT_DATA_PACKAGE_LENGTH, FINGERPRINT_DATA_LENGTH_64, (user_data->recieved_buffer), 2000)) // timeout = 2000ms = 2sec
        {
            F_res = Wait_till_system_free(item, 1000);
        }
    }
    return F_res;
}

#if 0
//---------------------------------- Function Filter-out occupied PAGEID ----------------------------------------------------------
/**
 * @brief ### This function creats a list of valid IDs and sends to the server
 */
 void List_of_Valid_IDs(l_ezlopi_item_t *item)
{
    if (NULL != item)
    {
        server_packet_t *user_data = (server_packet_t *)item->user_arg;
        // 1. update the status to latest
        Update_ID_status_list(item);
        // 2. To get variable sized array, Count no. of Valids (check ids:1~500)
        int valid_count = 0;
        for (uint16_t i = 1; i <= FINGERPRINT_MAX_CAPACITY_LIMIT; i++)
        { // First count the valid IDs
            if (true == (user_data->validity[i]))
            {
                valid_count++;
            }
        }
        // 3. Create only Array of size[valid_count] with Valid IDs
        uint16_t Temp_status[valid_count + 1] = {0};
        int temp_i = 1;
        for (uint16_t i = 1; i <= FINGERPRINT_MAX_CAPACITY_LIMIT; i++)
        { // Add the index value of valid IDs into 'Temp_status'
            if (true == (user_data->validity[i]))
            {
                if (temp_i > FINGERPRINT_MAX_CAPACITY_LIMIT)
                {
                    temp_i = 0;
                }
                else
                {
                    Temp_status[temp_i] = i;
                    temp_i++;
                }
            }
        }
        /**
         * You have to send this 'Temp_status' to get_cjson Action
         */
    }
}

/**
 * @brief #### This function mannually upload the image in Img_Buffer to upper computer.
 *
 * @param (int)uart_channel_num: The UART channel number
 * @param (uint8_t*)recieved_buffer: Holds the address to a uart_buffer with recieved and filtered message
 * @param (uint32_t)timeout: Timeout(N*1ms) for uart message polling
 *
 * @return succcess[>0] or failure[0]
 */
bool UpImage(int uart_channel_num, uint8_t *recieved_buffer, uint32_t timeout)
{
    // Reset the recieving buffer before new data is to be extracted.
    memset(recieved_buffer, 0, MAX_PACKET_LENGTH_VAL);
    // create the packet here
    FINGERPRINT_STATUS_t F_res = FINGERPRINT_FAIL;

    fingerprint_packet_t *txPacket = (fingerprint_packet_t *)malloc(sizeof(fingerprint_packet_t));
    if (txPacket)
    {
        txPacket->header_code[0] = FINGERPRINT_HEADER_MSB;
        txPacket->header_code[1] = FINGERPRINT_HEADER_LSB;
        txPacket->device_address[0] = txPacket->device_address[1] = txPacket->device_address[2] = txPacket->device_address[3] = FINGERPRINT_DEVICE_ADDR_BIT;

        //------------ Inst_code + Data_content  --------------------------------------------------------------------------------
        uint8_t Combined_data[] = {
            FINGERPRINT_UPLOAD_IMG, /*INS CODE [1Byte]*/
        };
        //------------ Fill the packet container and send it via uart  --------------------------------------------------------------------------------
        bool res = SEND_PACKET(uart_channel_num,                      /* UART CHANNEL NUMBER */
                               txPacket,                              /* Address of packet container */
                               FINGERPRINT_PID_COMMANDPACKET,         /* Packet Identifier CMD*/
                               (uint16_t)(sizeof(Combined_data) + 2), /* length <= combined_data + chk_sum */
                               Combined_data);                        /* Inst_code + Data_content*/
        //------------ Check of the appropriate responce  --------------------------------------------------------------------------------
        // If the tx is succesfull ; Read "recieved_buffer" that stores, the incoming message from the uart_ISR_upcall
        if (res)
        {
            F_res = __Response_function(recieved_buffer, timeout);
            if (FINGERPRINT_OK == F_res)
            {
                uint16_t Checksum = ((uint16_t)recieved_buffer[4] << 8) + ((uint16_t)recieved_buffer[5] & 0xFF); //(1~12)
                TRACE_I("UpImage =>Checksum [4]: %#x", Checksum);
            }
        }
        free(txPacket);
    }
    //----------------------------------------------------------------------------------------------------------------------
    return (bool)F_res;
}

/**
 * @brief #### This function mannually download the image from upper computer into Img_buffer.
 *
 * @param (int)uart_channel_num: The UART channel number
 * @param (uint8_t*)recieved_buffer: Holds the address to a uart_buffer with recieved and filtered message
 * @param (uint32_t)timeout: Timeout(N*1ms) for uart message polling
 *
 * @return succcess[>0] or failure[0]
 */
bool DownImage(int uart_channel_num, uint8_t *recieved_buffer, uint32_t timeout)
{
    // Reset the recieving buffer before new data is to be extracted.
    memset(recieved_buffer, 0, MAX_PACKET_LENGTH_VAL);
    // create the packet here
    FINGERPRINT_STATUS_t F_res = FINGERPRINT_FAIL;

    fingerprint_packet_t *txPacket = (fingerprint_packet_t *)malloc(sizeof(fingerprint_packet_t));
    if (txPacket)
    {
        txPacket->header_code[0] = FINGERPRINT_HEADER_MSB;
        txPacket->header_code[1] = FINGERPRINT_HEADER_LSB;
        txPacket->device_address[0] = txPacket->device_address[1] = txPacket->device_address[2] = txPacket->device_address[3] = FINGERPRINT_DEVICE_ADDR_BIT;

        //------------ Inst_code + Data_content  --------------------------------------------------------------------------------
        uint8_t Combined_data[] = {
            FINGERPRINT_DOWNLOAD_IMG, /*INS CODE [1Byte]*/
        };
        //------------ Fill the packet container and send it via uart  --------------------------------------------------------------------------------
        bool res = SEND_PACKET(uart_channel_num,                      /* UART CHANNEL NUMBER */
                               txPacket,                              /* Address of packet container */
                               FINGERPRINT_PID_COMMANDPACKET,         /* Packet Identifier CMD*/
                               (uint16_t)(sizeof(Combined_data) + 2), /* length <= combined_data + chk_sum */
                               Combined_data);                        /* Inst_code + Data_content*/
        //------------ Check of the appropriate responce  --------------------------------------------------------------------------------
        // If the tx is succesfull ; Read "recieved_buffer" that stores, the incoming message from the uart_ISR_upcall
        if (res)
        {
            F_res = __Response_function(recieved_buffer, timeout);
            if (FINGERPRINT_OK == F_res)
            {
                uint16_t Checksum = ((uint16_t)recieved_buffer[4] << 8) + ((uint16_t)recieved_buffer[5] & 0xFF); //(1~12)
                TRACE_I("DownImage =>Checksum [4]: %#x", Checksum);
            }
            // data_packet
        }
        free(txPacket);
    }
    //----------------------------------------------------------------------------------------------------------------------
    return (bool)F_res;
}

/**
 * @brief #### This function Sets Module handshaking password.
 *
 * @param (int)uart_channel_num: The UART channel number
 * @param (uint32_t)new_password: Value as a password
 * @param (uint8_t*)recieved_buffer: Holds the address to a uart_buffer with recieved and filtered message
 * @param (uint32_t)timeout: Timeout(N*1ms) for uart message polling
 *
 * @return succcess[>0] or failure[0]
 */
bool SetPwd(int uart_channel_num, uint32_t new_password, uint8_t *recieved_buffer, uint32_t timeout)
{
    // Reset the recieving buffer before new data is to be extracted.
    memset(recieved_buffer, 0, MAX_PACKET_LENGTH_VAL);
    // create the packet here
    FINGERPRINT_STATUS_t F_res = FINGERPRINT_FAIL;

    fingerprint_packet_t *txPacket = (fingerprint_packet_t *)malloc(sizeof(fingerprint_packet_t));
    if (txPacket)
    {
        txPacket->header_code[0] = FINGERPRINT_HEADER_MSB;
        txPacket->header_code[1] = FINGERPRINT_HEADER_LSB;
        txPacket->device_address[0] = txPacket->device_address[1] = txPacket->device_address[2] = txPacket->device_address[3] = FINGERPRINT_DEVICE_ADDR_BIT;

        //------------ Inst_code + Data_content  --------------------------------------------------------------------------------
        uint8_t Combined_data[] = {
            FINGERPRINT_SETPASSWORD,        /*INS CODE [1Byte]*/
            (uint8_t)(new_password >> 24),  /*MSB sent first*/
            (uint8_t)(new_password >> 16),  /*MSB sent first*/
            (uint8_t)(new_password >> 8),   /*MSB sent first*/
            (uint8_t)(new_password & 0xFF), /*LSB sent last*/
        };
        //------------ Fill the packet container and send it via uart  --------------------------------------------------------------------------------
        bool res = SEND_PACKET(uart_channel_num,                      /* UART CHANNEL NUMBER */
                               txPacket,                              /* Address of packet container */
                               FINGERPRINT_PID_COMMANDPACKET,         /* Packet Identifier CMD*/
                               (uint16_t)(sizeof(Combined_data) + 2), /* length <= combined_data + chk_sum */
                               Combined_data);                        /* Inst_code + Data_content*/
        //------------ Check of the appropriate responce  --------------------------------------------------------------------------------
        // If the tx is succesfull ; Read "recieved_buffer" that stores, the incoming message from the uart_ISR_upcall
        if (res)
        {
            F_res = __Response_function(recieved_buffer, timeout);
            if (FINGERPRINT_OK == F_res)
            {
                uint16_t Checksum = ((uint16_t)recieved_buffer[4] << 8) + ((uint16_t)recieved_buffer[5] & 0xFF); //(1~12)
                TRACE_I("SetPwd =>Checksum [4]: %#x", Checksum);
            }
        }
        free(txPacket);
    }
    //----------------------------------------------------------------------------------------------------------------------
    return (bool)F_res;
}

/**
 * @brief #### This function mannually upload the "character_file/template" from CharBuffer1/CharBuffer2 to upper computer.
 *
 * @param (int)uart_channel_num: The UART channel number
 * @param (uint8_t)CharBufferID: Character file buffer number [Chrbuff1 = 1h ; Chrbuff1 = 2h]
 * @param (uint8_t*)recieved_buffer: Holds the address to a uart_buffer with recieved and filtered message
 * @param (uint32_t)timeout: Timeout(N*1ms) for uart message polling
 *
 * @return succcess[>0] or failure[0]
 */
bool UpChar(int uart_channel_num, uint8_t CharBufferID, uint8_t *recieved_buffer, uint32_t timeout)
{
    // Reset the recieving buffer before new data is to be extracted.
    memset(recieved_buffer, 0, MAX_PACKET_LENGTH_VAL);
    // create the packet here
    FINGERPRINT_STATUS_t F_res = FINGERPRINT_FAIL;

    fingerprint_packet_t *txPacket = (fingerprint_packet_t *)malloc(sizeof(fingerprint_packet_t));
    if (txPacket)
    {
        txPacket->header_code[0] = FINGERPRINT_HEADER_MSB;
        txPacket->header_code[1] = FINGERPRINT_HEADER_LSB;
        txPacket->device_address[0] = txPacket->device_address[1] = txPacket->device_address[2] = txPacket->device_address[3] = FINGERPRINT_DEVICE_ADDR_BIT;

        //------------ Inst_code + Data_content  --------------------------------------------------------------------------------
        uint8_t Combined_data[] = {
            FINGERPRINT_UPLOAD_TEMP, /*INS CODE [1Byte]*/
            CharBufferID,            /*BufferId = CharBuf1 or CharBuf2*/
        };
        //------------ Fill the packet container and send it via uart  --------------------------------------------------------------------------------
        bool res = SEND_PACKET(uart_channel_num,                      /* UART CHANNEL NUMBER */
                               txPacket,                              /* Address of packet container */
                               FINGERPRINT_PID_COMMANDPACKET,         /* Packet Identifier CMD*/
                               (uint16_t)(sizeof(Combined_data) + 2), /* length <= combined_data + chk_sum */
                               Combined_data);                        /* Inst_code + Data_content*/
        //------------ Check of the appropriate responce  --------------------------------------------------------------------------------
        // If the tx is succesfull ; Read "recieved_buffer" that stores, the incoming message from the uart_ISR_upcall
        if (res)
        {
            F_res = __Response_function(recieved_buffer, timeout);
            if (FINGERPRINT_OK == F_res)
            {
                uint16_t Checksum = ((uint16_t)recieved_buffer[4] << 8) + ((uint16_t)recieved_buffer[5] & 0xFF); //(1~12)
                TRACE_I("UpChar =>Checksum [4]: %#x", Checksum);
            }
        }
        // data_packet
        free(txPacket);
    }
    //----------------------------------------------------------------------------------------------------------------------
    return (bool)F_res;
}

/**
 * @brief #### This function mannually download the "character_file/template" to CharBuffer1/CharBuffer2 from upper computer.
 *
 * @param (int)uart_channel_num: The UART channel number
 * @param (uint8_t)CharBufferID: Character file buffer number [Chrbuff1 = 1h ; Chrbuff1 = 2h]
 * @param (uint8_t*)recieved_buffer: Holds the address to a uart_buffer with recieved and filtered message
 * @param (uint32_t)timeout: Timeout(N*1ms) for uart message polling
 *
 * @return succcess[>0] or failure[0]
 */
bool DownChar(int uart_channel_num, uint8_t CharBufferID, uint8_t *recieved_buffer, uint32_t timeout)
{
    // Reset the recieving buffer before new data is to be extracted.
    memset(recieved_buffer, 0, MAX_PACKET_LENGTH_VAL);
    // create the packet here
    FINGERPRINT_STATUS_t F_res = FINGERPRINT_FAIL;

    fingerprint_packet_t *txPacket = (fingerprint_packet_t *)malloc(sizeof(fingerprint_packet_t));
    if (txPacket)
    {
        txPacket->header_code[0] = FINGERPRINT_HEADER_MSB;
        txPacket->header_code[1] = FINGERPRINT_HEADER_LSB;
        txPacket->device_address[0] = txPacket->device_address[1] = txPacket->device_address[2] = txPacket->device_address[3] = FINGERPRINT_DEVICE_ADDR_BIT;

        //------------ Inst_code + Data_content  --------------------------------------------------------------------------------
        uint8_t Combined_data[] = {
            FINGERPRINT_DOWNLOAD_TEMP, /*INS CODE [1Byte]*/
            CharBufferID,              /*BufferId = CharBuf1 or CharBuf2*/
        };
        //------------ Fill the packet container and send it via uart  --------------------------------------------------------------------------------
        bool res = SEND_PACKET(uart_channel_num,                      /* UART CHANNEL NUMBER */
                               txPacket,                              /* Address of packet container */
                               FINGERPRINT_PID_COMMANDPACKET,         /* Packet Identifier CMD*/
                               (uint16_t)(sizeof(Combined_data) + 2), /* length <= combined_data + chk_sum */
                               Combined_data);                        /* Inst_code + Data_content*/
        //------------ Check of the appropriate responce  --------------------------------------------------------------------------------
        // If the tx is succesfull ; Read "recieved_buffer" that stores, the incoming message from the uart_ISR_upcall
        if (res)
        {
            F_res = __Response_function(recieved_buffer, timeout);
            if (FINGERPRINT_OK == F_res)
            {
                uint16_t Checksum = ((uint16_t)recieved_buffer[4] << 8) + ((uint16_t)recieved_buffer[5] & 0xFF); //(1~12)
                TRACE_I("DownChar =>Checksum [4]: %#x", Checksum);
            }
        }
        // data_packet
        free(txPacket);
    }
    //----------------------------------------------------------------------------------------------------------------------
    return (bool)F_res;
}
#endif