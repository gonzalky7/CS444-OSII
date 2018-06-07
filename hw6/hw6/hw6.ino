#include "FS.h"

void setup() {
  Serial.begin(115200);
  Serial.flush();
  
  FS fs; 
  FCB f; 
  
//
  //fs.reformat(); 
  fs.initialize(); 
//  fs.printBitVector(); 
//  fs.list_all_files(); 
//  char file_name[30]; 
//  for (int i = 0; i < 5; ++i) {
//    int total_num_char_written = sprintf(file_name, "test_%02d.txt", i);  
//    fs.create(file_name);
//  }
//  fs.create("test_05.txt");
//  fs.create("test_06.txt");
  fs.openf("test_04.txt", &f); 
//  fs.deletef("test_03.txt");

  fs.writef(&f, 'hi', 2); 
  fs.list_all_files();
//  fs.printBitVector(); 
  //fs.printDirectoryList(); 
  //fs.printEeprom(); 
}





void loop() {
  // put your main code here, to run repeatedly:

}
