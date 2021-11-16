#include <WiFi.h>
#include <WiFiClient.h>
#include <ESP32_FTPClient.h>

char ftp_server[] = "192.168.1.39";
char ftp_user[]   = "manfrim";
char ftp_pass[]   = "admin123";

enum estado {
  pronto,
  iniciar_gravacao,
  aguardando_nucleo_iniciar_gravacao,
  gravando,
  terminar_gravacao,
  aguardando_nucleo_parar_gravacao,
  conectar_wifi,
  wifi_conectado,
  conectar_ftp,
  enviar_gravacoes,
  enviando_gravacoes,
  desligar_wifi,
  falha
};

int estadoAtual = pronto;
int timeout = 0;

void setup() {
  Serial.begin(115200);
}

void loop() {

  if (timeout > 1000) {
    estadoAtual = falha;
  }

  // interpreta entrada Serial (Botoes ou teclado)
  if (Serial.available() > 0) {
    // lê do buffer o dado recebido:
    String comando = Serial.readString();

    Serial.print("Comando reebido: ");
    Serial.println(comando);

    if (comando == "gravar") {

      if (estadoAtual == pronto)
        estadoAtual = gravando;
      else
        Serial.println("Dispositivo não está pronto para gravação, estado atual: " + String(estadoAtual));

    }
    else if (comando == "parar") {

      if (estadoAtual = gravando)
        estadoAtual = parar;
      else
        Serial.println("Dispositivo não está pronto para parar gravação, estado atual: " + String(estadoAtual));

    }

    else if (comando == "enviar") {
      if (estadoAtual = pronto)
        estadoAtual = enviar_gravacoes;
      else
        Serial.println("Dispositivo não está pronto para parar enviar gravações, estado atual: " + String(estadoAtual));

    }

    else {
      Serial.println("Comando não reconhecido!");
    }
  } // FIM DA INTERPRETACAO DE COMANDOS


  // INICIO DA MAQUINA DE ESTADOS
  switch (estadoAtual) {
    case pronto:
      timeout = 0;
      break;
    case iniciar_gravacao:
      // ENVIAR PARA NUCLEO INICIAR GRAVACAO
      timeout = 0;
      estadoAtual = aguardando_nucleo_iniciar_gravacao;
      break;

    case aguardando_nucleo_iniciar_gravacao:
      timeout++;
      break;
    case gravando:
      // Não faz nada
      break;
    case terminar_gravacao:
      // ENVIAR PARA NUCLEO PARAR GRAVACAO
      timeout = 0;
      estadoAtual = aguardando_nucleo_parar_gravacao;
      break;
    case aguardando_nucleo_parar_gravacao:
      timeout++;
      break;

    case enviar_gravacoes:
      estadoAtual = conectar_wifi;
      break;

    case conectar_wifi:
      timeout = 0;
      WiFi.mode(WIFI_STA);
      WiFi.begin("FUNDACAO1", "senh@123");
      estadoAtual = aguardando_wifi;
      break;
    case aguardando_wifi:
      if (WiFi.status() == WL_CONNECTED)
      {
        estadoAtual = conectar_ftp;
      }
      else {
        timeout++;
      }
      break;

    case conectar_ftp:
      ftp.OpenConnection();
      estadoAtual = enviando_gravacoes;
      break;

    case enviando_gravacoes:
      // implementar envio de gravacoes para servidor ftp
      // if (terminou de enviar)
      {
        estadoAtual = desligar_wifi();
      }


      break;
    case desligar_wifi:
      WiFi.mode(WIFI_OFF);
      if (WiFi.status() != WL_CONNECTED) {
        WiFi.forceSleepBegin();
        estadoAtual = pronto;
      }
      break;
    case falha:
      // comando(falha)
      break;
    default:
      // comando(s)
      break;
  }
  // FIM DA MAQUINA DE ESTADOS
}