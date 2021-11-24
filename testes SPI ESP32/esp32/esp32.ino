#include <WiFi.h>
#include <WiFiClient.h>
#include <ESP32_FTPClient.h>
#include <ESP32DMASPISlave.h>
#include "CRC16.h"
#include "sample.h"

#define ESP32_INPUT 27
#define ESP32_OUTPUT 28

// SPI
ESP32DMASPI::Slave slave;
static const uint32_t BUFFER_SIZE = 4096;
uint8_t* spi_slave_tx_buf;
uint8_t* spi_slave_rx_buf;

// FTP
char ftp_server[] = "192.168.0.146";
char ftp_user[]   = "manfrim";
char ftp_pass[]   = "admin123";
// you can pass a FTP timeout and debbug mode on the last 2 arguments
ESP32_FTPClient ftp(ftp_server, ftp_user, ftp_pass, 5000, 1); // Disable Debug to increase Tx Speed


bool FLUENCY = false; // variavel para controlar se a gravacao vai ser do ID ou da fluencia, caso seja true será a fluencia

enum estado {
  pronto,
  iniciar_gravacao,
  gravando,
  terminar_gravacao,
  iniciar_transmissao_gravacoes,
  conectar_wifi,
  aguardando_wifi,
  wifi_conectado,
  conectar_ftp,
  enviar_gravacoes,
  enviando_gravacoes,
  desligar_wifi,
  envia_comando_play,
  falha
};

const char* nomesEstado[] = {
  "pronto",
  "iniciar_gravacao",
  "gravando",
  "terminar_gravacao",
  "iniciar_transmissao_gravacoes",
  "conectar_wifi",
  "aguardando_wifi",
  "wifi_conectado",
  "conectar_ftp",
  "enviar_gravacoes",
  "enviando_gravacoes",
  "desligar_wifi",
  "envia_comando_play",
  "falha"
};

enum maquinaEstadoGERAL {
  SHUTDOWN,
  SM_READY,
  SM_RECORD_ID,
  SM_RECORD_FLUENCY,
  SM_HARD_FAULT,
  SM_CONNECT,
  SM_SEND,
  SM_PLAY
};

const char* nomesMaquinaEstadoGERAL[] = {
  "SHUTDOWN",
  "SM_READY",
  "SM_RECORD_ID",
  "SM_RECORD_FLUENCY",
  "SM_HARD_FAULT",
  "SM_CONNECT",
  "SM_SEND",
  "SM_PLAY"
};

int estadoAtual = pronto;

// MÉTODO QUE MONTA MSGS DE COMANDO PARA STM
void mountCommand(byte command, uint8_t (& commandBuffer)[9]) {
  //Header 'ALFA'0x41, 0x4c, 0x46, 0x41
  //size = 01 uint16 | 0x00, 0x01
  //payload com o comando, lista de comandos:
  //            - 1 SM_READY, CRC: 947E
  //            - 2 SM_RECORD_ID, CRC: 953E
  //            - 3 SM_RECORD_FLUENCY, CRC: 55FF
  //            - 6 SM_SEND, CRC: 563F
  //            - 7 SM_PLAY, CRC: 96FE
  // CRC16
  commandBuffer[0] = 0x41;
  commandBuffer[1] = 0x4c;
  commandBuffer[2] = 0x46;
  commandBuffer[3] = 0x41;
  commandBuffer[4] = 0x00;
  commandBuffer[5] = 0x01;
  commandBuffer[6] = command;

  //monta o CRC, TODO: implementar método que calcula CRC16 modbus e corrigir aqui para que nao fique fixo
  switch (command) {
    case (0x01):
      commandBuffer[7] = 0x94;
      commandBuffer[8] = 0x7E;
      break;
    case (0x02):
      commandBuffer[7] = 0x95;
      commandBuffer[8] = 0x3E;
      break;
    case (0x03):
      commandBuffer[7] = 0x55;
      commandBuffer[8] = 0xFF;
      break;
    case (0x06):
      commandBuffer[7] = 0x56;
      commandBuffer[8] = 0x3F;
      break;
    case (0x07):
      commandBuffer[7] = 0x96;
      commandBuffer[8] = 0xFE;
      break;
  }
}

//MÉTODO PARA ENVIAR UM COMANDO PARA STM
bool sendCommand(byte command) {
  //verificar se pode enviar para stm antes de enviar
  //if (ESP32_INPUT == LOW)
  if (true)
  {
    uint8_t commandBuffer[9] = {0, 0, 0, 0, 0, 0, 0, 0, 0};
    mountCommand(command, commandBuffer);
    Serial.println("Enviando comando para stm: ");
    Serial.println(nomesMaquinaEstadoGERAL[command]);
    for (int i = 0; i < 9; i++)
    {
      Serial.print(" ");
      Serial.print(commandBuffer[i], HEX);
    }
    Serial.println();
    return true;
  }
  else return false;
}

// MÉTODO PARA LER DADOS SPI
void readSPI() {
  // if there is no transaction in queue, add transaction
  if (slave.remained() == 0)
    slave.queue(spi_slave_rx_buf, spi_slave_tx_buf, BUFFER_SIZE);

  // if transaction has completed from master,
  // available() returns size of results of transaction,
  // and buffer is automatically updated

  while (slave.available()) {
    // do something here with received data
    for (size_t i = 0; i < 66; ++i) {
      Serial.print(spi_slave_rx_buf[i], HEX);
      Serial.print(" ");
    }
    Serial.println();
    Serial.println("============================================================");
    slave.pop();
  }
}

// MÉTODO PARA CONECTAR WIFI
void connect2Wifi() {
  Serial.println("Conectando Wifi...");
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  Serial.println("");
  Serial.print("Conectado! IP esp32: ");
  Serial.println(WiFi.localIP());
}

// MÉTODO QUE INICIALIZA ESP32
void setup() {

  // pino para verificar se esp32 pode enviar para stm
  pinMode(ESP32_INPUT, INPUT);

  // pino para informa stm que pode enviar para esp32
  pinMode(ESP32_OUTPUT, OUTPUT);

  // inicializa serial para receber comandos
  Serial.begin(115200);

  // iniciliza buffer conexao SPI com STM
  spi_slave_tx_buf = slave.allocDMABuffer(BUFFER_SIZE);
  spi_slave_rx_buf = slave.allocDMABuffer(BUFFER_SIZE);

  // configurando parametros da conexao SPI
  slave.setDataMode(SPI_MODE0);
  slave.setMaxTransferSize(BUFFER_SIZE);
  slave.setDMAChannel(2); // 1 or 2 only
  slave.setQueueSize(1); // transaction queue size

  // begin() after setting
  // HSPI = CS: 15, CLK: 14, MOSI: 13, MISO: 12
  // VSPI = CS: 5, CLK: 18, MOSI: 23, MISO: 19
  slave.begin(HSPI);
  Serial.println("SPI Inicializada");

  // desliga Wifi e Bluetooth do esp32
  WiFi.mode(WIFI_OFF);
  btStop();
}

// MÉTODO LOOP COM A PRINCIPAL LÓGICA DA ESP32
void loop() {
  int estadoAnterior = estadoAtual;
  //=======================================================
  // INTERPRETA IHM (ATUALMENTE ESTA NA SERIAL LENDO ASCII)
  //=======================================================
  if (Serial.available() > 0) {
    // lê do buffer o dado recebido:
    String comando = Serial.readString();

    Serial.print("Comando reebido: ");
    Serial.println(comando);

    if (comando == "gravarid\n") {
      if (estadoAtual == pronto) {
        FLUENCY = false;
        estadoAtual = iniciar_gravacao;
      }
      else
        Serial.print("Dispositivo não está pronto para gravação, estado atual: ");
      Serial.println(nomesEstado[estadoAtual]);
    }
    else if (comando == "gravarfluencia\n") {
      if (estadoAtual == pronto) {
        FLUENCY = true;
        estadoAtual = iniciar_gravacao;
      }
      else
        Serial.print("Dispositivo não está pronto para gravação, estado atual: ");
      Serial.println(nomesEstado[estadoAtual]);
    }
    else if (comando == "terminar\n") {
      if (estadoAtual == gravando)
        estadoAtual = terminar_gravacao;
      else
        Serial.print("Dispositivo não está pronto para terminar gravação, estado atual: ");
      Serial.println(nomesEstado[estadoAtual]);
    }
    else if (comando == "enviar\n") {
      if (estadoAtual == pronto) {
        estadoAtual = enviar_gravacoes;
      }
      else
        Serial.print("Dispositivo não está pronto para parar enviar gravações, estado atual: ");
      Serial.println(nomesEstado[estadoAtual]);
    }
    else if (comando == "play\n") {
      if (estadoAtual == pronto) {
        estadoAtual = envia_comando_play;
      }
      else
        Serial.print("Dispositivo não está pronto para parar enviar gravações, estado atual: ");
      Serial.println(nomesEstado[estadoAtual]);
    }
    else {
      Serial.println("Comando não reconhecido!");
    }
  }
  //=======================================================
  // FIM DA INTERPRETACAO DE COMANDOS
  //=======================================================

  if (estadoAtual == pronto) {
    //INDICAR PARA STM QUE PODE ENVIAR INFORMACAO
    digitalWrite(ESP32_OUTPUT, HIGH);
  } else {
    //INDICAR PARA STM QUE NÃOPODE ENVIAR INFORMACAO
    digitalWrite(ESP32_OUTPUT, LOW);
  }

  //=======================================================
  // INICIO DA MAQUINA DE ESTADOS
  //=======================================================
  switch (estadoAtual) {
    case pronto:
      readSPI();
      break;

    case iniciar_gravacao:
      //ENVIAR PARA NUCLEO INICIAR GRAVACAO COM A VARIAVARIAVEL FLUENCY
      if (!FLUENCY) {
        if (sendCommand(SM_RECORD_ID))
          estadoAtual = gravando;
      }
      else {
        if (sendCommand(SM_RECORD_FLUENCY))
          estadoAtual = gravando;
      }

      break;

    case gravando:
      // Não faz nada (stm está realizando a gravacao de id ou fluencia no cartao sd)
      break;

    case terminar_gravacao:
      //ENVIAR PARA NUCLEO PARAR GRAVACAO
      if (sendCommand(SM_READY))
        estadoAtual = pronto;
      break;

    case enviar_gravacoes:
      estadoAtual = conectar_wifi;
      break;

    case conectar_wifi:
      Serial.println("Aguardando Wifi");
      WiFi.mode(WIFI_STA);
      WiFi.begin("35_NET_2G", "Ap35Manfrim");
      estadoAtual = aguardando_wifi;

      break;

    case aguardando_wifi:
      if (WiFi.status() == WL_CONNECTED)
      {
        estadoAtual = conectar_ftp;
      }
      break;

    case conectar_ftp:
      if (!ftp.isConnected()) {
        ftp.OpenConnection();
      } else {
        estadoAtual = iniciar_transmissao_gravacoes;
      }
      break;

    case iniciar_transmissao_gravacoes:
      //ENVIAR COMANDO PARA STM ENVIAR ARQUIVOS PARA ESP32
      if (sendCommand(SM_SEND))
        estadoAtual = enviando_gravacoes;

    case enviando_gravacoes:
      readSPI();
      //TODO: LER BUFFER DO SPI E ENVIAR PARA FTP
      ftp.InitFile("Type A");
      ftp.NewFile("ESP32_XPTO_TESTE.wav");

      Serial.println("Enviando arquivo: ESP32_XPTO_TESTE.wav");
      Serial.println("inicio da transmissão");
      Serial.println("enviando parte 1/3");
      ftp.WriteData(wav_part1, sizeof(wav_part1));
      Serial.println("enviando parte 2/3");
      ftp.WriteData(wav_part2, sizeof(wav_part2));
      Serial.println("enviando parte 3/3");
      ftp.WriteData(wav_part3, sizeof(wav_part3));

      Serial.println("Arquivo enviado!");

      ftp.CloseFile();      
      ftp.CloseConnection();

      {
        estadoAtual = desligar_wifi;
      }
      break;

    case desligar_wifi:
      WiFi.disconnect(true);
      WiFi.mode(WIFI_OFF);
      if (WiFi.status() != WL_CONNECTED) {
        estadoAtual = pronto;
      }
      break;

    case envia_comando_play:
      sendCommand(SM_PLAY);
      estadoAtual = pronto;
      break;

    case falha:
      // comando(falha)
      break;

    default:
      Serial.println("ESTADO DESCONHECIDO!");
      estadoAtual = pronto;      
      break;
  }
  //=======================================================
  // FIM DA MAQUINA DE ESTADOS
  //=======================================================
  if (estadoAnterior != estadoAtual)
  {
    Serial.println("Novo estado:");
    Serial.println(nomesEstado[estadoAtual]);
  }
}
