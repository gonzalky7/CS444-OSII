//
//  VM.cpp
//  virtual_memory
//
//  Created by Kyleen Gonzalez on 5/17/18.
//  Copyright © 2018 Kyleen Gonzalez. All rights reserved.
//
#include "VM.h"
#include "SRAM.h"
#include <stdio.h>
#include <iostream>

using namespace std;

VM::VM() {
    for (int i = 0; i < TABLE_SIZE; i++) {
        pageTable[i] = '&';//filling with invalid
    }
}


void VM::operator[] (const int index) {
    unsigned long page;
    int offset;
    SRAM sm;
    
    page = index / PG_SIZE;
    offset = index % PG_SIZE;
    cout << "page: " << page <<endl;
    
    //Search Page table for page value
    for (int i = 0; i < TABLE_SIZE; i++) {
        if (page == pageTable[i]) {
            cout << "Page found " << i <<endl;
            //get it from physical memory
            break;
        }else {
            //get  data from SRAM
            //cout << "Value of page in SRAM: " << sm.read_byte(page) <<endl;
            unsigned char value = sm.read_byte(page);
            //Store it in physical memory
            //Have to find a free Frame (victim)
            physicalMemory[page] = value;
            cout << "Physical memory[" << page+offset << "]" << physicalMemory[page + offset] << endl;
            //Update Page table with page number
            //User FIFO or Find a free
            
        }
    }
}
void VM::print() {
    for (int i = 0; i < TABLE_SIZE; i++) {
        cout << pageTable[i] <<endl;
    }
}
