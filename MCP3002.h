#ifndef _MCP3002_H
#define _MCP3002_H

#include <Arduino.h>
#include <SPI.h>

#define MCP3002_START_BIT    0x40
#define MCP3002_SINGLE_ENDED 0x20
#define MCP3002_DIFFERENTIAL 0x00
#define MCP3002_MSB_FIRST    0x08
#define MCP3002_LSB_FIRST    0x00

class MCP3002 {
    public:
        MCP3002(uint8_t cs) : _cs(cs) {}
        void begin();
        uint16_t analogRead(uint8_t pin);
        int16_t analogReadDif(void);

    private:
        uint8_t _cs;
};
#endif
