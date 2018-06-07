#include <Streaming.h>
#include <Microchip_23LC1024.h>
#include <SPI.h>

#define SRAM_SIZE 131072
#define PG_SIZE 32

#define SRAM 5
#define HOLD 7

#define VERBOSE 0
#define TABLE_SIZE 1

class VM
{
    private :
    int pageTable [TABLE_SIZE];
    unsigned long page;
    unsigned long offset; 
    byte physicalAdressSpace[TABLE_SIZE * PG_SIZE]; //512 bytes
    Microchip_23LC1024 sram;
    
    public :
    VM(); 
    float fault_rate;
    unsigned long page_fault; 
    unsigned long memory_reference; 
    float getFaultRate();//returns current fault fate (#faults/#refs)
    void resetFaultRate();//resets page fault variables (faults = refs = 0)
    byte& operator[] (unsigned long index); 

     void printPageTable();
     void printPhysicalMemory(); 
     void printSRAM(); 
};



