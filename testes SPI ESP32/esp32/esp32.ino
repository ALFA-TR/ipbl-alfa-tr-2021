#include <WiFi.h>
#include <WiFiClient.h>
#include <ESP32_FTPClient.h>
#include <ESP32DMASPISlave.h>

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
ESP32_FTPClient ftp (ftp_server, ftp_user, ftp_pass, 5000, 0); // Disable Debug to increase Tx Speed


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
  comandoSTM_enviar_arquivos,
  enviar_gravacoes,
  enviando_gravacoes,
  desligar_wifi,
  envia_comando_play,
  falha
};

int estadoAtual = pronto;

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
  //=======================================================
  // INTERPRETA IHM (ATUALMENTE ESTA NA SERIAL LENDO ASCII)
  //=======================================================
  if (Serial.available() > 0) {
    // lê do buffer o dado recebido:
    String comando = Serial.readString();

    Serial.print("Comando reebido: ");
    Serial.println(comando);

    if (comando == "gravarid") {
      if (estadoAtual == pronto) {
        FLUENCY = false;
        estadoAtual = iniciar_gravacao;
      }
      else
        Serial.println("Dispositivo não está pronto para gravação, estado atual: " + String(estadoAtual));
    }
    if (comando == "gravarfluencia") {
      if (estadoAtual == pronto) {
        FLUENCY = true;
        estadoAtual = iniciar_gravacao;
      }
      else
        Serial.println("Dispositivo não está pronto para gravação, estado atual: " + String(estadoAtual));
    }
    else if (comando == "terminar") {
      if (estadoAtual = gravando)
        estadoAtual = terminar_gravacao;
      else
        Serial.println("Dispositivo não está pronto para terminar gravação, estado atual: " + String(estadoAtual));
    }
    else if (comando == "enviar") {
      if (estadoAtual = pronto)
        estadoAtual = iniciar_transmissao_gravacoes;
      else
        Serial.println("Dispositivo não está pronto para parar enviar gravações, estado atual: " + String(estadoAtual));
    }
    else if (comando == "play") {
      if (estadoAtual = pronto)
        estadoAtual = envia_comando_play;
      else
        Serial.println("Dispositivo não está pronto para parar enviar gravações, estado atual: " + String(estadoAtual));
    }
    else {
      Serial.println("Comando não reconhecido!");
    }
  }
  //=======================================================
  // FIM DA INTERPRETACAO DE COMANDOS
  //=======================================================

  //=======================================================
  // INICIO DA MAQUINA DE ESTADOS
  //=======================================================
  switch (estadoAtual) {
    case pronto:
      // TODO: INDICAR PARA STM QUE PODE ENVIAR INFORMACAO
      readSPI();
      break;

    case iniciar_gravacao:
      // TODO: ENVIAR PARA NUCLEO INICIAR GRAVACAO COM A VARIAVARIAVEL FLUENCY
      estadoAtual = gravando;
      break;

    case gravando:
      // Não faz nada (stm está realizando a gravacao de id ou fluencia no cartao sd)
      break;

    case terminar_gravacao:
      // TODO: ENVIAR PARA NUCLEO PARAR GRAVACAO
      estadoAtual = pronto;
      break;

    case enviar_gravacoes:
      estadoAtual = conectar_wifi;
      break;

    case conectar_wifi:
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
        estadoAtual = enviando_gravacoes;
      }
      break;

    case comandoSTM_enviar_arquivos:
      // TODO: ENVIAR COMANDO PARA STM ENVIAR ARQUIVOS PARA ESP32
      estadoAtual = enviando_gravacoes;
      break;

    case enviando_gravacoes:
      readSPI();
      //TODO: LER BUFFER DO SPI E ENVIAR PARA FTP
      ftp.InitFile("Type A");
      ftp.NewFile("ARQUIVO_AUDIO.wav");

      Serial.println("Enviando arquivo: ARQUIVO_AUDIO.wav");
      Serial.println("inicio da transmissão");

      ftp.WriteData(spi_slave_rx_buf, 1024);

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

    case falha:
      // comando(falha)
      break;

    default:
      Serial.println("ESTADO DESCONHECIDO!");
      break;
  }
  //=======================================================
  // FIM DA MAQUINA DE ESTADOS
  //=======================================================
}
