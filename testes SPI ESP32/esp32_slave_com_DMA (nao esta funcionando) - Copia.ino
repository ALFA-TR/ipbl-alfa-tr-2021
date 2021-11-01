#include <WiFi.h>
#include <WiFiClient.h>
#include <ESP32DMASPISlave.h>
#include <ESP32_FTPClient.h>

char ftp_server[] = "192.168.0.146";
char ftp_user[]   = "manfrim";
char ftp_pass[]   = "admin123";

ESP32_FTPClient ftp(ftp_server, ftp_user, ftp_pass, 5000, 1);


ESP32DMASPI::Slave slave;

static const uint32_t BUFFER_SIZE = 66;
uint8_t* spi_slave_tx_buf;
uint8_t* spi_slave_rx_buf;

void connect2Wifi() {
  Serial.println("Conectando Wifi...");
  WiFi.mode(WIFI_STA);
  WiFi.begin("35_NET_2G", "Ap35Manfrim");
  while (WiFi.status() != WL_CONNECTED) {
    Serial.print('.');
    delay(1000);
  }
  Serial.println(WiFi.localIP());
}


void setup() {
  Serial.begin(115200);

  connect2Wifi();

  Serial.println("Conectando ao servidor FTP...");
  ftp.OpenConnection();
  Serial.println("conectado ftp!");

  //  Serial.println("Arquivo criado!");

  // to use DMA buffer, use these methods to allocate buffer
  spi_slave_tx_buf = slave.allocDMABuffer(BUFFER_SIZE);
  spi_slave_rx_buf = slave.allocDMABuffer(BUFFER_SIZE);

  slave.setDataMode(SPI_MODE0);
  slave.setMaxTransferSize(BUFFER_SIZE);
  slave.setDMAChannel(2); // 1 or 2 only
  slave.setQueueSize(1); // transaction queue size

  // begin() after setting
  // HSPI = CS: 15, CLK: 14, MOSI: 13, MISO: 12
  // VSPI = CS: 5, CLK: 18, MOSI: 23, MISO: 19
  slave.begin(VSPI);
}

void loop() {

  // set buffer (reply to master) data here

  // if there is no transaction in queue, add transaction
  if (slave.remained() == 0)
    slave.queue(spi_slave_rx_buf, spi_slave_tx_buf, BUFFER_SIZE);

  // if transaction has completed from master,
  // available() returns size of results of transaction,
  // and buffer is automatically updated

  while (slave.available()) {
    // do something here with received data
    for (size_t i = 0; i < BUFFER_SIZE; ++i) {
      Serial.print(spi_slave_rx_buf[i], HEX);
      Serial.print(" ");
    }
    printf("\n");

    //    ftp.InitFile("Type A");
    //    ftp.NewFile("TESTE_WAV.wav");
    //
    //    Serial.println("Enviando arquivo: TESTE_WAV.wav");
    //    Serial.println("inicio da transmissão");
    //
    //    ftp.WriteData(spi_slave_rx_buf, BUFFER_SIZE);
    //
    //    Serial.println("Arquivo enviado!");
    //    ftp.CloseFile();
    
    slave.pop();
  }






}