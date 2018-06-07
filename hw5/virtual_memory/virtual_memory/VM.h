//
//  VM.h
//  virtual_memory
//
//  Created by Kyleen Gonzalez on 5/15/18.
//  Copyright © 2018 Kyleen Gonzalez. All rights reserved.
//

#ifndef VM_h
#define VM_h
#define SRAM_SIZE 131072
#define PG_SIZE 32
//#define SRAM 5
//#define HOLD 7
#define VERBOSE 0
#define TABLE_SIZE 16

class VM
{
    private:
        unsigned char pageTable [TABLE_SIZE];
        unsigned char physicalMemory[TABLE_SIZE * PG_SIZE];
    
    //instantiate the sram object on pins 5 and 7
    //Microchip_23LC1024 sram( SRAM, HOLD);

    public:
        VM();
        void& operator[] (const int index);
        void print();
};
#endif /* VM_h */
