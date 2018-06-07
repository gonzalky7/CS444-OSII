#include "VM.h"

void setup() {
  Serial.begin(115200);
  Serial.flush();
  VM vm;

long randNumber = 0;
long summation = 0;
byte b = 0; 
 
for(unsigned long i = 0; i < 50000; i++){
    randNumber = random(10);
    vm[i] = randNumber; 
}

for(unsigned long i = 50000; i < 100000; i++){
    randNumber = random(10);
    vm[i] = randNumber; 
}
 

for(unsigned long i = 0; i < 100000; i++){
   summation += vm[i]; 
}

Serial << "Summation: " << summation <<endl; 
vm.getFaultRate();
vm.resetFaultRate();
/*
 * Thrashing::Page fault occurs every time there is a 
 * reference to memeory
 */

char *msg = "It’s not a bug – it’s an undocumented feature.";

for(int i = 0; i < strlen(msg); i ++) {
    vm[i*32] = msg[i];  
}
vm.getFaultRate();
 







  //vm.printPhysicalMemory();
  //vm.printPageTable();
  //vm.printSRAM();

}

void loop() {
}
