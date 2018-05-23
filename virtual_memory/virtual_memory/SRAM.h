//
//  SRAM.h
//  virtual_memory
//
//  Created by Kyleen Gonzalez on 5/17/18.
//  Copyright © 2018 Kyleen Gonzalez. All rights reserved.
//

#ifndef SRAM_h
#define SRAM_h
#define READ                0x03
#define WRITE               0x02
#define READ_MODE_REGISTER  0x05
#define WRITE_MODE_REGISTER 0x01

#define BYTE_MODE           0x00
#define PAGE_MODE           0x80

#define MAX_ADDRESS         0x1FFFF
#define MAX_PAGE            4096
#define PAGE_SIZE           32
#define SIZE                131072

#include <stdio.h>


class SRAM {
public:
    SRAM();
    void write_byte( unsigned long address, unsigned char b);
    unsigned char read_byte ( unsigned long address );
    
    void read_page(unsigned long page_number, unsigned char *buffer);
    void write_page(unsigned long page_number, unsigned char *buffer);
    
    void print();
    
    unsigned char sram_memory[SIZE];
private:
    
};

#endif /* SRAM_h */
