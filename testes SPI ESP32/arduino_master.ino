#include<SPI.h>

void setup()
{
  SPI.begin();                            //Begins the SPI commnuication
  SPI.setDataMode(SPI_MODE0);
  SPI.setClockDivider(SPI_CLOCK_DIV8);    //Sets clock for SPI communication at 8 (16/8=2Mhz)
  digitalWrite(SS, HIGH);                 // Setting SlaveSelect as HIGH (So master doesnt connnect with slave)



  Serial.begin(9600);
  uint8_t wav_sound[] = {
    0x52, 0x49, 0x46, 0x46, 0x3A, 0x00, 0x00, 0x00, 0x57, 0x41, 0x56, 0x45, 0x66, 0x6D, 0x74, 0x20,
    0x10, 0x00, 0x00, 0x00, 0x07, 0x00, 0x01, 0x00, 0x40, 0x1F, 0x00, 0x00, 0x40, 0x1F, 0x00, 0x00,
    0x01, 0x00, 0x08, 0x00, 0x58, 0x78, 0x58, 0x78, 0x03, 0x00, 0x00, 0x00, 0x64, 0x64, 0x64, 0x00,
    0x64, 0x61, 0x74, 0x61, 0x09, 0x00, 0x00, 0x00, 0x43, 0x2F, 0x26, 0x2C, 0x35, 0x3A, 0xDF, 0xC7,
    0xBE, 0x00
  };

  digitalWrite(SS, LOW);
  SPI.transfer(wav_sound,sizeof(wav_sound));  
  digitalWrite(SS, HIGH);

}

void loop()
{



}