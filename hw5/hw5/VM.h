#include <Streaming.h>
#include <Microchip_23LC1024.h>
#include <SPI.h>

#define SRAM_SIZE 131072
#define PG_SIZE 32

#define SRAM 5
#define HOLD 7

#define VERBOSE 1
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
    int fault_rate;
    int page_fault; 
    int memory_reference; 
    int getFaultRate();//returns current fault fate (#faults/#refs)
    int resetFaultRate();//resets page fault variables (faults = refs = 0)
    byte& operator[] (const int index); 

     void printPageTable();
     void printPhysicalMemory(); 
     void printSRAM(); 
};



