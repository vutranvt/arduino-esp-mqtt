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
#include <SerialCommand.h>

extern "C" {
#include "user_interface.h"
}

const byte RX = 4;
const byte TX = 5;

SoftwareSerial mySerial(RX, TX, false, 256);
SerialCommand sCmd(mySerial); // Khai báo biến sử dụng thư viện Serial Command

// Update these with values suitable for your network.
const char* SSID = "Phong Ky Thuat";
const char* PASSWORD = "123456789";
const char* MQTT_SERVER = "broker.mqtt-dashboard.com";

const char* PUBLISH_TOPIC = "outTopic";
const char* SUBSCRIBE_TOPIC = "inTopic";

const char* get_chipID = "get_chipID";
uint32_t chipID;

WiFiClient espClient;
PubSubClient client(espClient);

long lastMsg = 0;
char msg[50];
int value = 0;

void setup() {
    pinMode(BUILTIN_LED, OUTPUT);     // Initialize the BUILTIN_LED pin as an output
    Serial.begin(115200);
    mySerial.begin(115200); //Bật software serial để giao tiếp với Arduino, nhớ để baudrate trùng với software serial trên mạch arduino
    delay(10);

    setup_wifi();
    client.setServer(MQTT_SERVER, 1883);
    client.setCallback(callback);

    chipID = ESP.getChipId();
    sCmd.addDefaultHandler(defaultCommand);
}

void setup_wifi() {

    delay(10);
    // We start by connecting to a WiFi network
    Serial.println();
    Serial.print("Connecting to ");
    Serial.println(SSID);

    WiFi.begin(SSID, PASSWORD);

    while (WiFi.status() != WL_CONNECTED) {
        delay(500);
        Serial.print(".");
    }

    Serial.println("");
    Serial.println("WiFi connected");
    Serial.println("IP address: ");
    Serial.println(WiFi.localIP());
}

void callback(char* topic, byte* payload, unsigned int length) {
    String jsonStr = "";
    Serial.print("Message arrived [");
    Serial.print(topic);
    Serial.print("] ");
    for (int i = 0; i < length; i++) {
        //Serial.print((char)payload[i]);
        jsonStr += (char)payload[i];
    }
    Serial.println(jsonStr);
    
    mySerial.print("ARDUINO");
    mySerial.print('\r');
    mySerial.print(jsonStr);
    mySerial.print('\r');
    
//    StaticJsonBuffer<200> jsonBuffer; //tạo Buffer json có khả năng chứa tối đa 200 ký tự
//    JsonObject& root = jsonBuffer.parseObject(jsonStr);
//
//    string cmd = root["jsonCmd"];
//
//    if (strcmp(cmd, "get_chipID") == 0) {
//        client.publish(PUBLISH_TOPIC, "chipID");
//    }

}

void reconnect() {
    // Loop until we're reconnected
    while (!client.connected()) {
        Serial.print("Attempting MQTT connection...");
        // Attempt to connect
        if (client.connect("ESP8266Client")) {
            Serial.println("connected");
            // Once connected, publish an announcement...
            client.publish(PUBLISH_TOPIC, "hello world");
            // ... and resubscribe
            client.subscribe(SUBSCRIBE_TOPIC);
        } else {
            Serial.print("failed, rc=");
            Serial.print(client.state());
            Serial.println(" try again in 5 seconds");
            // Wait 5 seconds before retrying
            delay(5000);
        }
    }
}
void loop() {

    if (!client.connected()) {
        reconnect();
    }
    client.loop();  //

    sCmd.readSerial();  //
    // long now = millis();
    // if (now - lastMsg > 2000) {
    //     lastMsg = now;
    //     ++value;
    //     snprintf (msg, 75, "hello world #%ld", value);
    //     Serial.print("Publish message: ");
    //     Serial.println(msg);
    //     client.publish("PUBLISH_TOPIC", msg);
    // }
}

/*
    json_string_from_server = {
    "jsonCmd" : "read_max_value/read_average_value/get_chipID"
    }
*/

void defaultCommand(String command) {
    char *json = sCmd.next();
    client.publish("PUBLISH_TOPIC", json);
}


