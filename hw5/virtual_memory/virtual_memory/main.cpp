//
//  main.cpp
//  virtual_memory
//
//  Created by Kyleen Gonzalez on 5/15/18.
//  Copyright © 2018 Kyleen Gonzalez. All rights reserved.
//
#include "VM.h"
#include "SRAM.h"
#include <iostream>

using namespace std;



int main(int argc, const char * argv[]) {
    VM vm;
    SRAM sm; 
    vm[100] = 'A';
    
    cout << "vm[100]: "<< vm[100]<<endl; 
//    vm.print();
   // sm.print();
    return 0;
}
