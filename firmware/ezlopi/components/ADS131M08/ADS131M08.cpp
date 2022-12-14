#include "ADS131M08.h"

spi_device_handle_t spi;
char * rx_buffer;
const int sampleTime = 50; 

#define MISO_PIN              19
#define MOSI_PIN              23
#define SCK_PIN               18
#define CS_PIN                27

#define VSPI_MISO             MISO_PIN
#define VSPI_MOSI             MOSI_PIN
#define VSPI_SCLK             SCK_PIN
#define VSPI_SS               CS_PIN

#define DRDY_PIN              -1//21
#define XTAL_PIN              -1//22
#define RESET                -1 //17

int micOut[8];
int32_t channelArr[8];
void findPTPAmp(ADS131M08* adc);

ADS131M08::ADS131M08(gpio_num_t mosi, gpio_num_t miso, gpio_num_t sck,int cs, int xtal, int drdy, int clk) {
    this->mosi = mosi;
    this->miso = miso;
    this->sck = sck;
    CS = cs; XTAL = xtal; DRDY = drdy; //You don't have to use DRDY, can also read off the ADS131_STATUS register.
    SpiClk = clk;
    rx_buffer = (char*)heap_caps_malloc(4, MALLOC_CAP_DMA);
}

void ADS131M08::init(int clkin) {

    
    ESP_LOGI(TAG,"Setting pin configuration");
        
    gpio_set_direction((gpio_num_t)CS, GPIO_MODE_OUTPUT); 
    csHigh();
    // pinMode(DRDY, INPUT_PULLUP);
    
    spi_init();

    ESP_LOGI(TAG,"Setting oscillator");

    // ledcSetup(2, clkin, 2);
    // ledcAttachPin(XTAL, 2); //Simulate 8.192Mhz crystal with PWM. This needs to be started as soon as possible after powering on
    // ledcWrite(2,2);

    ESP_LOGI(TAG,"SPI Ready...");

}

void ADS131M08::readChannels(int8_t * channelArrPtr, int8_t channelArrLen, int32_t * outputArrPtr) {
    
    uint32_t rawDataArr[10];

    // Get data
    spiCommFrame(&rawDataArr[0]);
    
    // Save the decoded data for each of the channels
    for (int8_t i = 0; i<channelArrLen; i++) {
        *outputArrPtr = twoCompDeco(rawDataArr[*channelArrPtr+1]);
        outputArrPtr++;
        channelArrPtr++;
    }
    
}

void ADS131M08::readAllChannels(int32_t inputArr[8]) {
    uint32_t rawDataArr[10];
    int8_t channelArrPtr = 0;
    int8_t channelArrLen = 8;

    // Get data
    spiCommFrame(&rawDataArr[0]);
    // Save the decoded data for each of the channels
    for (int8_t i = 0; i<8; i++) {
        inputArr[i] = twoCompDeco(rawDataArr[channelArrPtr+1]);
        channelArrPtr++;
    }
}

int32_t ADS131M08::readChannelSingle(int8_t channel) {
    /* Returns raw value from a single channel
        channel input from 0-7
    */
    
    int32_t outputArr[1];
    int8_t channelArr[1] = {channel};

    readChannels(&channelArr[0], 1, &outputArr[0]);

    return outputArr[0];
}

bool ADS131M08::globalChop(bool enabled, uint8_t log2delay) {
    /* Function to configure global chop mode for the ADS131M04.

        INPUTS:
        enabled - Whether to enable global-chop mode.
        log2delay   - Base 2 log of the desired delay in modulator clocks periods
        before measurment begins
        Possible values are between and including 1 and 16, to give delays
        between 2 and 65536 clock periods respectively
        For more information, refer to the datasheet.

        Returns true if settings were written succesfully.
    */

    uint8_t delayRegData = log2delay - 1;

    // Get current settings for current detect mode from the CFG register
    uint16_t currentDetSett = (readReg(ADS131_CFG) << 8) >>8;
    
    uint16_t newRegData = (delayRegData << 12) + (enabled << 8) + currentDetSett;

    return writeReg(ADS131_CFG, newRegData);
}

bool ADS131M08::writeReg(uint8_t reg, uint16_t data) {
    /* Writes the content of data to the register reg
        Returns true if successful
    */
    
    uint8_t commandPref = 0x06;

    // Make command word using syntax found in data sheet
    uint16_t commandWord = (commandPref<<12) + (reg<<7);

    csLow();

    spiTransferWord(commandWord);
    
    spiTransferWord(data);

    // Send 4 empty words
 
    for (uint8_t i=0; i<4; i++) {
        spiTransferWord();
    }

    csHigh();

    // Get response
    uint32_t responseArr[10];
    spiCommFrame(&responseArr[0]);

    if ( ( (0x04<<12) + (reg<<7) ) == responseArr[0]) {
        return true;
    } else {
        return false;
    }
}

uint16_t ADS131M08::readReg(uint8_t reg) {
    /* Reads the content of single register found at address reg
        Returns register value
    */
    
    uint8_t commandPref = 0x0A;

    // Make command word using syntax found in data sheet
    uint16_t commandWord = (commandPref << 12) + (reg << 7);

    uint32_t responseArr[10];
    // Use first frame to send command
    spiCommFrame(&responseArr[0], commandWord);

    // Read response
    spiCommFrame(&responseArr[0]);

    return responseArr[0] >> 16;
}

uint32_t ADS131M08::spiTransferWord(uint16_t inputData) {
    /* Transfer a 24 bit word
        Data returned is MSB aligned
    */ 
    uint8_t addr[3]= {(uint8_t)(inputData>>8),(uint8_t)inputData,0x00};
    // ESP_LOG_BUFFER_HEX("Checkkkkkkkkkkk", addr, 3 );

    spi_transaction_t transaction;
    memset(&transaction, 0, sizeof(transaction));
    transaction.cmd = 0;
    transaction.length = 3*8;
    transaction.tx_buffer = addr;
    transaction.rx_buffer = rx_buffer;
    assert(spi_device_polling_transmit(spi, &transaction) == ESP_OK);
    // ESP_LOG_BUFFER_HEX("readSingleRegister:", rx_buffer, 3 );

    uint32_t data=0;
    data = ((*rx_buffer) << 24) | ((*(rx_buffer+1)) << 16) | ((*(rx_buffer+2)) << 8);
    return data;
}

void ADS131M08::spiCommFrame(uint32_t * outPtr, uint16_t command) {
    // Saves all the data of a communication frame to an array with pointer outPtr

    csLow();

    // Send the command in the first word
    *outPtr = spiTransferWord(command);

    // For the next 8 words, just read the data
    for (uint8_t i=1; i < 9; i++) {
        outPtr++;
        *outPtr = spiTransferWord() >> 8;
    }

    // Save CRC bits
    outPtr++;
    *outPtr = spiTransferWord();

    csHigh();
}

int32_t ADS131M08::twoCompDeco(uint32_t data) {
    // Take the two's complement of the data

    data <<= 8;
    int32_t dataInt = (int)data;

    return dataInt/pow(2,8);
}

bool ADS131M08::setGain(int gain) { // apply gain to all channels (1 to 128, base 2 (1,2,4,8,16,32,64,128))
    uint16_t writegain = 0;
    if(gain == 1 ) {
        writegain = 0b0000000000000000;
    }  
    else if (gain == 2) {
        writegain = 0b0001000100010001;
    }
    else if (gain == 4) {
        writegain = 0b0010001000100010;
    }
    else if (gain == 8) { 
        writegain = 0b0011001100110011;
    }
    else if (gain == 16) { 
        writegain = 0b0100010001000100;
    }
    else if (gain == 32) {
        writegain = 0b0101010101010101;
    }
    else if (gain == 64) {
        writegain = 0b0110011001100110;
    }
    else if (gain == 128) {
        writegain = 0b0111011101110111;
    }
    else {
        return false;
    }
    writeReg(ADS131_GAIN1, writegain);
    writeReg(ADS131_GAIN2, writegain);

    return true;
}

void ADS131M08::csLow(void)
{
	gpio_set_level((gpio_num_t)CS, 0);
}

void ADS131M08::csHigh(void)
{
	gpio_set_level((gpio_num_t)CS, 1);
}

void ADS131M08::spi_init() {
	spi_bus_config_t buscfg = {
		.mosi_io_num= mosi,
		.miso_io_num= miso,		
		.sclk_io_num= sck,
		.quadwp_io_num=-1,
		.quadhd_io_num=-1,
        .max_transfer_sz = 32,
	};
	spi_device_interface_config_t devcfg = {
		.command_bits=0, // 8-bit command length
		.address_bits=0, // no address
		.mode=1, // SPI MODE 1
		.clock_speed_hz=8192000, 
		.spics_io_num=-1,
		.queue_size=20,
		//.input_delay_ns = 25 // minimum of td(SCCS) = 25ns
	};
	ESP_ERROR_CHECK(spi_bus_initialize(VSPI_HOST, &buscfg, 0/*dma_chan - 0 no DMA*/));
	ESP_ERROR_CHECK(spi_bus_add_device(VSPI_HOST, &devcfg, &spi));
}

// Find the Peak-to-Peak Amplitude Function
void findPTPAmp(ADS131M08* adc){
// Time variables to find the peak-to-peak amplitude
   unsigned long startTime= esp_timer_get_time()/1000;  // Start of sample window
   unsigned int PTPAmp[8] = {0}; 

// Signal variables to find the peak-to-peak amplitude
   int maxAmp[8];
   std::fill_n(maxAmp, 8, 200000);
   int minAmp[8] ;
   std::fill_n(minAmp, 8, -200000);

// Find the max and min of the mic output within the 50 ms timeframe
   while((esp_timer_get_time()/1000) - startTime < sampleTime) 
   {
    adc->readAllChannels(channelArr);
    for (uint8_t i = 0; i < 8; i++)
    {
      micOut[i] = channelArr[i];
      if( micOut[i] < 1023) //prevent erroneous readings
      {
        if (micOut > maxAmp)
        {
          maxAmp[i] = micOut[i]; //save only the max reading
        }
        else if (micOut < minAmp)
        {
          minAmp[i] = micOut[i]; //save only the min reading
        }
      }
    }     
   }

//   for (uint8_t i = 0; i < 8; i++){
//   PTPAmp[i] = maxAmp[i] - minAmp[i]; // (max amp) - (min amp) = peak-to-peak amplitude
//   }
// //   double micOut_Volts = (PTPAmp * 3.3) / 4096; // Convert ADC into voltage

//   //Uncomment this line for help debugging (be sure to also comment out the VUMeter function)
//   //Serial.println(PTPAmp); 

//   //Return the PTP amplitude to use in the soundLevel function. 
//   // You can also return the micOut_Volts if you prefer to use the voltage level.
//   for (uint8_t i = 0; i < 8; i++){
//     channelArr[i] = PTPAmp[i];
//   }   
}
ADS131M08  adc((gpio_num_t)VSPI_MOSI,
            (gpio_num_t)VSPI_MISO,
            (gpio_num_t)VSPI_SCLK,
            VSPI_SS,
            XTAL_PIN,
            DRDY_PIN,
            8192000);
void ADS131_init(void)
{
  
    adc.init();
	adc.writeReg(ADS131_CLOCK,0b1111111100011010); //Clock register (page 55 in datasheet)
    adc.setGain(32);

	uint16_t dataR;
    dataR = adc.readReg(ADS131_ID);
	ESP_LOGI("Chip ID "," %x \n",dataR);

	dataR = adc.readReg(ADS131_CLOCK);
	ESP_LOGI("Clock "," %x \n",dataR);

  dataR = 0;
  dataR = adc.readReg(ADS131_GAIN1);
	ESP_LOGI("Gain 1 "," %x \n",dataR);

  dataR = 0;
  dataR = adc.readReg(ADS131_GAIN2);
	ESP_LOGI("Gain 2 "," %x \n",dataR);
    
}

bool ADS131_value(void)
{
    findPTPAmp(&adc);
    printf("Value %d,%d,%d,%d,%d,%d,%d,%d\r\n", 
            channelArr[0], 
            channelArr[1], 
            channelArr[2],
            channelArr[3],
            channelArr[4],
            channelArr[5],
            channelArr[6],
            channelArr[7]
            );
    if(channelArr[1] > 50)
        return true;
    else
        return false;
}