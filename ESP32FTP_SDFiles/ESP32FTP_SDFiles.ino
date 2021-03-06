#include <SPI.h>
#include <SD.h>
#include "SD_card_helper.h"

#include <WiFi.h> 
#include <WiFiClient.h> 
#include <ESP32_FTPClient.h>
#include "octocat.h" 
#include "Secrets.h"

#include <LiquidCrystal.h>

char ftp_server[] = "192.168.0.146";
char ftp_user[]   = "manfrim";
char ftp_pass[]   = "admin123";


// you can pass a FTP timeout and debbug mode on the last 2 arguments
ESP32_FTPClient ftp (ftp_server,ftp_user,ftp_pass, 5000, 0); // Disable Debug to increase Tx Speed

// Create An LCD Object. Signals: [ RS, EN, D4, D5, D6, D7 ]
LiquidCrystal My_LCD(13, 12, 14, 27, 26, 25);

void setup()
{  

  My_LCD.begin(16, 2);
  My_LCD.clear();
  
  Serial.begin( 115200 );
  WiFi.begin( WIFI_SSID, WIFI_PASS );
  
  bool rodarTestes = false;
  
  if(rodarTestes) 
  {
  
    // CASOS DE TESTE US 110
    testeCaseUS110_1();
    testeCaseUS110_2();
    testeCaseUS110_3();

    // CASOS DE TESTE US 128
    testeCaseUS128_1();
    testeCaseUS128_2();
    testeCaseUS128_3();

    // CASOS DE TESTE US 129
    testeCaseUS129_1();
    testeCaseUS129_2();
    testeCaseUS129_3();

    // CASOS DE TESTE US 130
    testeCaseUS130_1();
    testeCaseUS130_2();
    testeCaseUS130_3();

    Serial.println("FIM DOS CASOS DE TESTES");
  }
  else
  {
    WiFi.begin( WIFI_SSID, WIFI_PASS );  
    My_LCD.print("Conectando Wifi!");
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
    Serial.println("inicio da transmiss??o");
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
    Serial.println("Meta: Teste de conex??o com o Server");
    connect2Wifi();
    mountSdCard();  
    ftp.OpenConnection();  
    Serial.println("Conectado ao servidor ftp!");
    ftp.CloseConnection();  
    Serial.println("------------fim do teste------------\n\n");
    delay(10000);
}

void testeCaseUS110_2(){
  Serial.println("------------testeCaseUS110_2------------");
  Serial.println("Meta: Teste de transmiss??o de dados");
  connect2Wifi();
  mountSdCard();  
  ftp.OpenConnection();  
  Serial.println("Enviando arquivo para servidor!");
  sendFile2FTP("/gravacoes/teste.txt", ftp);
  ftp.CloseConnection();
  Serial.println("------------fim do teste------------\n\n");
  delay(10000);
}


void testeCaseUS110_3(){
  Serial.println("------------testeCaseUS110_3------------");
  Serial.println("Meta: Teste de recep????o de dados");
  connect2Wifi();
  mountSdCard();  
  ftp.OpenConnection();  
  Serial.println("Baixando arquivo testeDeDownload.txt do servidor!");
  String response = "";
  ftp.InitFile("Type A");
  ftp.DownloadString("testeDeDownload.txt", response);
  Serial.println("O conteudo do arquivo baixado ??: \n" + response);
  ftp.CloseConnection();
  Serial.println("------------fim do teste------------\n\n");
  delay(10000);
}


//===============================================================
//                CASOS DE TESTE US 128
//===============================================================

void testeCaseUS128_1(){
  Serial.println("------------testeCaseUS128_1------------");
  Serial.println("Meta: Teste do m??dulo leitor de cart??o SD");  
  Serial.println("Iniciando m??dulo de cart??o SD!");
  mountSdCard();  
  Serial.println("M??dulo de cart??o SD reconhecido!");
  Serial.println("------------fim do teste------------\n\n");
  delay(10000);
}

void testeCaseUS128_2(){
  Serial.println("------------testeCaseUS128_2------------");
  Serial.println("Meta: Teste do Cart??o Micro SD 4GB (ler arquivos)");  
  Serial.println("Iniciando m??dulo de cart??o SD!");
  mountSdCard();  
  Serial.println("M??dulo de cart??o SD reconhecido!");
  delay(2000);
  Serial.println("/nDetectando Cart??o SD inserido no m??dulo");
  SD_Detect();
  delay(2000);
  Serial.println("/n Listando arquivos no cartao SD:");
  SD_ListDir("/",2);
  Serial.println("------------fim do teste------------\n\n");
  delay(10000);
}

void testeCaseUS128_3(){
  Serial.println("------------testeCaseUS128_3------------");
  Serial.println("Meta: Ligar o dispositivo sem cart??o SD");  
  Serial.println("ATEN????O, nos pr??ximos 20 segundos voc?? DEVE REMOVER o cartao SD ao dispositivo!\n"); 
    for (int i = 0; i <= 20; i++) {
    Serial.print(".");
    delay(1000);
  }
  Serial.println("Fim dos 20 segundos!");
  Serial.println("Lendo cart??o SD!");
  SD_Detect();  


  Serial.println("\n\nATEN????O, nos pr??ximos 20 segundos voc?? DEVE CONECTAR o cartao SD ao dispositivo!\n"); 
    for (int i = 0; i <= 20; i++) {
    Serial.print(".");
    delay(1000);
  }
  Serial.println("Fim dos 20 segundos!");
  
  Serial.println("------------fim do teste------------\n\n");
  delay(10000);
}


//===============================================================
//                CASOS DE TESTE US 129
//===============================================================

void testeCaseUS129_1(){
  Serial.println("------------testeCaseUS129_1------------");
  Serial.println("Meta: Salvar um arquivo FAT");  
  Serial.println("Iniciando m??dulo de cart??o SD!");
  mountSdCard(); 
  delay(2000);
  Serial.println("Listando arquivos na pasta testes do cartao SD:");
  SD_ListDir("/testes",1);
  delay(2000);
  Serial.println("Criando arquivo ARQUIVO_CASO_DE_TESTE_129.txt");
  SD_mkFile("/testes/ARQUIVO_CASO_DE_TESTE_129.txt");
  uint8_t buff[100] = "teste de arquivo";
  SD_WriteFile(buff,100,"/testes/ARQUIVO_CASO_DE_TESTE_129.txt");
  Serial.println("Arquivo Criado!");
  delay(2000);
  Serial.println("Listando arquivos na pasta testes do cartao SD:");
  SD_ListDir("/testes",1);
  Serial.println("------------fim do teste------------\n\n");
  delay(10000);
}

void testeCaseUS129_2(){
  Serial.println("------------testeCaseUS129_2------------");
  Serial.println("Meta: Ler um arquivo FAT");  
  Serial.println("Lendo arquivo ARQUIVO_CASO_DE_TESTE_129.txt na pasta testes");
  Serial.println("Conteudo:\n");
  uint8_t buff[100];
  File teste;
  SD_ReadFile(teste,buff,100,"/testes/ARQUIVO_CASO_DE_TESTE_129.txt");
  Serial.println((char*)buff);
  Serial.println("------------fim do teste------------\n\n");
  delay(10000);
}

void testeCaseUS129_3(){
  Serial.println("------------testeCaseUS129_3------------");
  Serial.println("Meta: Excluir um arquivo FAT");  
  Serial.println("Iniciando m??dulo de cart??o SD!");
  mountSdCard(); 
  delay(2000);
  Serial.println("Listando arquivos na pasta testes do cartao SD:");
  SD_ListDir("/testes",1);
  delay(2000);
  Serial.println("Removendo arquivo ARQUIVO_CASO_DE_TESTE_129.txt");
  SD_rmFile("/testes/ARQUIVO_CASO_DE_TESTE_129.txt");
  Serial.println("Arquivo Removido!");
  delay(2000);
  Serial.println("Listando arquivos na pasta testes do cartao SD:");
  SD_ListDir("/testes",1);
  Serial.println("------------fim do teste------------\n\n");
  delay(10000);
}


//===============================================================
//                CASOS DE TESTE US 130
//===============================================================

void testeCaseUS130_1(){
  Serial.println("------------testeCaseUS130_1------------");
  Serial.println("Meta: Teste de comunica????o via r??dio 2,4 GH");  
  connect2Wifi();
  Serial.println("Conex??o Wifi est?? OK!");  
  Serial.println("------------fim do teste------------\n\n");
  delay(10000);
}

void testeCaseUS130_2(){
  Serial.println("------------testeCaseUS130_2------------");
  Serial.println("Meta: Teste de ping por meio do protocolo TCP/IP");  
  Serial.println("Iniciando Conex??o com Wifi!"); 
  connect2Wifi();
  Serial.println("ATEN????O, nos pr??ximos 20 segundos voc?? DEVE realizar um teste de ping no ip do ESP32 informado anteriormente!\n"); 
    for (int i = 0; i <= 20; i++) {
    Serial.print(".");
    delay(1000);
    }
    Serial.println("Fim dos 20 segundos!");
  Serial.println("------------fim do teste------------\n\n");
  delay(10000);
}

void testeCaseUS130_3(){
  Serial.println("------------testeCaseUS130_3------------");
  Serial.println("Meta: Teste de Download do servidor FTP");  
  connect2Wifi();
  mountSdCard();  
  ftp.OpenConnection();  
  Serial.println("Baixando arquivo testeDeDownload.txt do servidor!");
  String response = "";
  ftp.InitFile("Type A");
  ftp.DownloadString("testeDeDownload.txt", response);
  Serial.println("O conteudo do arquivo baixado ??: \n" + response);
  ftp.CloseConnection();
  Serial.println("------------fim do teste------------\n\n");
  delay(10000);
}
