//
//  SRAM.cpp
//  virtual_memory
//
//  Created by Kyleen Gonzalez on 5/17/18.
//  Copyright © 2018 Kyleen Gonzalez. All rights reserved.
//
#include "SRAM.h"
#include <iostream>
using namespace std;

SRAM::SRAM() {
    for (int i = 0; i < SIZE; i++) {
        sram_memory[i] = 'x'; 
    }
}

unsigned char SRAM::read_byte (unsigned long address) {
    unsigned char value;
    value = sram_memory[address];
    
    return value;
}

void SRAM::write_byte(unsigned long address, unsigned char b) {
    sram_memory[address] = b;
}
void SRAM::print() {
    for (int i = 0; i < SIZE; i++) {
        cout << "[" << i << "]" << sram_memory[i] <<endl;
    }
}
