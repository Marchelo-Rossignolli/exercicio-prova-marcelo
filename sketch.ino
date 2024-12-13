#include <WiFi.h>
#include <HTTPClient.h>


#define led_verde 2 // Pino utilizado para controle do led verde
#define led_vermelho 40 // Pino utilizado para controle do led vermelho
#define led_amarelo 9 // Pino utilizado para controle do led amarelo

const int buttonPin = 18;  // Pino utilizado para o controle do botão
int buttonState = 0;  // Variável para ler o status do botão

const int ldrPin = 4;  // Pino utilizado para o controle do sensor LDR
const float limiarNoturno = 500; // Valor de limite para dia/noite

unsigned long previousMillis = 0;  // Variável para controle do tempo dos LEDs
int estado = 0; //Estado inicial do led
bool modoNoturno = false;

void setup() {

  // Configuração inicial dos pinos para controle dos leds como OUTPUTs (saídas) do ESP32
  pinMode(led_amarelo, OUTPUT);
  pinMode(led_verde, OUTPUT);
  pinMode(led_vermelho, OUTPUT);

  // Inicialização das entradas
  pinMode(buttonPin, INPUT_PULLUP); //Inicia o botão como um INPUT
  Serial.begin(115200);

  digitalWrite(led_amarelo, LOW);
  digitalWrite(led_verde, LOW);
  digitalWrite(led_vermelho, LOW);

  Serial.begin(9600); // Configuração para debug por interface serial entre ESP e computador com baud rate de 9600

  WiFi.begin("Wokwi-GUEST", ""); // Conexão à rede WiFi aberta com SSID Wokwi-GUEST

  while (WiFi.status() != WL_CONNECT_FAILED) {
    delay(100);
    Serial.print(".");
  }
  Serial.println("Conectado ao WiFi com sucesso!"); // Considerando que saiu do loop acima, o ESP32 agora está conectado ao WiFi (outra opção é colocar este comando dentro do if abaixo)

  // Verifica estado do botão
  buttonState = digitalRead(buttonPin);
  if (buttonState == HIGH) {
    Serial.println("Botão pressionado!");
  } else {
    Serial.println("Botão não pressionado!");
  }

  if (WiFi.status() == WL_CONNECTED) { // Se o ESP32 estiver conectado à Internet
    HTTPClient http;

    String serverPath = "http://www.google.com.br/"; // Endpoint da requisição HTTP

    http.begin(serverPath.c_str());

    int httpResponseCode = http.GET(); // Código do Resultado da Requisição HTTP

    if (httpResponseCode > 0) {
      Serial.print("HTTP Response code: ");
      Serial.println(httpResponseCode);
      String payload = http.getString();
      Serial.println(payload);
    }
    else {
      Serial.print("Error code: ");
      Serial.println(httpResponseCode);
    }
    http.end();
  }

  else {
    Serial.println("WiFi Disconnected");
  }
}

// Função que controla os modos do semáforo
void controleFarol() {
  unsigned long currentMillis = millis();
  sequenciaSemaforo(currentMillis);  // Sequência de semáforo
}

// Função que controla o estado de cada led, determinando qual é a sequência de funcionamento
void sequenciaSemaforo(unsigned long currentMillis) {
  static unsigned long previousMillis = 0;
  static int estado = 0;

  switch (estado) {
    case 0:
      ligarLed(led_verde);
      if (currentMillis - previousMillis >= 3000) {
        previousMillis = currentMillis;
        desligarLed(led_verde);
        estado = 1;
      }
      break;
    case 1:
      ligarLed(led_amarelo);
      if (currentMillis - previousMillis >= 2000) {
        previousMillis = currentMillis;
        desligarLed(led_amarelo);
        estado = 2;
      }
      break;
    case 2:
      ligarLed(led_vermelho);
      if (currentMillis - previousMillis >= 5000) {
        previousMillis = currentMillis;
        desligarLed(led_vermelho);
        estado = 0;
      }
      break;
  }
}

void ligarLed(int led) {
  digitalWrite(led, HIGH);
}

void desligarLed(int led) {
  digitalWrite(led, LOW);
}

void loop() {
  int ldrstatus = analogRead(ldrPin);

  if (ldrstatus <= limiarNoturno) {
    Serial.print("its dark turn on led");
    Serial.println(ldrstatus);

  } else {
    Serial.print("its bright turn off light");
    Serial.println(ldrstatus);
  }
}