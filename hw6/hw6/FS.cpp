#include "FS.h"

byte masks[8] = {0x01, 0x02, 0x04, 0x08, 0x10, 0x20, 0x40, 0x80};
bool file_found = false;
FS::FS() : eeprom() {

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
  for (int i = 0; i < BIT_VECTOR_SIZE; i++) {
    Serial << "bit_vector[" << i << "]" << "= ";
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


//Change specified bit in bit_vector from occuppied 0 to free 1
void FS::set_bit(int block_number, int offset) {
  byte value = bit_vector[block_number] | masks[offset];
  bit_vector[block_number] = value;
}


//Change specified bit in bit_vector from free 1 to occuppied 0
//invert the mask, then &.
void FS::clear_bit(int block_number, int offset) {
  byte inverted = ~masks[offset];
  byte value = bit_vector[block_number] & inverted;
  bit_vector[block_number] = value;
}

int FS::find_free_directory_spot() {
  for (int i = 0; i < 32; i++) {
    if (directory[i] == -1) {
      return i;
    }
  }
  Serial << "create_file ERROR: no space in FCB directory" << endl;
  return -1;
}


void FS::list_all_files() {
  Serial << "Listing Files..." << endl;
  for (int i = 0; i < DIRECTORY_SIZE; i++) {
    int bytes_counter = 0;
    if (directory[i] != -1) {
      eeprom.read_page(directory[i], (byte*)&fcb);
      for (int i = 0; i < 16; i++) {
        if (fcb.data_blocks[i] != -1) {
//          Serial <<"fcb.data_blocks[i]: " << fcb.data_blocks[i] << endl; 
          bytes_counter++;
        }
      }
      Serial << "File: " << fcb.file_name << ", " << bytes_counter * 64 << " bytes" << endl;
    }
  }
  return;
}

void FS::commit_to_eeprom() {
  eeprom.write_page(0, &bit_vector[0]);
  eeprom.write_page(1, (byte*)directory);
}

void FS::create(char *file_name) {
  //Steps to creating a file
  //  1. Check for duplicate
  //    - Get FCB
  //    - Compare FCB name
  for (int i = 0; i < DIRECTORY_SIZE; i++) {
    if (directory[i] != -1) {
      eeprom.read_page(directory[i], (byte*)&fcb);
      int res = strcmp(fcb.file_name, file_name);
      if (res == 0) {
        Serial << "File already exists: " << file_name << endl;
        return;
      }
    }
  }
  
  //  2. If no duplicate, then we check if there is an empty spot in directory
  //  will return an error if there is no space, else returns directory spot (int)
  int free_spot_on_directory = find_free_directory_spot();
  if(free_spot_on_directory == -1) return; 
  //  There was no duplicate file and there was space in directory
  //  3. Find free block on disk
  //  find_free_space() returns block number
  int free_spot_on_disk = find_free_space();

  //  4. Update Directory
  directory[free_spot_on_directory] = free_spot_on_disk;
  //4-a. copy file name to FCB structs, initialize data_blocks array 0
  strcpy(fcb.file_name, file_name);
  fcb.current_file_offset = 0;
  //  Serial << "Currnt file offset: " << fcb.current_file_offset <<endl;
  memset(fcb.data_blocks, -1, sizeof(fcb.data_blocks));
  //We save the spot on eeprom where FCB is held
  fcb.block_number_eeprom = free_spot_on_disk;
  //4-b. Update bit_vector
  int bit_vector_index = free_spot_on_disk / 8;
  int masks_index = free_spot_on_disk % 8;
  clear_bit(bit_vector_index, masks_index);

  //  5. Commit to disk
  Serial << "Creating File: " << file_name << endl;
  eeprom.write_page(free_spot_on_disk, (byte*)&fcb);

  //Write updated directory and bit_vector to disk
  commit_to_eeprom();
}


int FS::openf(char *file_name, FCB *fb) {
  for (int i = 0; i < DIRECTORY_SIZE; i++) {
    if (directory[i] != -1) {
      eeprom.read_page(directory[i], (byte*)&fcb);
      int res = strcmp(fcb.file_name, file_name);
      if (res == 0) {
        Serial << "Found file to open: " << fcb.file_name << endl;
        eeprom.read_page(directory[i], (byte*)fb);
        //Serial <<"Current File Offset: " << fb->current_file_offset <<endl;
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
  int number_of_free_blocks = 0;
  //64 bytes for each block
  for (int i = 0; i < 64; i++) {
    //8 for each mask
    for (int j = 0; j < 8; j++) {
      if ((bit_vector[i] & masks[j]) > 0) {
        //There was an empty space found
        number_of_free_blocks++; ;
      }
    }
  }
  int total_number_of_bytes_available = number_of_free_blocks * 64;
  //  Serial << "total_number_of_bytes_available" << total_number_of_bytes_available << endl;
  if (total_number_of_bytes_available >= number_of_bytes_requesting) {
    return true;
  } else {
    Serial << "Not enough room to write" << endl;
    return -1;
  }
}

void FS::writef(FCB *f, byte* m, int num_bytes) {
  /*******************************************
    Error checking: If there are free blocks
    EEPROM, if the file was found when open was
    called, if there is even room left in file
    to write the rest of the message.
  *******************************************/
//  for(int i = 0; i < num_bytes; i++) {
//    Serial <<"m[" << i << "]: " <<char(m[i]) << endl; 
//  }
  number_of_free_blocks_left(num_bytes);
  //check if there are any space on eeprom to write to

  //Before open check if file exists
  if (file_found == false) {
    Serial << "No file was found" << endl;
    return;
  }
  //check if file size is > 1024 and there is room to write to file
  if ((f->current_file_offset + num_bytes) > 1024) {
//    Serial << "f->current_file_offset: " <<f->current_file_offset<< endl;
//    Serial << "num_bytes: " << num_bytes << endl; 
    Serial << "File cannot be greater than 1024" << endl;
    return;
  }

  /*******************************************
     Done Checking now getting values to write
   *******************************************/
  byte duff_beer[64];

  //Get offset from File Control Blocks
  int offset = f->current_file_offset;
//  Serial << "initital offset: " << offset << endl;

  int index_of_datablock = offset / 64;
//  Serial << "index_of_datablock: " << index_of_datablock << endl;
  /*
     Check to see if the data overflows into multiple blocks.
     Each data_block is a spot in EEPROM which is 64 bytes
  */
  //This equation tells us how many blocks this write will take up
    int data_overflow = (offset + num_bytes) / 64;
//  Serial << "data_overflow: " << data_overflow << endl;


  //if data block == -1 we need to find a free spot in EEPROM
  if (f->data_blocks[index_of_datablock] == -1) {
    int free_block_eeprom = find_free_space();

    //Read in free block into our buffer
    eeprom.read_page(free_block_eeprom, (byte*)duff_beer);

    //Update bit_vectors
    int bit_vector_index = free_block_eeprom / 8;
    int masks_index = free_block_eeprom % 8;
    clear_bit(bit_vector_index, masks_index);
    //Update FCB's data_block with found free block from eeprom
    f->data_blocks[index_of_datablock] = free_block_eeprom;
  } else {
    //If data block != -1, then it has a spot in eeprom already allocated
    eeprom.read_page(f->data_blocks[index_of_datablock], (byte*)duff_beer);
  }

  int bytes_written = 0;
  /*
     In duff_beer array we have either a new block of data, or a page that is read in
     offset should determind which data block index we are at
  */

  //if Data_overflow == 0 then all bytes will fit into current block
  if (data_overflow == 0) {
    for (int i = offset; i < (offset + num_bytes); i++) {
      duff_beer[i] = m[bytes_written];
      bytes_written++; 
//      Serial << "duff_beer[" << i << "]" <<   char(duff_beer[i]) << endl;
    }
    //From message on in byte array, will fill in rest with null to get rid of garbage
    for (int i = (offset + num_bytes); i < 64; i++) {
      duff_beer[i] = NULL;
//      Serial <<"duff_beer[" << i << "]" <<   char(duff_beer[i]) << endl;
    }
      //Update file offset
      f->current_file_offset += num_bytes;
      //Write message to eeprom
      eeprom.write_page(f->data_blocks[index_of_datablock], duff_beer);
      //update FCB if new data block values were allocated
      eeprom.write_page(f->block_number_eeprom, (byte*)f);
      //Update bit_vector not so much directory
      commit_to_eeprom(); 
      return; 
  }else {
      //If data_overflow != 0 then have to write to next data block and find new memory on EEPROM
      //Finish writting to current data_block
      for (int j = offset; j < 64; j++) {
        duff_beer[j] = m[bytes_written];
        bytes_written++;
//        Serial << "duff_beer[" << j << "]" <<   char(duff_beer[j]) << endl;
      }
     
      //Finished writting to all of current block, now commit it to eeprom.
      eeprom.write_page(f->data_blocks[index_of_datablock], duff_beer);  
      //update offset
      offset += bytes_written; 
      f->current_file_offset = offset;
      eeprom.write_page(f->block_number_eeprom, (byte*)f); 
      memset(duff_beer, NULL, sizeof(duff_beer)); 

      /*
       * Keep on allocating new blocks until all bytes have been written
       */
      while (bytes_written != num_bytes) {
//        Serial <<"Bytes written so far: " << bytes_written << endl; 
//        Serial <<"Number of bytes requested: " << num_bytes << endl; 
//        Serial <<"Bytes remaining: " << num_bytes - bytes_written << endl; 
        int bytes_remaining = num_bytes - bytes_written; 
        //update next_data_block
//        Serial <<"In while loop Offset: " << offset << endl;
        int new_index_of_datablock = offset / 64;
//        Serial << "new_index_of_datablock: " << new_index_of_datablock << endl;
        
        if (f->data_blocks[new_index_of_datablock] == -1) {
          int free_block_eeprom = find_free_space();
          //Read in free block into our buffer
          eeprom.read_page(free_block_eeprom, duff_beer);
  
          //Update bit_vectors
          int bit_vector_index = free_block_eeprom / 8;
          int masks_index = free_block_eeprom % 8;
          clear_bit(bit_vector_index, masks_index);
          //Update FCB's data_block with found free block from eeprom
          f->data_blocks[new_index_of_datablock] = free_block_eeprom;
           //update FCB if new data block values were allocated
          eeprom.write_page(f->block_number_eeprom, (byte*)f);
        } else {
          //If data block != -1, then it has a spot in eeprom already allocated
          eeprom.read_page(f->data_blocks[new_index_of_datablock], duff_beer);
        }
        
        //With next data block selected with either new allocated EEPROM or existing EEPROM
        //beging to fill in the rest of the message
        if(bytes_remaining > 64) {
          for(int i = 0; i < 64; i++){
           duff_beer[i] = m[bytes_written]; 
//           Serial << "duff_beer[" << i << "]" <<   char(duff_beer[i]) << endl;
           bytes_written++; 
          }
         }else {
            for(int i = 0; i < bytes_remaining; i++){
             duff_beer[i] = m[bytes_written]; 
//             Serial << "duff_beer[" << i << "]" <<   char(duff_beer[i]) << endl;
             bytes_written++; 
            }
            for(int i = bytes_remaining; i < 64; i++){
             duff_beer[i] = NULL; 
            }  
          }
        offset += bytes_written; 
        f->current_file_offset = offset;
        eeprom.write_page(f->block_number_eeprom, (byte*)f);
        eeprom.write_page(f->data_blocks[new_index_of_datablock], duff_beer);  
      }
      
      commit_to_eeprom(); 
      //If here then all the bytes have been written and can commit to eeprom
  }

 
  return;
}


void FS::readf(FCB *f) {
  byte buff[64];
  int count = 0;
  if(file_found) {
    for (int i = 0; i < 16; i++) {
      if (f->data_blocks[i] != -1) {
        eeprom.read_page(f->data_blocks[i], buff);
        for (int j = 0; j < 64; j++) {
          Serial << char(buff[j]);
        }
        Serial << endl;
        memset(buff, NULL, sizeof(buff)); 
      } else {
        count++;
      }
    }
    if (count == 16) {
      Serial << "No data to be read" << endl;
      return;
    }
  }else {
    Serial << "No file to be read" << endl; 
  }
  return;
}



void FS::deletef(char *file_name) {
  //Steps to deleting a file oppostie of creating a file
  //  1. Check for file
  //    - Get FCB
  //    - Compare FCB name
  for (int i = 0; i < DIRECTORY_SIZE; i++) {
    //-1 is an empty spot we are looking for the file to delete
    if (directory[i] != -1) {
      //retrieving fcb from eeprom to check its file name
      eeprom.read_page(directory[i], (byte*)&fcb);
      int res = strcmp(fcb.file_name, file_name);
      //if file was found go into this loop
      if (res == 0) {
        Serial << "Deleting File: " << file_name << endl;
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

void FS::closef() {
  Serial << "Closing file..." << endl;
  commit_to_eeprom();
}


void FS::seekf(FCB *f) {
  //Go to the beginning of a file
  if(file_found) {
    f->current_file_offset = 0; 
    eeprom.write_page(f->block_number_eeprom, (byte*)f); 
  }else {
    Serial << "No file selected to seek" << endl; 
  }

  
}int FS::find_free_space() {
  //from right to left check if bit is 0 or 1
  int block_number = 0;
  //64 for 64 bytes in the bit_vector
  for (int i = 0; i < 64; i++) {
    //8 for each mask
    for (int j = 0; j < 8; j++) {
      if ((bit_vector[i] & masks[j]) > 0) {
        //There was an empty space found
        return block_number;
      } else {
        //Iterate over each bit which represents 1 block of EEPROM
        block_number++;
      }
    }
  }
  //What if no free space
  Serial << "No Free Space in bit_vector" << endl;
  return -1;
}

void FS::printBitVector() {
  for (int i = 0; i < 5; i++) {
    Serial << "bit_vector[" << i << "]" << "= ";
    Serial.println(bit_vector[i], BIN);
  }
}


void FS::printDirectoryList() {
  for (int i = 0; i < DIRECTORY_SIZE ; i++) {
    Serial << "directory[" << i << "]" << "= " << directory[i] << endl;
  }
}







