#ifndef GXHTC3_H
#define GXHTC3_H

#include <stdint.h>
#include <stdbool.h>

#define GXHTC3_CRC_POLY 0x31

#define GXHTC3_I2C_SLAVE_ADDRESS 0X60
#define GXHTC3_I2C_TIMEOUT 100

#define GXHTC3_I2C_RAW_DATA_LEN 6
#define GXHTC3_I2C_REG_ID_READ_LEN 3

#define GXHTC3_I2C_CMD_SLEEP 0XB098
#define GXHTC3_I2C_CMD_WAKEUP 0X3517
#define GXHTC3_I2C_CMD_SOFT_RESET 0x805D
#define GXHTC3_I2C_CMD_READ_REG_ID 0xEFC8
#define GXHTC3_I2C_CMD_READ_NM_CSEN_TF 0x7CA2
#define GXHTC3_I2C_CMD_READ_NM_CSEN_HF 0x5C24
#define GXHTC3_I2C_CMD_READ_NM_CSDS_TF 0x7866
#define GXHTC3_I2C_CMD_READ_NM_CSDS_HF 0x58E0
#define GXHTC3_I2C_CMD_READ_LP_CSEN_TF 0x6458
#define GXHTC3_I2C_CMD_READ_LP_CSEN_HF 0x44DE
#define GXHTC3_I2C_CMD_READ_LP_CSDS_TF 0x609C
#define GXHTC3_I2C_CMD_READ_LP_CSDS_HF 0x401A

typedef struct s_gxhtc3_id
{
    uint16_t id;
    bool status;

} s_gxhtc3_id_t;

typedef struct s_gxhtc3_sensor_handler
{
    uint8_t i2c_ch_num;
    uint8_t i2c_slave_addr;
    s_gxhtc3_id_t id;
    uint8_t raw_data_reg[GXHTC3_I2C_RAW_DATA_LEN];
    bool read_status;
    float reading_temp_c;
    float reading_rh;

} s_gxhtc3_sensor_handler_t;

s_gxhtc3_sensor_handler_t *GXHTC3_init(int32_t i2c_ch_num, uint8_t i2c_slave_addr);
bool GXHTC3_read_sensor(s_gxhtc3_sensor_handler_t *handler);

#endif /* GXHTC3_H */