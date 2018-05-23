#include "Microchip_23LC1024.h"

Microchip_23LC1024::Microchip_23LC1024( byte cs, byte hold )
{
    _cs = cs;
    _hold = hold;
    _mode = BYTE_MODE;
    
    pinMode(_cs, OUTPUT);
    pinMode(_hold, OUTPUT);
    pinMode(10, OUTPUT);
    
    deselect();
    SPI.begin();

}

void Microchip_23LC1024::write_byte( unsigned long address, byte b )
{
    byte *ptr;
    int i;
    ptr = (byte *) &address;
    if(_mode != BYTE_MODE) setMode(BYTE_MODE);

    select();
    SPI.transfer(WRITE);
    for(i = 2; i >= 0; i--) SPI.transfer(ptr[i]);
    SPI.transfer(b);
    
    deselect();
}

byte Microchip_23LC1024::read_byte ( unsigned long address )
{
    byte b, *ptr;
    int i;
    ptr = (byte *) &address;
    if(_mode != BYTE_MODE) setMode(BYTE_MODE);
    
    select();
    SPI.transfer(READ);
    
    for(i = 2; i >= 0; i--) SPI.transfer(ptr[i]);
    b = SPI.transfer(0x00);
    deselect();
    
    return b;
}

void Microchip_23LC1024::write_page( unsigned long page_number, byte *buffer )
{
    int i;
    unsigned long address = page_number * PAGE_SIZE;
    byte *ptr = (byte *) &address;
    
    if(_mode != PAGE_MODE) setMode(PAGE_MODE);
    
    select();
    SPI.transfer(WRITE);
    for(i = 2; i >= 0; i--) SPI.transfer(ptr[i]);
    
    for(i = 0; i < PAGE_SIZE; i++){
        SPI.transfer(buffer[i]);
    }
    
    deselect();
}

void Microchip_23LC1024::read_page ( unsigned long page_number, byte *buffer )
{
    unsigned long i;
    unsigned long address = page_number * PAGE_SIZE;
    
    //work around..
    for(i = 0; i < PAGE_SIZE; i++){
        //byte b = SPI.transfer(0x00);
        buffer[i] = read_byte((unsigned long) address + i);
    }
}

void Microchip_23LC1024::setMode(byte mode)
{
    _mode = mode;

    select();
    SPI.transfer(WRITE_MODE_REGISTER);
    SPI.transfer(mode);
    deselect();
}

byte Microchip_23LC1024::getMode()
{
    byte b;
    
    select();
    SPI.transfer(READ_MODE_REGISTER);
    b = SPI.transfer(0x00);
    
    deselect();
    return b;
}

void Microchip_23LC1024::select()
{
    SPI.setDataMode(SPI_MODE0);
    SPI.setBitOrder(MSBFIRST);
    SPI.setClockDivider(SPI_CLOCK_DIV8);
    
    digitalWrite(_cs, LOW);
    digitalWrite(_hold, HIGH);
}

void Microchip_23LC1024::deselect()
{
    digitalWrite(_hold, HIGH);
    digitalWrite(_cs, HIGH);
}
