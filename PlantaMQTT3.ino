// Bruna Di Giovanni Ribeiro
// Caroline de Almeida Cimenta

//////Plant Care 

//Inclusão de bibliotecas
#include <ESP8266WiFi.h>
#include <PubSubClient.h>

//Dados da rede e do MQTT server
const char* ssid = "NET_2G457CD7";
const char* password = "DA457CD7";
const char* mqtt_server = "broker.mqtt-dashboard.com";

//Client
WiFiClient espClient;
PubSubClient client(espClient);

//Variáveis
unsigned long lastMsg = 0;
#define MSG_BUFFER_SIZE	(50)
char msg[MSG_BUFFER_SIZE];
int sensor_pin = A0;
const int relay = D1;
int output_value ;

//Conectando WiFi  
void setup_wifi() {

  delay(10);
  Serial.println();
  Serial.print("Connecting to ");
  Serial.println(ssid);

  WiFi.mode(WIFI_STA);
  WiFi.begin(ssid, password);

  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }

  randomSeed(micros());

  Serial.println("");
  Serial.println("WiFi connected");
  Serial.println("IP address: ");
  Serial.println(WiFi.localIP());
}

//Callback contendo o acionamento do relé via MQTT
void callback(char* topic, byte* payload, unsigned int length) {
  Serial.print("Message arrived [");
  Serial.print(topic);
  Serial.print("] ");
  for (int i = 0; i < length; i++) {
    Serial.print((char)payload[i]);
  }
  Serial.println();

  if ((char)payload[0] == '1') {
    digitalWrite(BUILTIN_LED, LOW);  
    digitalWrite(relay, LOW);
  } else {
    digitalWrite(BUILTIN_LED, HIGH); 
    digitalWrite(relay, HIGH);
  }

}

//Reconectando
void reconnect() {
  while (!client.connected()) {
    Serial.print("Attempting MQTT connection...");
    String clientId = "ESP8266Client-";
    clientId += String(random(0xffff), HEX);
    if (client.connect(clientId.c_str())) {
      Serial.println("connected");
      client.publish("teste_publicando_umi", "hello world");
      client.subscribe("teste_lendo_umi");
    } else {
      Serial.print("failed, rc=");
      Serial.print(client.state());
      Serial.println(" try again in 5 seconds");
      delay(5000);
    }
  }
}

//Setup
void setup() {
  pinMode(BUILTIN_LED, OUTPUT);
  pinMode(relay, OUTPUT);
  Serial.begin(115200);
  setup_wifi();
  client.setServer(mqtt_server, 1883);
  client.setCallback(callback);
}

//Loop contendo as informações para publicação do estado do sendor de umidade
void loop() {

  if (!client.connected()) {
    reconnect();
  }
  client.loop();

  unsigned long now = millis();
  if (now - lastMsg > 5000) {
    lastMsg = now;
    output_value= analogRead(sensor_pin);
    Serial.print("\nNível do sensor: ");
    Serial.print(output_value);

   
    if (output_value > 800){
      snprintf (msg, MSG_BUFFER_SIZE, "Estou com sede!!!");
      Serial.print("\nMensagem Publicada: ");
      Serial.println(msg);
      Serial.println("\n");
      client.publish("teste_publicando_umi", msg);

    }
    if (output_value < 800){
      snprintf (msg, MSG_BUFFER_SIZE, "Estou bem!!! Obrigada");
      Serial.print("\nMensagem Publicada: ");
      Serial.println(msg);
      Serial.println("\n");
      client.publish("teste_publicando_umi", msg);
    }
  }
}
