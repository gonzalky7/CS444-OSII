#include <MsTimer2.h>
#include <Microchip_MCP3002.h>

Microchip_MCP3002 adc(4); //chip_select_pin

void sample() {
  float x = adc.getSample();
  Serial.print("Analog value: ");
  Serial.println(x); 
}

void setup() {
  Serial.begin(115200);
  //setting channel
  adc.setChannel(1); 
  MsTimer2::set(10, sample); //100hz equals 10ms period
  MsTimer2::start();
}

void loop() {
  // put your main code here, to run repeatedly:

}
