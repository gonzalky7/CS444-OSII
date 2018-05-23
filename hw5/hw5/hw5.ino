#include "VM.h"

void setup() {
  Serial.begin(115200);
  Serial.flush();
  VM vm;


  char *msg = "Hello from";
  //this fills up the page table

  for (int i = 0; i < strlen(msg); i++) {
    vm[i * 32] = msg[i];
  }


  for (int i = 0; i < strlen(msg); i++) {
    Serial << (char) vm[i * 32];
  }

//vm[7978] = 100; 
//vm[127937] = 100; 
//vm[39847] = vm[7978] + vm[127937];
//
//Serial << "vm[7978] + vm[127937]" << " = " << vm[39847] << endl; 

Serial << "\n";
  //After message has been filed page table should have page values

  //vm.printPhysicalMemory();
  //vm.printPageTable();
  //vm.printSRAM();

}

void loop() {
}
