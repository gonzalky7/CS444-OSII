#include <Arduino.h>
#include <SPI.h>

#pragma once

#define READ                0x03
#define WRITE               0x02
#define READ_MODE_REGISTER  0x05
#define WRITE_MODE_REGISTER 0x01

#define BYTE_MODE           0x00
#define PAGE_MODE           0x80

#define MAX_ADDRESS         0x1FFFF
#define MAX_PAGE            4096
#define PAGE_SIZE           32


class Microchip_23LC1024 {
    public:
        Microchip_23LC1024( byte cs, byte hold );

        void write_byte( unsigned long address, byte b );
        byte read_byte ( unsigned long address );

        void write_page( unsigned long page_number, byte *buffer );
        void read_page ( unsigned long page_number, byte *buffer );

        void setMode(byte mode);
        byte getMode();

    private:
        byte _cs;
        byte _hold;
        byte _mode;

        void select();
        void deselect();
};
