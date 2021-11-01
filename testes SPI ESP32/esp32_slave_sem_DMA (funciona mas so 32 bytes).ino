#include <ESP32SPISlave.h>

ESP32SPISlave slave;

static constexpr uint32_t BUFFER_SIZE {32};
uint8_t spi_slave_tx_buf[BUFFER_SIZE];
uint8_t spi_slave_rx_buf[BUFFER_SIZE];

void setup() {
    Serial.begin(115200);
    // HSPI = CS: 15, CLK: 14, MOSI: 13, MISO: 12
    // VSPI = CS: 5, CLK: 18, MOSI: 23, MISO: 19
    slave.setDataMode(SPI_MODE0);
    slave.setQueueSize(1);  // transaction queue size
    slave.begin(VSPI);
}

void loop() {
    // block until the transaction comes from master
    slave.wait(spi_slave_rx_buf, spi_slave_tx_buf, BUFFER_SIZE);

    // if transaction has completed from master,
    // available() returns size of results of transaction,
    // and `spi_slave_rx_buf` is automatically updated
    while (slave.available()) {
        // do something with `spi_slave_rx_buf`
        for (int i = 0; i < 32; i++){
          Serial.print(spi_slave_rx_buf[i], HEX);
          Serial.print(" ");
        }
        Serial.println();

        slave.pop();
    }
}