
struct FCB {
  char file_name[28];//28 bytes
  int current_file_offset;//2 bytes
  int block_number_eeprom; //Needed this bc when I went to write f back into eeprom with updated values 2bytes
  //needed its spot in eeprom 
  int data_blocks[16];//1024 / 16 = 64 32 bytes
  
  //Files can contain no more than 1024 bytes of data
  //16 pointers to data blocks(initialized to 0 or NULL)
};
