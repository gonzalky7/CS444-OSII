#include "VM.h"

VM::VM() : sram(SRAM, HOLD) { //constuctor initliazation list
  for (int i = 0; i < TABLE_SIZE; i++) {
    pageTable[i] = -1;//filling with invalid
  }
  for (int i = 0; i < (TABLE_SIZE * PG_SIZE); i++) {
    physicalAdressSpace[i] = '-';//filling with invalid
  }
  fault_rate = 0;
  memory_reference = 0;
  page_fault = 0;
}

int index_value = 0;
int victim_counter_for_page_table = 0;
bool first_flag = false;
bool second_flag = false;
bool victim_counter_flag = false;
unsigned long i;


byte& VM::operator[] (unsigned long index)
{
  if (index < 0 && index < 131072) {
    Serial << "Index must be > 0 and < 131,072" << endl;
    Serial << "Index: " << index << endl;
    while (1) {
    }
  }

  /*
     Do calculations for finding page and offset
  */
  page = index / PG_SIZE;
  offset = index % PG_SIZE;
  if (VERBOSE == 1) {
    Serial << "=======================================" << endl;
    Serial << "requesting virtual address: " << index << endl;
    Serial << "    page = " << page << ",  offset = " << offset << endl;
  }
  memory_reference++;
  /* 1). Cycle through page table and look for page
      that user is requesting.
      If page is found will use the calculated page number
      and the offset to either retrieve or write.
  */

  for (int i = 0; i < TABLE_SIZE; i++) {
    if (page == pageTable[i]) {
      index_value = (i * 32) + offset;

      if (VERBOSE == 1) {
        Serial << "Page FOUND!" << endl;
        Serial << "page table entry: " << i << endl;
        Serial << "physical address: " << index_value << endl;
      }
      //first_flag = false;
      return physicalAdressSpace[index_value];
    }
    //Even though we will set this with each iteration of
    //the floor loop if page is found if loop will set it back to false
    //first_flag = true; //Did not find the page
  }
  /*
    2). If pageTable[i] == -1 that means that there is a free spot and no
    page has been allocated, page table and physical memory is one to one
    i.e. if page table at 7 is empty Frame 7 in physical memory will be empty
  */
  for (int i = 0; i < TABLE_SIZE; i++) {

    if (pageTable[i] == -1) {
      if (VERBOSE == 1) {
        Serial << "PAGE FAULT!" << endl;
        Serial << "   empty slot in page table: " << i << endl;
        Serial << "   reading SRAM memory page:" << page << endl;
        Serial << "   physical address: " << (i * 32) + offset << endl;
      }
      page_fault++;
      //Reading memory from SRAM
      sram.read_page(page, &physicalAdressSpace[i * 32]);

      //Update the page table
      pageTable[i] = page;

      //Calculate offset to specify where in Frame to write value
      index_value = (i * 32) + offset;
      return physicalAdressSpace[index_value];
    }

  }
  /*
     3). If page was not found in page table and there were no
     free pages. We kick a page out of physical memory/update page table store
     it in SRAM and then load into physical memory page
  */


  //grab the next victim page in page table
  //victim_counter_for_page_table is the index of page table 0 to 15
  
  int victim_page = pageTable[victim_counter_for_page_table];
  int victim_index = victim_counter_for_page_table;
  victim_counter_for_page_table++; 
  victim_counter_for_page_table %= TABLE_SIZE; 
  
  //Write victim frame to index of SRAM all 32 bytes of physicalAddressSpace[at page specified]
  //                        SRAM [1]                                   [1]
  sram.write_page(victim_page, &physicalAdressSpace[victim_index * 32]);

  //READ PAGE 0 of SRAM into                   phsyicalAddress[1]
  sram.read_page(page, &physicalAdressSpace[victim_index * 32]);

  /*I have written contents of physical memory that we
     FIFOd out of the page table into SRAM.
     Now can replace with new information call
  */
  if (VERBOSE == 1) {
    Serial << "PAGE FAULT! " << endl;
    Serial << "Page fault: " << page_fault << endl; 
    Serial << "   NO SPACE in page table" << endl;
    Serial << "   page OUT: " << victim_page << endl;
    Serial << "   page IN: " << page << endl;
  }
  page_fault++; 
  //update page table with new page
  pageTable[victim_index] = page;


  return physicalAdressSpace[(victim_index * 32) + offset]; //512 bytes
}


float VM::getFaultRate() {
  //returns current fault fate (#faults/#refs)
  fault_rate = page_fault / memory_reference;
  Serial << "_=_=_=_=_=_=_=_=_=_=_=_=_=_=_=_=_=" << endl;
  //Serial << "Fault Rate: " << page_fault <<"/" << memory_reference << endl; 
  Serial << "Page Fault rate: " << fault_rate << endl;
}
void VM::resetFaultRate() {
  //resets page fault variables (faults = refs = 0)
  page_fault = memory_reference = 0;
}



