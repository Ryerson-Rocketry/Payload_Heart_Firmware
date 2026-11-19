#define RFM95_RST  = 1    // TX0 = GPIO1, shared with the USB serial TX line (see radioReset())
#define RFM95_CS = 4
#define SD_CS 5
#define SCK 18
#define MISO 19
#define RFM95_INT = 22
#define MOSI 23
#define SD_CD 26 //card detector (HIGH = Card Inserted)
//#define SD_LOG_FILE "/launch_data_XXXX.csv" //where XXXX 
char SD_LOG_FILE[40];
bool initializeSD();
bool logPacketToSD(uint32_t packetNumber,
                   long elapsedTime,
                   const char* aprsPacket);
uint16_t calculateCRC16(const uint8_t* data, size_t length)
char* createAPRSPacket(float latitude, float longitude,
                        float altitude,
                        //ANY OTHER DATA HERE (GOES IN COMMENT SECTION OF APRS)
                        float battery,
                        uint32_t packetNumber)
void radioReset();
bool radioSetup();
void setup();
void loop();