#include <Microchip_23LC1024.h>
#include <SPI.h>

#define SRAM_SIZE 131072
#define PG_SIZE 32

#define SRAM 5
#define HOLD 7

//instantiate the sram object on pins 5 and 7
Microchip_23LC1024 sram( SRAM, HOLD);

void setup()
{
  unsigned long i;
  
  Serial.begin(115200);
  Serial.flush();

  Serial.println("Writing bytes to SRAM...");

  //write 0xCE to every byte in memory
  //    this may take a while
  for(i = 0; i < SRAM_SIZE; i++){
    sram.write_byte(i, 0xCE);
  }
  
  //read and print from every byte in memory
  //   should all be 0xCE
  for(i = 0; i < SRAM_SIZE; i++){
    byte b = sram.read_byte(i);
    Serial.print("sram[");
    Serial.print(i);
    Serial.print("] = ");
    Serial.println(b, HEX);
  }

  //32-byte message to fit in exactly one page
  byte msg[32] = {'H','E','L','L','O',' ','W','O','R','L','D','!',
                    '\0','\0','\0','\0','\0','\0','\0','\0','\0','\0',
                    '\0','\0','\0','\0','\0','\0','\0','\0','\0','\0'};
                    
  //write the message to each of the 4096 pages
  for(i = 0; i < SRAM_SIZE / PG_SIZE; i++){
    sram.write_page(i, msg);
  }
  
  //read the message from each of the 4096 pages
  for(i = 0; i < SRAM_SIZE / PG_SIZE; i++){
    //first clear the message (no cheating)
    for(int j = 0; j < PG_SIZE; j++) msg[j] = 0;
    
    //read in the page from memory
    sram.read_page(i, msg);
    
    //print msg
    Serial.print("page ");
    Serial.print(i);
    Serial.print(" = ");
    Serial.println((char *) msg);
  }
  
  //sit in infinite loop..
  while (true);
}

void loop() 
{

}
