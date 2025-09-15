#include "FINS.h"

FINS::FINS(IPAddress local_ip, IPAddress plc_ip, uint16_t plc_port, uint16_t local_port) {
    this->local_ip = local_ip;
    this->plc_ip = plc_ip;
    this->plc_port = plc_port;
    this->local_port = local_port;
    this->sid_counter = 0;
    this->timeout_ms = FINS_DEFAULT_TIMEOUT;
}

bool FINS::init() {
    if (!udp.begin(local_port)) {
        //Serial.println("FINS: Failed to start UDP");
        return false;
    }
    
    //Serial.println("FINS: Initialized successfully");
    //Serial.print("FINS: Local IP: "); Serial.print(local_ip);
    //Serial.print(", Port: "); Serial.println(local_port);
    //Serial.print("FINS: PLC IP: "); Serial.print(plc_ip);
    //Serial.print(", Port: "); Serial.println(plc_port);
    
    return true;
}

void FINS::setTimeout(uint32_t timeout_ms) {
    this->timeout_ms = timeout_ms;
}

void FINS::end() {
    udp.stop();
}

void FINS::createHeader(uint8_t* buffer, uint8_t sid, bool response) {
    buffer[0] = response ? 0x80 : 0x81;  // ICF
    buffer[1] = 0x00;                     // RSV
    buffer[2] = 0x02;                     // GCT
    buffer[3] = 0x00;                     // DNA
    buffer[4] = 0x00;                     // DA1
    buffer[5] = 0x00;                     // DA2
    buffer[6] = 0x00;                     // SNA
    buffer[7] = 0x22;                     // SA1
    buffer[8] = 0x00;                     // SA2
    buffer[9] = sid;                      // SID
}

int FINS::buildReadCommand(uint8_t* buffer, uint8_t memory_area, uint16_t start_addr, uint16_t count) {
    uint8_t sid = getNextSID();
    int pos = 0;
    
    createHeader(buffer, sid);
    pos += FINS_HEADER_SIZE;
    
    buffer[pos++] = 0x01;  // MRC
    buffer[pos++] = 0x01;  // SRC
    buffer[pos++] = memory_area;
    buffer[pos++] = (start_addr >> 8);
    buffer[pos++] = (start_addr & 0xFF);
    buffer[pos++] = 0x00;
    buffer[pos++] = (count >> 8);
    buffer[pos++] = (count & 0xFF);
    
    return pos;
}

int FINS::buildWriteCommand(uint8_t* buffer, uint8_t memory_area, uint16_t start_addr, uint16_t* data, uint16_t count) {
    uint8_t sid = getNextSID();
    int pos = 0;
    
    createHeader(buffer, sid);
    pos += FINS_HEADER_SIZE;
    
    buffer[pos++] = 0x01;  // MRC
    buffer[pos++] = 0x02;  // SRC
    buffer[pos++] = memory_area;
    buffer[pos++] = (start_addr >> 8);
    buffer[pos++] = (start_addr & 0xFF);
    buffer[pos++] = 0x00;
    buffer[pos++] = (count >> 8);
    buffer[pos++] = (count & 0xFF);
    
    for (int i = 0; i < count; i++) {
        buffer[pos++] = (data[i] >> 8);
        buffer[pos++] = (data[i] & 0xFF);
    }
    
    return pos;
}

bool FINS::sendCommand(uint8_t* cmd_buffer, int cmd_len) {
    udp.beginPacket(plc_ip, plc_port);
    int written = udp.write(cmd_buffer, cmd_len);
    bool success = udp.endPacket();
    
    if (!success || written != cmd_len) {
        //Serial.println("FINS: Failed to send command");
        return false;
    }
    return true;
}

int FINS::receiveResponse() {
    unsigned long start_time = millis();
    
    while (millis() - start_time < timeout_ms) {
        int packet_size = udp.parsePacket();
        if (packet_size > 0) {
            int len = udp.read(rx_buffer, FINS_MAX_RESPONSE_SIZE);
            return len;
        }
        yield();
        delay(1);
    }
    
    //Serial.println("FINS: Timeout waiting for response");
    return -1;
}

bool FINS::processResponse(int response_len, uint16_t* data) {
    if (response_len < 14) {
        //Serial.println("FINS: Response too short");
        return false;
    }
    
    uint8_t mres = rx_buffer[12];
    uint8_t sres = rx_buffer[13];
    
    if (mres != 0x00 || sres != 0x00) {
        //Serial.print("FINS: Error response - MRES: 0x");
        //Serial.print(mres, HEX);
        //Serial.print(", SRES: 0x");
        //Serial.println(sres, HEX);
        return false;
    }
    
    if (data != nullptr) {
        int data_bytes = response_len - 14;
        int word_count = data_bytes / 2;
        
        for (int i = 0; i < word_count; i++) {
            data[i] = (rx_buffer[14 + i * 2] << 8) | rx_buffer[14 + i * 2 + 1];
        }
    }
    
    return true;
}

uint8_t FINS::getNextSID() {
    return ++sid_counter;
}

// DM (Data Memory) operations
int FINS::readDM(uint16_t address) {
    uint16_t data;
    if (readDM(address, 1, &data)) {
        return data;
    }
    return -1;
}

bool FINS::readDM(uint16_t start_address, uint16_t count, uint16_t* data) {
    return readMemory(FINS_MEMORY_AREA_DATA, start_address, count, data);
}

bool FINS::writeDM(uint16_t address, uint16_t value) {
    return writeDM(address, &value, 1);
}

bool FINS::writeDM(uint16_t start_address, uint16_t* data, uint16_t count) {
    return writeMemory(FINS_MEMORY_AREA_DATA, start_address, data, count);
}

// WR (Work Memory) operations
int FINS::readWR(uint16_t address) {
    uint16_t data;
    if (readWR(address, 1, &data)) {
        return data;
    }
    return -1;
}

bool FINS::readWR(uint16_t start_address, uint16_t count, uint16_t* data) {
    return readMemory(FINS_MEMORY_AREA_WORK, start_address, count, data);
}

bool FINS::writeWR(uint16_t address, uint16_t value) {
    return writeWR(address, &value, 1);
}

bool FINS::writeWR(uint16_t start_address, uint16_t* data, uint16_t count) {
    return writeMemory(FINS_MEMORY_AREA_WORK, start_address, data, count);
}

// Generic memory operations
bool FINS::readMemory(uint8_t memory_area, uint16_t start_address, uint16_t count, uint16_t* data) {
    int cmd_len = buildReadCommand(tx_buffer, memory_area, start_address, count);
    
    if (!sendCommand(tx_buffer, cmd_len)) {
        return false;
    }
    
    int response_len = receiveResponse();
    if (response_len < 0) {
        return false;
    }
    
    return processResponse(response_len, data);
}

bool FINS::writeMemory(uint8_t memory_area, uint16_t start_address, uint16_t* data, uint16_t count) {
    int cmd_len = buildWriteCommand(tx_buffer, memory_area, start_address, data, count);
    
    if (!sendCommand(tx_buffer, cmd_len)) {
        return false;
    }
    
    int response_len = receiveResponse();
    if (response_len < 0) {
        return false;
    }
    
    return processResponse(response_len);
}