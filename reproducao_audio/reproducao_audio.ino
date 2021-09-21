#include "Arduino.h"
#include "Audio.h"         // da biblioteca https://github.com/schreibfaul1/ESP32-audioI2S
#include "SPI.h"           // para controle do SPI
#include "SD.h"            // para controle do cartão SD
#include "FS.h"            // para controle dos arquivos
#include "esp_task_wdt.h"  // biblioteca para watchdog das tarefas

// Pinos digitais usados para SD/SPI e I2S

#define SD_CS       5
#define SPI_MOSI    23
#define SPI_MISO    19
#define SPI_SCK     18
#define I2S_DOUT    25
#define I2S_BCLK    27
#define I2S_LRC     26

#define ch1 36      // comando para o som 11
#define ch2 39      // comando para o som 12

// Cria os manipuladores das tasks

TaskHandle_t hTask1;

// Variáveis para controle do som a ser emitido

int som = -1;              // para o estado normal de funcionamento, usamos -1 para nenhuma mensagem
boolean executado = true;  // para confirmar o fim da execução do som

// Cria o objeto de áudio

Audio audio;

void setup() {
  // Ajustes das chaves

  pinMode(ch1, INPUT);
  pinMode(ch2, INPUT);

  // Ajustes do SPI

  pinMode(SD_CS, OUTPUT);
  digitalWrite(SD_CS, HIGH);
  SPI.begin(SPI_SCK, SPI_MISO, SPI_MOSI);
  SPI.setFrequency(1000000);

  // Início do Secure Digital Card (cartão SD)

  SD.begin(SD_CS);

  // Início da Serial - para debug

  Serial.begin(115200);

  // Ajuste de áudio I2S

  audio.setPinout(I2S_BCLK, I2S_LRC, I2S_DOUT);
  audio.setVolume(21);

  // Desabilita o WD do core 0. O core 1 já foi desabilitado

  disableCore0WDT();

  // Inicia o WD das tasks
  // esp_task_wdt_init([Intervalo em segundos], [ativa ou desativa o reset])

  esp_task_wdt_init(20, true);

  // Cria as tasks
  // xTaskCreatePinnedToCore (Função, Nome, TamanhoPilha, Parâmetros, Prioridade, Manipuladores, Núcleo)

  xTaskCreatePinnedToCore(task1, "task1", 8192, NULL, 1, &hTask1, 0);  
}

void loop() {
  
  verificaChaves();  // verifica o estado das chaves continuamente
  
}

void verificaChaves() {
  if (executado == false) return;  // se uma mensagem não terminou, não faz nenhuma verificação

  // Verifica o estado de cada chave. Note que as chaves verificadas primeiro têm prioridade

  if (digitalRead(ch1)) {
    som = 11;           // seleciona o som de índice 11
    executado = false;  // indica que este som ainda não foi executado
  }
  else if(digitalRead(ch2)) {
    som = 12;
    executado = false;
  }
}

void task1(void *parameter) {
  // Inscreve uma task no WDT

  esp_task_wdt_add(NULL);

  while(1) {
    if(!audio.isRunning()) {
      switch (som) {  // inicia a seleção do áudio
        case -1:
          break;
        case 11:
          Serial.println("Audio 11");
          delay(500);
          audio.connecttoFS(SD, "/audio11.mp3");
          break;
        case 12:
          Serial.println("Audio 12");
          delay(500);
          audio.connecttoFS(SD, "/audio12.mp3");
          break;
          
        default:  // se o som for inválido, volta para o estado de funcionamento normal
          Serial.println("Solicitação inválida");
          som = -1;
          break;      
      }
    }
    while(audio.isRunning()) { // enquanto estiver em execução, mantenha-se no loop de áudio
       audio.loop();
    }

    executado = true;      // áudio executado
    som = -1;              // define o estado normal de funcionamento 
    esp_task_wdt_reset();  // reinicia o timer do WDT
  }
}
