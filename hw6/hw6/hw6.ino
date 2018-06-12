#include "FS.h"

void setup() {
  Serial.begin(115200);
  Serial.flush();
  
  FS fs; 
  FCB f; 
  
//  fs.reformat(); 
  fs.initialize(); 


  //Create 33 files and get an error for trying to create more than 32
//  char file_name[28]; 
//  for (int i = 0; i < 33; ++i) {
//    sprintf(file_name, "test_%02d.txt", i);  
//    fs.create(file_name);
//  }
//
//  //Error checking try to create files that already exits
//  memset(file_name,NULL,sizeof(file_name));
//  for (int i = 0; i < 5; ++i) {
//    sprintf(file_name, "test_%02d.txt", i);  
//    fs.create(file_name);
//  }

    //List all the created files
//    fs.list_all_files();

   
//    //Delete files
//    memset(file_name,NULL,sizeof(file_name));
//    for (int i = 0; i < 4; ++i) {
//    sprintf(file_name, "test_%02d.txt", i);  
//    fs.deletef(file_name);
//  }
    //Demonstrate that files were deleted from directory
//    fs.list_all_files();
 
    //Open a created file and write a small message
//    fs.openf("test_10.txt", &f); 
//    fs.writef(&f, "hi", 2);
//
//    //Then read the message that was written to the file
//    fs.readf(&f); 
//
//    //Error checking
    //Try to open a file that does not exist
//    fs.openf("does_not_exist.txt",&f); 
//    //Try to write to a file that does not exist
//    fs.writef(&f,"does not exits", 14); 
//    //Try to delete a file that does not exit
//    fs.deletef("does_not_exist.txt"); 
//    //Try to seek a file that does not exist
//    fs.seekf(&f); 
    
    byte overflow_msg[156] = {'H','E','L','L','O',' ','W','O','R','L','D','!',
                             'H','E','L','L','O',' ','W','O','R','L','D','!',
                             'H','E','L','L','O',' ','W','O','R','L','D','!',
                             'H','E','L','L','O',' ','W','O','R','L','D','!',
                             'H','E','L','L','O',' ','W','O','R','L','D','!',
                             'H','E','L','L','O',' ','W','O','R','L','D','!',
                             'H','E','L','L','O',' ','W','O','R','L','D','!',
                             'H','E','L','L','O',' ','W','O','R','L','D','!',
                             'H','E','L','L','O',' ','W','O','R','L','D','!',
                             'H','E','L','L','O',' ','W','O','R','L','D','!',
                             'H','E','L','L','O',' ','W','O','R','L','D','!',
                             'H','E','L','L','O',' ','W','O','R','L','D','!',
                             'H','E','L','L','O',' ','W','O','R','L','D','!'}; 
      
 
   //Write a msg that overflows, then read the msg,then list files
//    fs.openf("test_19.txt", &f);
//    fs.writef(&f, overflow_msg, 156);  
//    fs.readf(&f);  
//    fs.list_all_files();

  //Not sure what to use seek for, but I use it to find the beginning of the file and write over
  //existing content
//    fs.openf("test_00.txt", &f);
//    byte msg[2] = {'h','i'}; 
//    fs.writef(&f, msg, 2); 
//    fs.readf(&f);
//    byte second_msg[3] = {'b','y','e'}; 
//    fs.seekf(&f);
//    //write over the current file  
//    fs.writef(&f, second_msg, 3); 
//    fs.readf(&f); 
//    fs.list_all_files(); 



  
  //Writting to a file that is more than 1024 bytes
  byte over_msg[64] = {'H','E','L','L','O',' ','W','O','R','L','D','!',
                             'H','E','L','L','O',' ','W','O','R','L','D','!',
                             'H','E','L','L','O',' ','W','O','R','L','D','!',
                             'H','E','L','L','O',' ','W','O','R','L','D','!',
                             'H','E','L','L','O',' ','W','O','R','L','D','!',
                             'H','E','L','L'}; 
                                                       
  //Don't judge me lol, I tried using a for loop                           
  fs.openf("test_06.txt", &f);
  fs.writef(&f, over_msg, 64);  
  fs.writef(&f, over_msg, 64); 
  fs.writef(&f, over_msg, 64); 
  fs.writef(&f, over_msg, 64); 
  fs.writef(&f, over_msg, 64);  
  fs.writef(&f, over_msg, 64); 
  fs.writef(&f, over_msg, 64); 
  fs.writef(&f, over_msg, 64);
  fs.writef(&f, over_msg, 64);  
  fs.writef(&f, over_msg, 64); 
  fs.writef(&f, over_msg, 64); 
  fs.writef(&f, over_msg, 64);
  fs.writef(&f, over_msg, 64);  
  fs.writef(&f, over_msg, 64); 
  fs.writef(&f, over_msg, 64); 
  fs.writef(&f, over_msg, 64);
  fs.list_all_files(); 


  //This will be over
  fs.writef(&f, over_msg, 64);
  fs.fs.readf(&f); 
  //For unplugging and plugging arduino
//  fs.list_all_files(); 
  
}





void loop() {
  // put your main code here, to run repeatedly:

}
