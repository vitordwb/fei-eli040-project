// Sensor de corrente SCT e Calculo Potencia Aparente (S)
const int pino_sct = A0; // Pino Analogico ADC
double CorrenteRms = 0;
unsigned long tempoatual;
float corrente = 0;
float correnteRMS=0;
float correnteMax = 0;
float correnteMin = 0;

// Cálculo da Potência Aparente (S) e Ativa (P)
int rede = 127; // Tensão da rede elétrica
double UltimoS = 0;
float energia=0;
unsigned long tempoanterior;
unsigned long deltat;

//rfid
#include <SPI.h>
#include <MFRC522.h>
#define RST_PIN D1 // Pino RST
#define SS_PIN D2  // Pino SDA
MFRC522 mfrc522(SS_PIN, RST_PIN); // Cria a instância MFRC522
unsigned long timer_rfid = millis();
String read_rfid;
String ok_rfid_1 = "49bbe96";
String ok_rfid_2 = "ec0bf79"; // Adicionar quantos cartões forem necessários
int estado_rele = LOW;
const int LedRele = D8;
const int rele = D0;


// servidor
#include <ESP8266WiFi.h>
#include <WiFiClient.h>
#include <ESP8266WebServer.h>
#include "index.h" // Conteudo da pagina HTML com javascript

//SSID e Password do seu roteador WiFi
const char* ssid = "WIFI_NAME"; // Variavel que armazena o nome da rede sem fio
const char* password = "WIFI_PASSWORD"; // Variavel que armazena a senha da rede sem fio

ESP8266WebServer server(80);  // Caso tenha problemas com a porta 80, 
                              // utilize outra, por exemplo 8082,
                              // e o endereco do NodeMCU passara a ser
                              // IP:PORTA, por exemplo 192.168.0.15:8082

// Setup
void setup() {
  Serial.begin(115200);
  delay(500);                                       // INTERVALO DE 500 MILISEGUNDOS

  // Leitor MFRC522
  SPI.begin(); // Inicia o barramento SPI
  mfrc522.PCD_Init(); // Inicia a placa MFRC522
  Serial.println("Sistema bloqueado. Aguardando cartao para leitura...\n");
  pinMode(LedRele, OUTPUT);
  pinMode(rele, OUTPUT);
  digitalWrite(LedRele,HIGH);
  digitalWrite(rele,LOW);

  // WiFi
  Serial.println("");                               // PULA UMA LINHA NA JANELA SERIAL
  Serial.print("Conectando a ");                    // ESCREVE O TEXTO NA SERIAL
  Serial.print(ssid);                               // ESCREVE O NOME DA REDE NA SERIAL
  WiFi.begin(ssid, password);                       // PASSA OS PARÂMETROS PARA A FUNÇÃO QUE VAI FAZER A CONEXÃO COM A REDE SEM FIO
  while (WiFi.status() != WL_CONNECTED) {           // ENQUANTO STATUS FOR DIFERENTE DE CONECTADO
    delay(500);                                     // INTERVALO DE 500 MILISEGUNDOS
    Serial.print(".");                              // ESCREVE O CARACTER NA SERIAL
  }
  Serial.println("");                               // PULA UMA LINHA NA JANELA SERIAL
  Serial.print("Conectado a rede sem fio ");        // ESCREVE O TEXTO NA SERIAL
  Serial.println(ssid);                             // ESCREVE O NOME DA REDE NA SERIAL
  server.begin();                                   // INICIA O SERVIDOR PARA RECEBER DADOS NA PORTA DEFINIDA EM "WiFiServer server(porta);"
  Serial.println("Servidor iniciado");              // ESCREVE O TEXTO NA SERIAL
  Serial.print("IP para se conectar ao NodeMCU: "); // ESCREVE O TEXTO NA SERIAL
  Serial.print("http://");                          // ESCREVE O TEXTO NA SERIAL
  Serial.println(WiFi.localIP());                   // ESCREVE NA SERIAL O IP RECEBIDO DENTRO DA REDE SEM FIO 
                                                    // (O IP NESSA PRÁTICA É RECEBIDO DE FORMA AUTOMÁTICA)
                                                    // Servidor
  server.on("/", handleRoot);                       // Qual rotina a ser seguida no local raiz. É a página a ser exibida
  server.on("/readADC", handleADC);                 // Esta página é chamada pelo Script java AJAX
  server.begin();                                   // Inicia o servidor
  Serial.println("Servidor HTTP iniciado");
}

void loop() {

  //corrente
  corrente = ((analogRead(pino_sct) - 512) / 1024.0 * 3.3 / 20.0 * 2000.0);
  leitura_rfid();

  server.handleClient();
}

// Rotina executada quando voce acessa o IP do NodeMCU pelo browser
void handleRoot() {
  String s = MAIN_page; // Le o conteudo HTML
  server.send(200, "text/html", s); // Envia a pagina web
}

void handleADC() {
  // Ref 1: https://circuits4you.com/2019/01/11/nodemcu-esp8266-arduino-json-parsing-example/
  // Ref 2: https://circuits4you.com/2019/01/25/arduino-how-to-put-quotation-marks-in-a-string/
  String data = "{\"Irms\":\""+ String(Irms()) +"\", \"potAparente\":\""+ String(potAparente()) +"\", \"potAtiva\":\""+ 
  String(potAtiva()) +"\", \"Consumo\":\""+ String(Consumo()) +"\"}";
  server.send(200, "text/plane", data); // Envia os valores JSON para a requisicao ajax do cliente
}

//funcoes do rfid
void dump_byte_array(byte *buffer, byte bufferSize) {
  read_rfid = "";
  for (byte i = 0; i < bufferSize; i++) {
    read_rfid = read_rfid + String(buffer[i], HEX);
  }
}

void open_lock() {
  estado_rele = !estado_rele;
  digitalWrite(rele, estado_rele);
  if(estado_rele) {
    digitalWrite(LedRele,LOW);
    Serial.println("Sistema liberado...\n");
  }
  else {
    digitalWrite(LedRele,HIGH);
    Serial.println("Sistema bloqueado...\n");
  }
}

void leitura_rfid() {
  if((millis() - timer_rfid) > 1000) {
  // Bloqueia para novos cartões
  if (!mfrc522.PICC_IsNewCardPresent())
  return;
  // Seleciona um dos cartões
  if (!mfrc522.PICC_ReadCardSerial())
  return;
  dump_byte_array(mfrc522.uid.uidByte, mfrc522.uid.size);
    Serial.print("\nNumero ID do cartao identificado (hex): ");
    Serial.println(read_rfid);
    
  if (read_rfid == ok_rfid_1) { // RFID #1
  // Libere o sistema:
    open_lock();
  }
  if (read_rfid == ok_rfid_2) { // RFID #2
    // Libere o sistema:
    open_lock();
  }
  if (read_rfid != ok_rfid_1 && read_rfid != ok_rfid_2) {
    digitalWrite(LedRele,HIGH);
    Serial.print("Cadastro nao encontrado.\n");
  }
  timer_rfid = millis();
  }
}
//fim da funcoes do rfid

//funcoes da corrente
float Irms() {
  // Leitura da corrente RMS
  float soma = 0;
  float nMedio = 0;
  float amostras[10000];
  float produto[10000];

  // Leitura de 1 período
  int i = 1;
  tempoatual = micros();
  while(micros() - tempoatual < 16667) {
    corrente = ((analogRead(pino_sct) - 512) / 1024.0 * 3.3 / 20.0 * 2000.0);
    amostras[i] = corrente;
    i++;
  }

  // Calcula o nível médio
  correnteMax = 0.0;
  correnteMin = 3.3;
  for(int n = 1; n < i; n++) {
    if(amostras[n] > correnteMax) {
    correnteMax = amostras[n];
    }
    if(amostras[n] < correnteMin) {
    correnteMin = amostras[n];
    }
  }
  nMedio = (correnteMin + correnteMax) / 2.0;

  // Remove o nível médio e faz o somatório das amostras ao quadrado
  for(int n = 1; n < i; n++) {
    amostras[n] = amostras[n] - nMedio; // Remove nível médio
    produto[n] = amostras[n] * amostras[n];
    soma += produto[n];
  }

  // Calcula a média dos valores
  correnteRMS = soma / i;
  
  // Calcula a raiz quadrada
  correnteRMS = sqrt(correnteRMS);
  return correnteRMS;
}

// Cálculo da Potência Aparente em kVA
float potAparente() {
    return UltimoS = Irms() * rede / 1000.0;
}
// Cálcula a Potência Ativa em kW considerando FP = 0.8
float potAtiva() {
    return (potAparente() * 0.8);
}
// Calcula o consumo de energia em kWh
float Consumo() {
    tempoatual = millis();
    deltat = tempoatual - tempoanterior;
    tempoanterior = tempoatual;
    return energia += (potAtiva() * deltat) / 3600.0 / 1000.0;
}