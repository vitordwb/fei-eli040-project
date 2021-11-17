// Biblioteca necessária (disponível através do menu Ferramentas > Gerencias Bibliotecas...):
// MFRC522 by GithubCommunity
// Leitor MFRC522
#include <SPI.h>
#include <MFRC522.h>
#define RST_PIN D1 // Pino RST
#define SS_PIN D2  // Pino SDA
MFRC522 mfrc522(SS_PIN, RST_PIN); // Cria a instância MFRC522
unsigned long timer_rfid = millis();
String read_rfid;
String ok_rfid_1 = "49bbe96";
String ok_rfid_2 = "ec0bf79"; // Adicionar quantos cartões forem necessários
int estado_rele = 0;
const int LedRele = D8;
const int rele = D0;

void setup() {
  Serial.begin(115200);
  delay(500); // INTERVALO DE 500 MILISEGUNDOS
  // Leitor MFRC522
  SPI.begin(); // Inicia o barramento SPI
  mfrc522.PCD_Init(); // Inicia a placa MFRC522
  Serial.println("Sistema bloqueado. Aguardando cartao para leitura...\n");
}

void loop() {
  // put your main code here, to run repeatedly:
  //Serial.println(analogRead(A0)-512);
  leitura_rfid();
}

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
