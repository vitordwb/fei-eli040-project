const int pino_sct = A0; 
int corrente = 0; 
void setup() {
Serial.begin(115200);
}
void loop() {
    // Lê o valor analógico da corrente
    // SCT013 100A:50mA, R = 20 Ohm
    corrente = ((analogRead(pino_sct) - 512) / 1024.0 * 3.3 / 20.0 * 2000.0);
    // SCT013 20A/1V, sem resistor
    //corrente = ((analogRead(pino_sct) - 512) / 1024.0 * 3.3 * 20.0);
    // SCT013 30A/1V, sem resistor
    //corrente = ((analogRead(pino_sct) - 512) / 1024.0 * 3.3 * 30.0);
    // Envia os valores para o monitor serial
    Serial.println(corrente);
} 

// Leitura da corrente
float soma = 0;
float amostras[10000]; 
// Leitura de 1 período
int i = 1;
tempoatual = micros();
while(micros() - tempoatual < 16667) {
    corrente = ((analogRead(pino_sct) - 512) / 1024.0 * 3.3 / 20.0 * 2000.0);  // SCT013 100A:50mA, R = 20 Ohm
    //corrente = ((analogRead(pino_sct) - 512) / 1024.0 * 3.3 * 20.0);         // SCT013 20A/1V, sem resistor
    //corrente = ((analogRead(pino_sct) - 512) / 1024.0 * 3.3 * 30.0);         // SCT013 30A/1V, sem resistor
    amostras[i] = corrente;
    i++;
}

// Calcula o nível médio
float nMedio = 0;
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
float produto[10000]; 
for(int n = 1; n < i; n++) {
    amostras[n] = amostras[n] - nMedio; // Remove nível médio
    produto[n] = amostras[n] * amostras[n];
    soma += produto[n];
}

// Sensor de corrente SCT e Calculo Potencia Aparente (S)
const int pino_sct = A0;  // Pino Analogico ADC
double CorrenteRms = 0;
unsigned long tempoatual;
float corrente = 0;
float correnteRMS=0;
float correnteMax = 0;
float correnteMin = 0;

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
        corrente = ((analogRead(pino_sct) - 512) / 1024.0 * 3.3 / 20.0 * 2000.0);  // SCT013 100A:50mA, R = 20 Ohm
        //corrente = ((analogRead(pino_sct) - 512) / 1024.0 * 3.3 * 20.0);         // SCT013 20A/1V, sem resistor
        //corrente = ((analogRead(pino_sct) - 512) / 1024.0 * 3.3 * 30.0);         // SCT013 30A/1V, sem resistor
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

// Cálculo da Potência Aparente (S) e Ativa (P)
int rede = 127; // Tensão da rede elétrica
double UltimoS = 0;
float energia=0;
unsigned long tempoanterior;
unsigned long deltat;
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