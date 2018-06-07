#include "FS.h"

byte masks[8]= {0x01,0x02,0x04,0x08,0x10,0x20,0x40,0x80};
bool file_found = false; 
FS::FS() : eeprom(){

}

//0 = occupied
//1 = free space
void FS::reformat() {
  Serial << "Reformatting EEPROM Chip..." << endl; 
  //we set each byte (64) in bit_vector to 8 bits (1)
  memset(bit_vector, 0xFF, sizeof(bit_vector)); 
  //Need to set the first byte's first two bits as occupied
  bit_vector[0] = 0b11111100;

  //Initializing director block of pointer to -1
  for (int i = 0; i < 32; i++) {
    directory[i] = -1;
  }
 
  //write to EEPROM
  for (int i = 0; i < BIT_VECTOR_SIZE; i++){
    Serial <<"bit_vector[" << i << "]" <<"= "; 
    Serial.println(bit_vector[i], BIN); 
   }
  eeprom.write_page(0, &bit_vector[0]); 
  eeprom.write_page(1, (byte*)directory); 
}

void FS::initialize() {
  Serial << "Initializing..." << endl; 
  //Reading in block 0 which holds bit vector data
  eeprom.read_page(0, &bit_vector[0]); 
  //Reading in block 1 which holds directory list information
  eeprom.read_page(1, (byte*)directory); 
}


int FS::find_free_space() { 
  //from right to left check if bit is 0 or 1
  int block_number = 0;
  //64 for 64 bytes in the bit_vector 
  for (int i = 0; i < 64; i++){
    //8 for each mask
    for(int j = 0; j < 8; j++){
      if((bit_vector[i] & masks[j]) > 0){
        //There was an empty space found
        return block_number; 
      }else{
        //Iterate over each bit which represents 1 block of EEPROM
        block_number++; 
      }
    }
  }
  //What if no free space
  Serial << "No Free Space in bit_vector" << endl; 
  return -1; 
}

//Change specified bit in bit_vector from occuppied 0 to free 1
void FS::set_bit(int block_number, int offset){
   byte value = bit_vector[block_number] | masks[offset]; 
   bit_vector[block_number] = value; 
}


//Change specified bit in bit_vector from free 1 to occuppied 0
//invert the mask, then &. 
void FS::clear_bit(int block_number, int offset){
   byte inverted = ~masks[offset];
   byte value = bit_vector[block_number] & inverted; 
   bit_vector[block_number] = value; 
}

int FS::find_free_directory_spot() {
    for (int i = 0; i < 32; i++){
    if(directory[i] == -1) {
        return i; 
      }
    }
    Serial << "create_file ERROR: no space in FCB directory" << endl; 
    return; 
}


void FS::list_all_files() {
  Serial << "Listing Files..." <<endl; 
  for(int i = 0; i < DIRECTORY_SIZE; i++) {
    int bytes_counter = 0; 
    if(directory[i] != -1) {
      eeprom.read_page(directory[i], (byte*)&fcb);
      for(int i = 0; i < 16; i++) {
        if (fcb.data_blocks[i] != 0){
           bytes_counter++; 
        }
      }
      Serial << "File: " << fcb.file_name <<", "<<bytes_counter<< " bytes"<<endl; 
    }
  }
  return; 
}

void FS::commit_to_eeprom(){
  eeprom.write_page(0, &bit_vector[0]); 
  eeprom.write_page(1, (byte*)directory); 
}

void FS::create(char *file_name) { 
  Serial << "Creating File: " << file_name << endl; 
  //Steps to creating a file
  //  1. Check for duplicate
  //    - Get FCB
  //    - Compare FCB name
  for (int i = 0; i < DIRECTORY_SIZE; i++){  
    if(directory[i] != -1) {
      eeprom.read_page(directory[i], (byte*)&fcb); 
      int res = strcmp(fcb.file_name,file_name);
      if (res==0){
        Serial << "File already exists: "<< file_name << endl;
        return; 
      }
    }
  }

  //  2. If no duplicate, then we check if there is an empty spot in directory
  //  will return an error if there is no space, else returns directory spot (int)
      int free_spot_on_directory = find_free_directory_spot();
      
  //  There was no duplicate file and there was space in directory 
  //  3. Find free block on disk
  //  find_free_space() returns block number
      int free_spot_on_disk = find_free_space();
      
  //  4. Update Directory
      directory[free_spot_on_directory] = free_spot_on_disk;
      //4-a. copy file name to FCB structs, initialize data_blocks array 0
        strcpy(fcb.file_name, file_name); 
        fcb.current_file_offset = 0; 
        memset(fcb.data_blocks,-1,sizeof(fcb.data_blocks));
      //4-b. Update bit_vector 
         int bit_vector_index = free_spot_on_disk / 8; 
         int masks_index = free_spot_on_disk % 8; 
         clear_bit(bit_vector_index, masks_index); 
        
  //  5. Commit to disk
      eeprom.write_page(free_spot_on_disk, (byte*)&fcb); 
      
      //Write updated directory and bit_vector to disk
      commit_to_eeprom();
}


int FS::openf(char *file_name, FCB *fb){
  for (int i = 0; i < DIRECTORY_SIZE; i++){  
    if(directory[i] != -1) {
      eeprom.read_page(directory[i], (byte*)&fcb); 
      int res = strcmp(fcb.file_name,file_name);
      if (res==0){
        Serial << "Found file to open: " << fcb.file_name <<endl; 
        eeprom.read_page(directory[i], (byte*)fb);
        file_found = true; 
        return; 
      }
    }
  }
  Serial << "File not Found" << endl; 
  file_found = false; 
  return -1; 
}

bool FS::number_of_free_blocks_left(int number_of_bytes_requesting) {
  //from right to left check if bit is 0 or 1
  //count is number of bytes requesting
  int number_of_free_blocks = 0;
  //64 bytes for each block
  for (int i = 0; i < 64; i++){
    //8 for each mask
    for(int j = 0; j < 8; j++){
      if((bit_vector[i] & masks[j]) > 0){
        //There was an empty space found
        number_of_free_blocks++; ; 
      }
    }
  }
  int total_number_of_bytes_available = number_of_free_blocks * 64; 
//  Serial << "total_number_of_bytes_available" << total_number_of_bytes_available << endl;
  if(total_number_of_bytes_available >= number_of_bytes_requesting) {
    return true; 
  }else {
    Serial << "Not enough room to write"<< endl; 
    return -1; 
  }
}

void FS::writef(FCB *f, void *m, int num_bytes) {  
   char duff[64];
   memset(duff, 0, sizeof(duff)); 
   
   number_of_free_blocks_left(num_bytes);
  //check if there are any space on eeprom to write to
  
  //Before open check if file exists 
  if(file_found == false) {
    Serial << "No file was found" <<endl; 
    return; 
  }
  //check if file size is > 1024
  if(num_bytes > 1024) return; 

  //find a free spot in EEPROM to begin writing
  //returns block number
  int free_block_eeprom = find_free_space(); 
  Serial << "free_block_eeprom: " <<free_block_eeprom << endl; 
  //Get offset 
  int offset = f->current_file_offset; 
  Serial << "offset: " << offset << endl; 
  
  int index_of_datablock = offset/64; 
  Serial << "index_of_datablock: " << index_of_datablock << endl; 
  //int data_overflow = f->current_file_offset + num_bytes; 
  //eeprom.read_page(free_block_eeprom, duff);
  for(int i - 0 ; i < num_bytes ; i ++ ){
      cout << m[i] ;
   }
  
//   Serial << "f.file_name: " << f->file_name << endl ;
//   Serial <<"current_file_offset: " << f->current_file_offset<< endl;
//   for (int i = 0; i < 16; i++) {
//    Serial <<"f.data_blocks[" << i << "]" << f->data_blocks[i] << endl; 
//   }
}


void FS::deletef(char *file_name) {
   Serial << "Deleting File: " << file_name << endl; 
  //Steps to deleting a file oppostie of creating a file
  //  1. Check for file
  //    - Get FCB
  //    - Compare FCB name
  for (int i = 0; i < DIRECTORY_SIZE; i++){  
    //-1 is an empty spot we are looking for the file to delete
    if(directory[i] != -1) {
      //retrieving fcb from eeprom to check its file name
      eeprom.read_page(directory[i], (byte*)&fcb); 
      int res = strcmp(fcb.file_name,file_name);
      //if file was found go into this loop
      if (res==0){
        Serial << "Found file to delete: "<< fcb.file_name << endl;
        //Grab eeprom block number from inside directory reset
        int eeprom_block_number = directory[i]; 
        directory[i] = -1; 
        //Set bit in bit_vectory to empty 1
        int bit_vector_index = eeprom_block_number / 8; 
        int masks_index = eeprom_block_number % 8; 
        set_bit(bit_vector_index, masks_index); 
        
        //Write updated directory and bit_vector to disk
       commit_to_eeprom();
       return; 
      }
    }
  }
  Serial << "File not found" << endl;
  return -1; 
}

void FS::closef(char *file_name, FCB *f) {



  
}








void FS::printBitVector() {
   for (int i = 0; i < 5; i++){
    Serial <<"bit_vector[" << i << "]" <<"= "; 
    Serial.println(bit_vector[i], BIN); 
   }
}


void FS::printDirectoryList(){
  for (int i = 0; i < DIRECTORY_SIZE ; i++){
    Serial <<"directory[" << i << "]" <<"= "<< directory[i] <<endl; 
   }
}







