#include "MCP3002.h"

void MCP3002::begin() {
    pinMode(_cs, OUTPUT);
    digitalWrite(_cs, HIGH);
    SPI.begin();
}

uint16_t MCP3002::analogRead(uint8_t pin) {
    uint8_t b1, b2;
    uint8_t send;

    send = (MCP3002_START_BIT | MCP3002_MSB_FIRST);
    send |= MCP3002_SINGLE_ENDED | ((pin & 0x01) << 4);

    digitalWrite(_cs, LOW);
    b1 = SPI.transfer(send);
    b2 = SPI.transfer(0x00);
    digitalWrite(_cs, HIGH);

    return ((b1 << 8) | b2) & 0x03ff;
}

int16_t MCP3002::analogReadDif(void) {
    uint16_t diff;
    uint8_t b1, b2;
    uint8_t send;

    send = MCP3002_START_BIT | MCP3002_MSB_FIRST;
    send |= MCP3002_DIFFERENTIAL;

    digitalWrite(_cs, LOW);
    b1 = SPI.transfer(send);
    b2 = SPI.transfer(0x00);
    digitalWrite(_cs, HIGH);

    diff = ((b1 << 8) | b2) & 0x03ff;
    if (diff > 0) {
        return diff;
    }

    send = MCP3002_START_BIT | MCP3002_MSB_FIRST;
    send |= MCP3002_DIFFERENTIAL | 0x10;

    digitalWrite(_cs, LOW);
    b1 = SPI.transfer(send);
    b2 = SPI.transfer(0x00);
    digitalWrite(_cs, HIGH);
    diff = ((b1 << 8) | b2) & 0x03ff;
    return -diff;
}
