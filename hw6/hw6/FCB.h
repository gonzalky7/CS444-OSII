
struct FCB {
  char file_name[30];
  int current_file_offset;
  int data_blocks[16];//1024 / 16 = 64
  
  //Files can contain no more than 1024 bytes of data
  //16 pointers to data blocks(initialized to 0 or NULL)
};
