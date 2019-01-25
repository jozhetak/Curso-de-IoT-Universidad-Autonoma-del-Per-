/*
 Basic ESP8266 MQTT example
 This sketch demonstrates the capabilities of the pubsub library in combination
 with the ESP8266 board/library.
 It connects to an MQTT server then:
  - publishes "hello world" to the topic "outTopic" every two seconds
  - subscribes to the topic "inTopic", printing out any messages
    it receives. NB - it assumes the received payloads are strings not binary
  - If the first character of the topic "inTopic" is an 1, switch ON the ESP Led,
    else switch it off
 It will reconnect to the server if the connection is lost using a blocking
 reconnect function. See the 'mqtt_reconnect_nonblocking' example for how to
 achieve the same result without blocking the main loop.
 To install the ESP8266 board, (using Arduino 1.6.4+):
  - Add the following 3rd party board manager under "File -> Preferences -> Additional Boards Manager URLs":
       http://arduino.esp8266.com/stable/package_esp8266com_index.json
  - Open the "Tools -> Board -> Board Manager" and click install for the ESP8266"
  - Select your ESP8266 in "Tools -> Board"
*/

#include <ESP8266WiFi.h>
#include <PubSubClient.h>

// Update these with values suitable for your network.

const char* ssid = "xperia";
const char* password = "04592945";
const char* mqtt_server = "broker.shiftr.io";

const char clientID[]="ESP8266-marko";  //identificador unico de para cada dispositivo iot
const char username[]="marko-caballero";       //usuario configurado en broker
const char passwords[]="marko-caballero";      //contraseña usuario broker
const char willTopic[]="status";
int willQoS=2 ;                             //0-1-2
int  willRetain=0 ;                         //0-1  //si se activa o no la retencion de data
const char willMessage[]="desconectado";    //mensaje cuando device este desconectado de broker



WiFiClient espClient;
PubSubClient client(espClient);
long lastMsg = 0;
char msg[50];
int value = 0;

void setup_wifi() {

  delay(10);
  // We start by connecting to a WiFi network
  Serial.println();
  Serial.print("Connecting to ");
  Serial.println(ssid);

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

//aqui llegan los mensajes
void callback(char* topic, byte* payload, unsigned int length) {
  Serial.print("Message arrived [");
  Serial.print(topic);
  Serial.print("] ");


  String string_topic = String(topic);
  String string_payload;
  char c;
  for (int i = 0; i < length; i++) {
    Serial.print((char)payload[i]);
    c=payload[i];
    string_payload += c; //concatenando mensajes
  }

  if(string_topic=="rele"){
    if(string_payload=="true")
    {
      Serial.println("encender foco");
      pinMode(D4,OUTPUT);
      digitalWrite(D4, LOW);
    }
    if(string_payload=="false")
    {
      Serial.println("apagar foco");
      pinMode(D4,OUTPUT);
      digitalWrite(D4, HIGH);
    }
  }
  Serial.println();
}

void reconnect() {
  // Loop until we're reconnected
  while (!client.connected()) {
    Serial.print("Attempting MQTT connection...");
    // Create a random client ID
    String clientId = "ESP8266Client-";
    clientId += String(random(0xffff), HEX);
    // Attempt to connect
    //if (client.connect(clientId.c_str())) {
  if(client.connect(clientID,username,passwords,willTopic,willQoS,willRetain,willMessage)){   

      Serial.println("connected");
      // Once connected, publish an announcement...
      client.subscribe("inTopic");
      client.subscribe("rele");
      
    } else {
      Serial.print("failed, rc=");
      Serial.print(client.state());
      Serial.println(" try again in 5 seconds");
      // Wait 5 seconds before retrying
      delay(5000);
    }
  }
}

void setup() {
  pinMode(BUILTIN_LED, OUTPUT);     // Initialize the BUILTIN_LED pin as an output
  Serial.begin(115200);
  setup_wifi();
  client.setServer(mqtt_server, 1883);
  client.setCallback(callback);
}



void loop() {

  if (!client.connected()) { //se verifica que la conexion sea correcta
    reconnect();
  }
  
  client.loop(); //funcion para verificar si llegan mensajes

  long now = millis();
  if (now - lastMsg > 2000){
    lastMsg = now;
    
    int temperatura = random(21,26); //simular sensor
    int humedad = random(70,85);  
    
    String my_payload= String(temperatura); //converir a estring
    my_payload.toCharArray(msg, (my_payload.length() + 1)); //luego convierto a char array 
    Serial.println(msg);
    client.publish("temperatura",msg );  //publicar mensajes al topico outTopic

    my_payload= String(humedad); //converir a estring
    my_payload.toCharArray(msg, (my_payload.length() + 1)); //luego convierto a char array 
    Serial.println(msg);
    client.publish("humedad",msg );  //publicar mensajes al topico outTopic
 
  }
}
