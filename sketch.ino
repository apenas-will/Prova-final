#include <WiFi.h>
#include <HTTPClient.h>

#define led_verde 2 // Pino utilizado para controle do led verde
#define led_amarelo 9 // Pino utilizado para controle do led amarelo
#define led_vermelho 40 // Pino utilizado para controle do led vermelho

const int botao = 18;  // Pino utilizado para captar sinal do botão
int estadoBotao = 0;  // Variável para leitura do estado do botão

const int pinoLdr = 4;  // Pino utilizado para captar sinal do sensor de luminosidade LDR
int limite=600; // Limite para disparo de ação do LDR

// Variáveis para debounce
unsigned long lastDebounceTime = 0; // Último momento de mudança de estado
unsigned long debounceDelay = 50; // Intervalo do debounce (50 ms)
bool lastButtonState = false;  // Último estado do botão

void setup() {

  // Configuração inicial dos pinos para controle dos leds como OUTPUTs (saídas) do ESP32
  pinMode(led_verde,OUTPUT);
  pinMode(led_vermelho,OUTPUT);
  pinMode(led_amarelo,OUTPUT);

  // Inicialização das entradas
  pinMode(botao, INPUT); // Inicializa o pino do botão como INPUT (entrada)
  pinMode(pinoLdr, INPUT); // Inicializa o pino do botão como INPUT (entrada)

  digitalWrite(led_verde, LOW); // Inicializa o led verde como LOW (apagado)
  digitalWrite(led_vermelho, LOW); // Inicializa o led vermelho como LOW (apagado)
  digitalWrite(led_amarelo, LOW); // Inicializa o led amarelo como LOW (apagado)

  Serial.begin(9600); // Configuração para debug por interface serial entre ESP e computador com baud rate de 9600

  WiFi.begin("Wokwi-GUEST", ""); // Conexão à rede WiFi aberta com SSID Wokwi-GUEST

  while (WiFi.status() != WL_CONNECT_FAILED) {
    delay(100);
    Serial.print(".");
  }
  Serial.println("Conectado ao WiFi com sucesso!"); // Considerando que saiu do loop acima, o ESP32 agora está conectado ao WiFi (outra opção é colocar este comando dentro do if abaixo)

  // Verifica estado do botão
  estadoBotao = digitalRead(botao);
  if (estadoBotao == HIGH) {
    Serial.println("Botão pressionado!");
  } else {
    Serial.println("Botão não pressionado!");
  }

  if(WiFi.status() == WL_CONNECTED){ // Se o ESP32 estiver conectado à Internet
    HTTPClient http;

    String serverPath = "http://www.google.com.br/"; // Endpoint da requisição HTTP

    http.begin(serverPath.c_str());

    int httpResponseCode = http.GET(); // Código do Resultado da Requisição HTTP

    if (httpResponseCode>0) {
      Serial.print("Resposta HTTP: ");
      Serial.println(httpResponseCode);
      String payload = http.getString();
      Serial.println(payload);
      }
    else {
      Serial.print("Código do erro: ");
      Serial.println(httpResponseCode);
      }
      http.end();
    }

  else {
    Serial.println("WiFi desconectado");
  }
}

bool claro = true; // Variável que guarda se está claro (true) ou escuro (false)
bool estado_amarelo = false; // Variável que guarda se o led amarelo está aceso (true) ou apagado (false)
bool estado_verde = false; // Variável que guarda se o led verde está aceso (true) ou apagado (false)
bool estado_vermelho = false; // Variável que guarda se o led vermelho está aceso (true) ou apagado (false)

// Função que apaga leds
void apagar(led){
  digitalWrite(led, LOW);
  if (led == 2){
    estado_verde = false;
  } else if (led == 9){
    estado_amarelo = false;
  } else {
    estado_vermelho = false;
  }
}

// Função qeu liga leds
void acender(led){
  digitalWrite(led, HIGH);
  if (led == 2){
    estado_verde = true;
  } else if (led == 9){
    estado_amarelo = true;
  } else {
    estado_vermelho = true;
  }
}

int inicio = millis(); // variável que guarda o incio do ciclos (modificada a cada rotina específica)

void loop() {
  int estadoLdr=analogRead(pinoLdr);

  if(estadoLdr <= limite){
    Serial.print("Está escuro, ligar luzes");
    Serial.println(estadoLdr);
    int intervalo = 1000;

    if (claro){
      inicio = millis(); // Inicio do ciclo de piscadas do led amarelo
      claro = false; // Salva que agora está escuro
    }

    if (!claro && millis() - inicio >= intervalo){
      if (estado_amarelo){
        apagar(led_amarelo);
        inicio = millis(); // Determina que o ciclo recomeçou
      } else {
        acender(led_amarelo);
      }
    }

  } else {
    if (!claro){
      claro = true;
    }
    
    inicio = millis();

    Serial.print("Está claro, desligar luzes");
    Serial.println(estadoLdr);
    apagar(led_amarelo);
    apagar(led_verde);
    apagar(led_vermelho);

    acender(led_verde);
    if (claro && millis() - inicio == 3000){
      if (estado_verde){
        apagar(led_verde);
      }
      acender(led_amarelo);
    }

    if (claro && millis() - inicio == 5000){
      if (estado_amarelo){
        apagar(led_amarelo);
      }
      acender(led_vermelho);
    }    

    if (digitalRead(botao) == HIGH && estado_vermelho){
      delay(1000);
      apagar(led_vermelho)
      inicio = millis();
    } else {
      delay(5000);
    }
  }
}