#include <stdio.h>
#include "simple_ftp_client.h"
#include "SD_card_helper.h"
#include "button_helper.h"
#include "led.h"
#include "crc32.h"

const char* ssid       = "35_NET_2G"; //Nome da rede Wifi
const char* password = "Ap35Manfrim";//Senha da rede Wifi

char* host = "192.168.0.146"; //Servidor FTP
unsigned int ftpPort = 21;  //Porta Servidor FTP
unsigned int DataPort = 1;  //nao alterar

void setup() {

  //initialize uart 1
  Serial.begin(115200);

  //initialize uart 2
  Serial2.begin(115200);
  Serial2.setTimeout(50);

  //initialize SD card with FAT file system and EEPROM
  while (!SD_Initialize()) {
    Serial.println("Card Mount Failed");
  }

  //Connect wifi to AP
  WiFi.begin(ssid, password);
  

}

void loop()
{
    uint32_t t = millis();
    if (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.println("Aguardando Conexão Wifi!");
    if ( (millis() - t) > 4000 )  //if not connect try again in 4sec
    {
      Serial.println("Tentando conectar ao Wifi");
      WiFi.mode(WIFI_OFF);        //stop wifi
      WiFi.begin(ssid, password);
      t = millis();
    }
    return;
  }
  else
    {
      //print wifi information
      Serial.println("WiFi conectado!");
      Serial.println("Endereço IP: ");
      Serial.println(WiFi.localIP());
    }
  //mainApp();
  //TestCase1();
  //TestCase2();
  //TestCase3();
  //TestCase4();
  AppAlfaTR();
}

void AppAlfaTR()
{
  uint8_t buf[500];
  uint32_t ret = 0;
  
  readEEPROMData();     //read eeprom data
  
  if (!connectToFTPServer())  //connect to ftp server
  {
    Serial.println("FTP connect Failed");
    return;
  }

  if (!loginFTPServer("manfrim", "admin123")) //login to ftp server
  {
    Serial.println("Login Failed");
    return;
  }

  Serial.println("Arquivos no diretório gravacoes:");
  SD_ListDir("/gravacoes",1);
}


void mainApp()
{
  uint8_t buf[500];
  uint32_t ret = 0;
  
  readEEPROMData();     //read eeprom data
  
  if (!connectToFTPServer())  //connect to ftp server
  {
    Serial.println("FTP connect Failed");
    return;
  }

  if (!loginFTPServer("admin", "651151")) //login to ftp server
  {
    Serial.println("Login Failed");
    return;
  }

  if (downloadFileFromFTP("/test.txt")) //download file from ftp server
  {
    while (1)
    {
      if (isButtonPressed(BOTTON_1))  //read 500 bytes from file
      {
        ret = SD_ReadFile(readFilePointer, buf, 500, "/test1.txt");
        Serial.println((String)"Ret " + ret);
        writeEEPROMData();  //save readfilepointer
      }
      else if (isButtonPressed(BOTTON_2)) //send 500 bytes on uart2
      {
        Serial.println("Sending Data To uart");
        Serial2.write(buf, ret);
        ret = 0;
      }

      if (Serial2.available() > 0)  //read if data received on uart2
      {
        unsigned int led = -1;
        unsigned int val = 0;
        if (8 >= Serial2.readBytes(buf, 8)) //read 8 bytes only
        {
          sscanf((char*)buf, "LED%u-%u", &led, &val);   //command format - "LED1-0" ,"LED1-1"
          Serial.printf("LED - %d , Val - %d\n", led, val); //print LED value
          setLed(led, val); // ON/OFF LED
        }
      }
    }
  }
}

void TestCase1()
{
  if (SD_Detect())
  {
    if (!connectToFTPServer())
    {
      Serial.println("Connect ftp Failed");
      return;
    }
    if (true != loginFTPServer("manfrim", "admin123"))
    {
      Serial.println("Login Failed");
      return;
    }
    downloadFileFromFTP("/test.txt");  //dont use capital extentions for file eg. test.BMP

    Serial.printf("File Size %lld\n", Get_File_Size("/test.txt"));
    //wait forever
    //while (1);
  }
  delay(1000);
}

void TestCase2()
{
  uint8_t buf[500];
  while (1)
  {
    //if (SD_Detect())
    {
      if (isButtonPressed(BOTTON_1))
      {
        uint32_t ret = SD_ReadFile(readFilePointer, buf, 500, "/test1.txt");
        Serial.println((String)"Ret " + ret);
        Serial2.write(buf, ret);
        if (fileReadFlag)
        {
          fileReadFlag = 0;
          Serial.println("------------File End Here------------");
        }
      }
      if (isButtonPressed(BOTTON_2))
      {
        Serial.println("Button 2 Pressed ");
      }
    }
  }
}

void TestCase3()
{
  uint8_t buf[500];
  while (1)
  {
    //if (SD_Detect())
    {
      uint32_t ret = SD_ReadFile(5158633, buf, 500, "/test1.txt");
      while ( (ret = SD_ReadFile(readFilePointer, buf, 500, "/test1.txt") ))
      {
        Serial.println((String)"Ret " + ret);
        Serial2.write(buf, ret);
        if (fileReadFlag)
        {
          fileReadFlag = 0;
          Serial.println("------------File End Here------------");
          break;
        }
      }
      //stop here
      while (1);
    }
  }
}

void TestCase4()
{
  readEEPROMData();     //read eeprom data
  Serial.println("Read after restart from EEPROM");
  Serial.printf("readFilePointer-%ld, writeFilePointer-%ld, epVariable-%u\n", (long int)readFilePointer, (long int)writeFilePointer, epVariable);

  while (1)
  {
    if (isButtonPressed(BOTTON_1))
    {
      readFilePointer += 1;
      writeFilePointer += 2;
      epVariable += 3;
      Serial.println("Write to EEPROM");
      Serial.printf("readFilePointer-%ld, writeFilePointer-%ld, epVariable-%u\n", (long int)readFilePointer, (long int)writeFilePointer, epVariable);
      writeEEPROMData();
    }
  }
}
