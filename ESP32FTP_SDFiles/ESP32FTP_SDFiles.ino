#include <SPI.h>
#include <SD.h>
#include "SD_card_helper.h"

#include <WiFi.h> 
#include <WiFiClient.h> 
#include <ESP32_FTPClient.h>
#include "octocat.h" 
#include "Secrets.h"


char ftp_server[] = "192.168.0.146";
char ftp_user[]   = "manfrim";
char ftp_pass[]   = "admin123";


// you can pass a FTP timeout and debbug mode on the last 2 arguments
ESP32_FTPClient ftp (ftp_server,ftp_user,ftp_pass, 5000, 0); // Disable Debug to increase Tx Speed

void setup()
{  
  Serial.begin( 115200 );
  WiFi.begin( WIFI_SSID, WIFI_PASS );
  
  bool rodarTestes = true;
  
  if(rodarTestes) 
  {
  
    // CASOS DE TESTE US 110
    delay(5000);
    testeCaseUS110_1();
    testeCaseUS110_2();
    testeCaseUS110_3();

    // CASOS DE TESTE US 128
    delay(5000);
    testeCaseUS128_1();
    testeCaseUS128_2();
    testeCaseUS128_3();

    // CASOS DE TESTE US 129
    delay(5000);
    testeCaseUS129_1();
    testeCaseUS129_2();
    testeCaseUS129_3();

    // CASOS DE TESTE US 130
    delay(5000);
    testeCaseUS130_1();
    testeCaseUS130_2();
    testeCaseUS130_3();
  }
  else
  {
    WiFi.begin( WIFI_SSID, WIFI_PASS );  
    connect2Wifi();
    mountSdCard();  
    ftp.OpenConnection();  
    sendFile2FTP("/gravacoes/teste.txt", ftp);
    ftp.CloseConnection();
  }
}

void loop()
{

}

void connect2Wifi(){
  Serial.println("Connecting Wifi...");
  while (WiFi.status() != WL_CONNECTED) {
      delay(500);
      Serial.print(".");
  }
  Serial.println("");
  Serial.print("IP address: ");
  Serial.println(WiFi.localIP());
}

void mountSdCard(){
  //initialize SD card with FAT file system and EEPROM
  while (!SD_Initialize()) {
    Serial.println("Card Mount Failed");
  }
}

void sendFile2FTP(const char* filePath, ESP32_FTPClient ftpClient) {
    ftpClient.InitFile("Type I");
    ftpClient.NewFile(filePath);    
    Serial.printf("Enviando arquivo: %s\n", filePath);

    File file = SD.open(filePath, FILE_READ);
    if (!file) {
        Serial.println("Failed to open file for reading");
        return;
    }
    Serial.println("inicio da transmissão");
    while (file.available()) {
        unsigned char buf[1024];
        int readVal = file.read(buf, sizeof(buf));
        ftpClient.WriteData(buf,sizeof(buf));
    }
    Serial.println("Arquivo enviado!");
    ftpClient.CloseFile();
    file.close();
}

//===============================================================
//                CASOS DE TESTE US 110
//===============================================================
void testeCaseUS110_1(){
    Serial.println("------------testeCaseUS110_1------------");
    Serial.println("Meta: Teste de conexão com o Server");
    connect2Wifi();
    mountSdCard();  
    ftp.OpenConnection();  
    Serial.println("Conectado ao servidor ftp!");
    ftp.CloseConnection();  
    Serial.println("------------fim do teste------------\n\n");
    delay(2000);
}

void testeCaseUS110_2(){
  Serial.println("------------testeCaseUS110_2------------");
  Serial.println("Meta: Teste de transmissão de dados");
  connect2Wifi();
  mountSdCard();  
  ftp.OpenConnection();  
  Serial.println("Enviando arquivo para servidor!");
  sendFile2FTP("/gravacoes/teste.txt", ftp);
  ftp.CloseConnection();
  Serial.println("------------fim do teste------------\n\n");
  delay(2000);
}


void testeCaseUS110_3(){
  Serial.println("------------testeCaseUS110_3------------");
  Serial.println("Meta: Teste de recepção de dados");
  connect2Wifi();
  mountSdCard();  
  ftp.OpenConnection();  
  Serial.println("Baixando arquivo testeDeDownload.txt do servidor!");
  String response = "";
  ftp.InitFile("Type A");
  ftp.DownloadString("testeDeDownload.txt", response);
  Serial.println("O conteudo do arquivo baixado é: \n" + response);
  ftp.CloseConnection();
  Serial.println("------------fim do teste------------\n\n");
  delay(2000);
}


//===============================================================
//                CASOS DE TESTE US 128
//===============================================================

void testeCaseUS128_1(){
  Serial.println("------------testeCaseUS128_1------------");
  Serial.println("Meta: Teste do módulo leitor de cartão SD");  

  Serial.println("------------fim do teste------------\n\n");
  delay(2000);
}

void testeCaseUS128_2(){
  Serial.println("------------testeCaseUS128_2------------");
  Serial.println("Meta: Teste do Cartão Micro SD 4GB (ler arquivos)");  

  Serial.println("------------fim do teste------------\n\n");
  delay(2000);
}

void testeCaseUS128_3(){
  Serial.println("------------testeCaseUS128_3------------");
  Serial.println("Meta: Ligar o dispositivo sem cartão SD");  

  Serial.println("------------fim do teste------------\n\n");
  delay(2000);
}


//===============================================================
//                CASOS DE TESTE US 129
//===============================================================

void testeCaseUS129_1(){
  Serial.println("------------testeCaseUS129_1------------");
  Serial.println("Meta: Salvar um arquivo FAT");  

  Serial.println("------------fim do teste------------\n\n");
  delay(2000);
}

void testeCaseUS129_2(){
  Serial.println("------------testeCaseUS129_2------------");
  Serial.println("Meta: Ler um arquivo FAT");  

  Serial.println("------------fim do teste------------\n\n");
  delay(2000);
}

void testeCaseUS129_3(){
  Serial.println("------------testeCaseUS129_3------------");
  Serial.println("Meta: Excluir um arquivo FAT");  

  Serial.println("------------fim do teste------------\n\n");
  delay(2000);
}


//===============================================================
//                CASOS DE TESTE US 130
//===============================================================

void testeCaseUS130_1(){
  Serial.println("------------testeCaseUS130_1------------");
  Serial.println("Meta: Teste de comunicação via rádio 2,4 GH");  
  connect2Wifi();
  Serial.println("Conexão Wifi está OK!");  
  Serial.println("------------fim do teste------------\n\n");
  delay(2000);
}

void testeCaseUS130_2(){
  Serial.println("------------testeCaseUS130_2------------");
  Serial.println("Meta: Teste de ping por meio do protocolo TCP/IP");  
  Serial.println("Iniciando Conexão com Wifi!"); 
  connect2Wifi();
  Serial.println("ATENÇÃO, nos próximos 60 segundos você DEVE realizar um teste de ping no ip do ESP32 informado anteriormente!\n"); 
    for (int i = 0; i <= 60; i++) {
    Serial.print(".");
    delay(1000);
    }
    Serial.println("Fim dos 60 segundos!");
  Serial.println("------------fim do teste------------\n\n");
  delay(2000);
}

void testeCaseUS130_3(){
  Serial.println("------------testeCaseUS130_3------------");
  Serial.println("Meta: ");  
  connect2Wifi();
  mountSdCard();  
  ftp.OpenConnection();  
  Serial.println("Baixando arquivo testeDeDownload.txt do servidor!");
  String response = "";
  ftp.InitFile("Type A");
  ftp.DownloadString("testeDeDownload.txt", response);
  Serial.println("O conteudo do arquivo baixado é: \n" + response);
  ftp.CloseConnection();
  Serial.println("------------fim do teste------------\n\n");
  delay(2000);
}
