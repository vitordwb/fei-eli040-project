// servidor

#include <ESP8266WiFi.h>
#include <WiFiClient.h>
#include <ESP8266WebServer.h>
#include "index.h" // Conteudo da pagina HTML com javascript

//SSID e Password do seu roteador WiFi
const char* ssid = "VITOR"; // Variavel que armazena o nome da rede sem fio
const char* password = "vitor2019!"; // Variavel que armazena a senha da rede sem fio

ESP8266WebServer server(80);  // Caso tenha problemas com a porta 80, 
                              // utilize outra, por exemplo 8082,
                              // e o endereco do NodeMCU passara a ser
                              // IP:PORTA, por exemplo 192.168.0.15:8082

// Setup
void setup() {
  Serial.begin(115200);
  delay(500);                                       // INTERVALO DE 500 MILISEGUNDOS
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
  String data = "{\"Irms\":\""+ String(1) +"\", \"potAparente\":\""+ String(2) +"\", \"potAtiva\":\""+ 
  String(4) +"\", \"Consumo\":\""+ String(3) +"\"}";
  server.send(200, "text/plane", data); // Envia os valores JSON para a requisicao ajax do cliente
}
