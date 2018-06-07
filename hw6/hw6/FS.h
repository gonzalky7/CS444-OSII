#include <Streaming.h>
#include <string.h>
#include "FCB.h"
#include <Microchip_24LC256.h>
#define BIT_VECTOR_SIZE 64
#define DIRECTORY_SIZE 32


class FS
{
  private:
    Microchip_24LC256 eeprom;
    //1 bit per block of memory on EEPROM
    byte bit_vector[BIT_VECTOR_SIZE]; //Byte 8 bits x 64 = 512
    int directory[DIRECTORY_SIZE]; 
    //FCB
    FCB fcb; 
    
   public:
    FS(); 
    void reformat();// Reformat EEPROM chip (clear free-space list and file directory)
    void initialize(); //Initialize file system (bring free-space list & file directory
    //into memory)
    void create(char *file_name); 
    int openf(char *file_name, FCB *f); 
    void writef(FCB *f, void *m, int num_bytes); 
    //read bytes from a file
    //seek to beginning of a file (only)
    void closef(char *file_name, FCB *f); 
    void deletef(char *file_name); 
    void commit_to_eeprom(); 
    void list_all_files(); 
    int find_free_directory_spot(); 
    int find_free_space();
    void set_bit(int block_number, int offset); 
    void clear_bit(int block_number, int offset); 
    bool FS::number_of_free_blocks_left(int count);
    void printDirectoryList(); 
    void printBitVector(); 
}; 
  

