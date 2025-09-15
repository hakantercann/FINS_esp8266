#ifndef FINS_H
#define FINS_H

#include <Arduino.h>
#include <ESP8266WiFi.h>
#include <WiFiUdp.h>

// FINS Protocol Constants
#define FINS_MEMORY_AREA_DATA   0x82  // DM area
#define FINS_MEMORY_AREA_WORK   0xB1  // WR area
#define FINS_MEMORY_AREA_CIO    0x30  // CIO area
#define FINS_HEADER_SIZE        10
#define FINS_MAX_RESPONSE_SIZE  2048
#define FINS_DEFAULT_PORT       9600
#define FINS_DEFAULT_TIMEOUT    2000

class FINS {
private:
    WiFiUDP udp;
    IPAddress local_ip;
    IPAddress plc_ip;
    uint16_t plc_port;
    uint16_t local_port;
    uint8_t sid_counter;
    uint32_t timeout_ms;
    
    // Buffer for commands and responses
    uint8_t tx_buffer[512];
    uint8_t rx_buffer[FINS_MAX_RESPONSE_SIZE];
    
    void createHeader(uint8_t* buffer, uint8_t sid, bool response = true);
    int buildReadCommand(uint8_t* buffer, uint8_t memory_area, uint16_t start_addr, uint16_t count);
    int buildWriteCommand(uint8_t* buffer, uint8_t memory_area, uint16_t start_addr, uint16_t* data, uint16_t count);
    bool sendCommand(uint8_t* cmd_buffer, int cmd_len);
    int receiveResponse();
    bool processResponse(int response_len, uint16_t* data = nullptr);
    uint8_t getNextSID();

public:
    // Constructor
    FINS(IPAddress local_ip, IPAddress plc_ip, uint16_t plc_port = FINS_DEFAULT_PORT, uint16_t local_port = 9600);
    
    // Initialization and cleanup
    bool init();
    void setTimeout(uint32_t timeout_ms);
    void end();
    
    // DM (Data Memory) operations
    int readDM(uint16_t address);
    bool readDM(uint16_t start_address, uint16_t count, uint16_t* data);
    bool writeDM(uint16_t address, uint16_t value);
    bool writeDM(uint16_t start_address, uint16_t* data, uint16_t count);
    
    // WR (Work Memory) operations
    int readWR(uint16_t address);
    bool readWR(uint16_t start_address, uint16_t count, uint16_t* data);
    bool writeWR(uint16_t address, uint16_t value);
    bool writeWR(uint16_t start_address, uint16_t* data, uint16_t count);
    
    // Generic memory operations
    bool readMemory(uint8_t memory_area, uint16_t start_address, uint16_t count, uint16_t* data);
    bool writeMemory(uint8_t memory_area, uint16_t start_address, uint16_t* data, uint16_t count);
};

#endif