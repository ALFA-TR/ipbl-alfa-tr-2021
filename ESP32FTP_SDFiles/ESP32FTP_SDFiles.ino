#include <SPI.h>
#include <SD.h>
#include "SD_card_helper.h"

#include <WiFi.h> 
#include <WiFiClient.h> 
#include <ESP32_FTPClient.h>
#include "octocat.h" 
#include "Secrets.h"


//!!DONT FORGET TO UPDATE Secrets.h with your WIFI Credentials!!
char ftp_server[] = "192.168.0.146";
char ftp_user[]   = "manfrim";
char ftp_pass[]   = "admin123";


// you can pass a FTP timeout and debbug mode on the last 2 arguments
ESP32_FTPClient ftp (ftp_server,ftp_user,ftp_pass, 5000, 0); // Disable Debug to increase Tx Speed

void setup()
{
  Serial.begin( 115200 );

  WiFi.begin( WIFI_SSID, WIFI_PASS );
  
  Serial.println("Connecting Wifi...");
  while (WiFi.status() != WL_CONNECTED) {
      delay(500);
      Serial.print(".");
  }
  Serial.println("");
  Serial.print("IP address: ");
  Serial.println(WiFi.localIP());

   //initialize SD card with FAT file system and EEPROM
  while (!SD_Initialize()) {
    Serial.println("Card Mount Failed");
  }
  
  ftp.OpenConnection();
  
  sendFile2FTP("/gravacoes/teste.txt", ftp);
  ftp.CloseConnection();
}

void loop()
{

}

void sendFile2FTP(const char* filePath, ESP32_FTPClient ftpClient) {
    ftpClient.InitFile("Type I");
    ftpClient.NewFile(filePath);    
    Serial.printf("Reading file: %s\n", filePath);

    File file = SD.open(filePath, FILE_READ);
    if (!file) {
        Serial.println("Failed to open file for reading");
        return;
    }

    Serial.print("Read from file: ");
    
    while (file.available()) {
        unsigned char buf[1024];
        int readVal = file.read(buf, sizeof(buf));
        Serial.printf("%d",readVal);
        ftpClient.WriteData(buf,sizeof(buf));
    }
    Serial.println("Terminou");
    ftpClient.CloseFile();
    file.close();
}
