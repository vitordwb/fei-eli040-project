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

// Leitor MFRC522
#include <SPI.h>
#include <MFRC522.h>
#define RST_PIN D1 // Pino RST
#define SS_PIN D2 // Pino SDA
MFRC522 mfrc522(SS_PIN, RST_PIN); // Cria a instância MFRC522
unsigned long timer_rfid = millis();
String read_rfid;
String ok_rfid_1 = "4c92383";
String ok_rfid_2 = "45388530"; // Adicionar quantos cartões forem necessários

int estado_rele = 0;
const int rele = D8;
const int LedRele = D0;


#include <ESP8266WiFi.h>
#include <WiFiClient.h>
#include <ESP8266WebServer.h>
//SSID e Password do seu roteador WiFi
const char* ssid = "CRISTIANE & MURILO"; // Variavel que armazena o nome da rede sem fio
const char* password = "cm682311"; // Variavel que armazena a senha da rede sem fio
ESP8266WebServer server(80); 

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

  // Leitura do RFID
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

const char MAIN_page[] PROGMEM = R"=====(
<!doctype html>
<html>
<head>
<!--Titulo da pagina-->
<title>Monitoramento de Energia</title>
<!—- Baseado no tutorial: https://circuits4you.com/2019/01/11/esp8266-data-logging-with-real-time-graphs/ -->
<script src = "https://cdnjs.cloudflare.com/ajax/libs/Chart.js/2.7.3/Chart.min.js"></script>
<link rel="stylesheet" href="https://maxcdn.bootstrapcdn.com/bootstrap/4.0.0/css/bootstrap.min.css" integrity="sha384-Gn5384xqQ1aoWXA+058RXPxPg6fy4IWvTNh0E263XmFcJlSAwiGgFAW/dAiS6JXm" crossorigin="anonymous">
    <script src="https://code.jquery.com/jquery-3.2.1.slim.min.js" integrity="sha384-KJ3o2DKtIkvYIK3UENzmM7KCkRr/rE9/Qpg6aAZGJwFDMVNA/GpGFF93hXpG5KkN" crossorigin="anonymous"></script>
<script src="https://cdnjs.cloudflare.com/ajax/libs/popper.js/1.12.9/umd/popper.min.js" integrity="sha384-ApNbgh9B+Y1QKtv3Rn7W3mgPxhU9K/ScQsAP7hUibX39j7fakFPskvXusvfa0b4Q" crossorigin="anonymous"></script>
<script src="https://maxcdn.bootstrapcdn.com/bootstrap/4.0.0/js/bootstrap.min.js" integrity="sha384-JZR6Spejh4U02d8jOt6vLEHfe/JQGiRRSQQxSfFWpi1MquVdAyjUar5+76PVCmYl" crossorigin="anonymous"></script>
<style>
#above-table {
  background-color:cornflowerblue;
  border-style: solid;
  border-color:black;
  border-radius: 10px;
  font-family: "Lucida Console";
}
#title_block {
  margin-left: 100px;
  margin-top:-50px;
}
#fei {
  margin-top: 8px;
  margin-left:10px;
  border-radius: 20px;
  margin-bottom: 8px;
}
#under-table{
  border-style: solid;
  border-color:black;
  border-radius: 10px;
  font-family: "Lucida Console";
}
#preco {
  border-style:solid;
  border-color:black;
  border-radius: 10px;
  margin-top: -35px;
  margin-left: 150px;
}
canvas{
-moz-user-select: none;
-webkit-user-select: none;
-ms-user-select: none;
}
/* Data Table Styling */
#dataTable {
font-family: "Trebuchet MS", Arial, Helvetica, sans-serif;
border-collapse: collapse;
width: 100%;
}
#dataTable td, #dataTable th {
border: 1px solid #ddd;
padding: 8px;
}
#dataTable tr:nth-child(even){background-color: #f2f2f2;}
#dataTable tr:hover {background-color: #ddd;}
#dataTable th {
padding-top: 12px;
padding-bottom: 12px;
text-align: left;
background-color: #4CAF50;
color: white;
}
</style>
</head>
<body>
<div id="above-table">
       <img id="fei" src="https://s1.static.brasilescola.uol.com.br/be/vestibular/fei.jpg" width=70 height=60>
        <h2 id="title_block">ABASTECIMENTO DE CARRO ELETRICO</h2>
    </div>
<button id="start_btn" type="button" class="btn btn">Bloqueado</button>
<!--Primeira linha da pagina-->
<div style="text-align:center;"><b>Corrente RMS, Potencia Aparente, Potencia Ativa e Consumo utilizando o ESP8266</b></div>
<div class="chart-container" position: relative; height:350px; width:100%">
<canvas id="Chart" width="400" height="400"></canvas>
</div>
<div>
<table id="dataTable">
<!--Cabecalho da tabela-->
<tr><th>Horario</th><th>Corrente RMS (A)</th><th>Potencia Aparente (kVA)</th><th>Potencia Ativa (FP=0.8 - kW)</th><th>Consumo (FP=0.8 - kWh)</th></tr>
</table>
</div>
<br>
<br>
<script>
// Graphs visit: https://www.chartjs.org
var CorrenteRMS = [];
var PotenciaS = [];
var PotenciaP = [];
var ConsumoEE = [];
var timeStamp = [];
function showGraph()
{
var ctx = document.getElementById("Chart").getContext('2d');
var Chart2 = new Chart(ctx, {
type: 'line',
data: {
labels: timeStamp, // Bottom Labeling
datasets: [{
label: "Corrente RMS (A)",
fill: false,
backgroundColor: 'rgba( 243, 156, 18 , 1)', // Dot marker color
borderColor: 'rgba( 243, 156, 18 , 1)', // Graph Line Color
data: CorrenteRMS,
},
{
label: "Potencia Aparente (kVA)",
fill: false,
backgroundColor: 'rgba(156, 18, 243 , 1)', // Dot marker color
borderColor: 'rgba(156, 18, 243 , 1)', // Graph Line Color
data: PotenciaS,
},
{
label: "Potencia Ativa (FP=0.8 - kW)",
fill: false,
backgroundColor: 'rgba(204, 0, 0 , 1)', // Dot marker color
borderColor: 'rgba(204, 0, 0 , 1)', // Graph Line Color
data: PotenciaP,
},
{
label: "Consumo (FP=0.8 - kWh)",
fill: false,
backgroundColor: 'rgba(0, 51, 102 , 1)', // Dot marker color
borderColor: 'rgba(0, 51, 102 , 1)', // Graph Line Color
data: ConsumoEE,
}
],
},
options: {
title: {
display: true,
text: "Dados aferidos em tempo real"
},
maintainAspectRatio: false,
elements: {
line: {
tension: 0.5 // Smoothening (Curved) of data lines
}
},
scales: {
yAxes: [{
ticks: {
beginAtZero:true
}
}]
}
}
});
}
// On Page load show graphs
window.onload = function() {
console.log(new Date().toLocaleTimeString());
};
// Ajax script to get ADC voltage at every 5 Seconds
// Read This tutorial https://circuits4you.com/2018/02/04/
// esp8266-ajax-update-part-of-web-page-without-refreshing/
setInterval(function() {
// Call a function repetatively with 5 Second interval
getData();
}, 5000); // 5000mSeconds update rate
function getData() {
var xhttp = new XMLHttpRequest();
xhttp.onreadystatechange = function() {
if (this.readyState == 4 && this.status == 200) {
// Push the data in array
var time = new Date().toLocaleTimeString();
var txt = this.responseText;
var obj = JSON.parse(txt); // Ref: https://www.w3schools.com/js/js_json_parse.asp
CorrenteRMS.push(obj.Irms);
PotenciaS.push(obj.potAparente);
PotenciaP.push(obj.potAtiva);
ConsumoEE.push(obj.Consumo);
if(obj.Irms<=1){
  document.getElementById('start_btn').style.backgroundColor = 'Red';
  document.getElementById('start_btn').innerHTML = "Bloqueado";
}else{
  document.getElementById('start_btn').style.backgroundColor = 'Green';
  document.getElementById('start_btn').innerHTML = "Desbloqueado";
}
timeStamp.push(time);
showGraph(); // Update Graphs
// Update Data Table
var table = document.getElementById("dataTable");
var row = table.insertRow(1); // Add after headings
var cell1 = row.insertCell(0);
var cell2 = row.insertCell(1);
var cell3 = row.insertCell(2);
var cell4 = row.insertCell(3);
var cell5 = row.insertCell(4);
cell1.innerHTML = time;
cell2.innerHTML = obj.Irms;
cell3.innerHTML = obj.potAparente;
cell4.innerHTML = obj.potAtiva;
cell5.innerHTML = obj.Consumo;
}
};
xhttp.open("GET", "readADC", true); // Handle readADC server on ESP8266
xhttp.send();
}
</script>
</body>
</html>
)=====";

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

void setup() {
  Serial.begin(115200);
  delay(500); // INTERVALO DE 500 MILISEGUNDOS
  // Leitor MFRC522
  SPI.begin(); // Inicia o barramento SPI
  mfrc522.PCD_Init(); // Inicia a placa MFRC522
  Serial.println("Sistema bloqueado. Aguardando cartao para leitura...\n");

  // Acionamento do rele/led
  pinMode(LedRele, OUTPUT);
  pinMode(rele, OUTPUT);
  digitalWrite(LedRele,HIGH);
  digitalWrite(rele,LOW);

  //Servidor
  Serial.begin(115200);
  delay(500); // INTERVALO DE 500 MILISEGUNDOS
  // WiFi
  Serial.println(""); // PULA UMA LINHA NA JANELA SERIAL
  Serial.print("Conectando a "); // ESCREVE O TEXTO NA SERIAL
  Serial.print(ssid); // ESCREVE O NOME DA REDE NA SERIAL
  WiFi.begin(ssid, password); // PASSA OS PARÂMETROS PARA A FUNÇÃO QUE VAI FAZER A CONEXÃO COM A REDE SEM FIO
  while (WiFi.status() != WL_CONNECTED) { // ENQUANTO STATUS FOR DIFERENTE DE CONECTADO
  delay(500); // INTERVALO DE 500 MILISEGUNDOS
  Serial.print("."); // ESCREVE O CARACTER NA SERIAL
  }
  Serial.println(""); // PULA UMA LINHA NA JANELA SERIAL
  Serial.print("Conectado a rede sem fio "); // ESCREVE O TEXTO NA SERIAL
  Serial.println(ssid); // ESCREVE O NOME DA REDE NA SERIAL
  server.begin(); // INICIA O SERVIDOR PARA RECEBER DADOS NA PORTA DEFINIDA EM "WiFiServer server(porta);"
  Serial.println("Servidor iniciado"); // ESCREVE O TEXTO NA SERIAL
  Serial.print("IP para se conectar ao NodeMCU: "); // ESCREVE O TEXTO NA SERIAL
  Serial.print("http://"); // ESCREVE O TEXTO NA SERIAL
  Serial.println(WiFi.localIP()); // ESCREVE NA SERIAL O IP RECEBIDO DENTRO DA REDE SEM FIO
  // (O IP NESSA PRÁTICA É RECEBIDO DE FORMA AUTOMÁTICA)
  // Servidor
  server.on("/", handleRoot); // Qual rotina a ser seguida no local raiz. É a página a ser exibida
  server.on("/readADC", handleADC); // Esta página é chamada pelo Script java AJAX
  server.begin(); // Inicia o servidor
  Serial.println("Servidor HTTP iniciado");
}


void loop() {
  // Lê o valor analógico da corrente
  // SCT013 100A:50mA, R = 20 Ohm
  corrente = ((analogRead(pino_sct) - 512) / 1024.0 * 3.3 / 20.0 * 2000.0);
  // Envia os valores para o monitor serial
  Serial.println(Irms());
  Serial.println(potAparente());
  Serial.println(potAtiva());
  Serial.println(Consumo());

  leitura_rfid();

  // Servidor Web
  server.handleClient();
}