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
#include <SoftwareSerial.h>
#include <ArduinoJson.h>

extern "C" {
#include "user_interface.h"
}

// config device command
const char* server_arduino = "server_arduino";
const char* esp_arduino = "esp_arduino";

// config request command
#define chip_id_request "chip_id_request"
#define mac_address_request "mac_address_request"
#define data "data"

String chipIdEsp = "";
String mac_address = "";

const byte RX = 4;
const byte TX = 5;

SoftwareSerial mySerial(RX, TX, false, 256);
SerialCommand sCmd(mySerial); // Khai báo biến sử dụng thư viện Serial Command

// Update these with values suitable for your network.
const char* WIFI_SSID = "Phong Ky Thuat";
const char* WIFI_PASS = "123456789";

const char* MQTT_ID = "ESP8266";
const char* MQTT_USER = "esp32";
const char* MQTT_PASS = "mtt@23377";
const char* MQTT_SERVER = "113.161.21.15";
const char* MQTT_PORT = "1883";

const char* TOPIC_PUBLISH = "outTopic";
const char* TOPIC_SUBSCRIBE = "inTopic";

const char* get_chipID = "get_chipID";
uint32_t chipID;

WiFiClient espClient;
PubSubClient client(espClient);

//StaticJsonBuffer<200> jsonBuffer;

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
    // sCmd.addDefaultHandler(defaultCommand);

    // Một số hàm trong thư viện Serial Command
    sCmd.addCommand(server_arduino, requestResponse);   //Khi có lệnh thì thực hiện hàm "rxRequestServer"
//    sCmd.addCommand(esp_arduino, rxRequest);        //Khi có lệnh thì thực hiện hàm "rxResponseEsp"
//sCmd.addDefaultHandler(defaultCommand);


}

void setup_wifi() {

    delay(10);
    // We start by connecting to a WiFi network
    Serial.println();
    Serial.print("Connecting to ");
    Serial.println(WIFI_SSID);

    WiFi.begin(WIFI_SSID, WIFI_PASS);

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
    StaticJsonBuffer<200> jsonBuffer;
    JsonObject& root = jsonBuffer.parseObject(jsonStr); 
    
    mySerial.print("ARDUINO");
    mySerial.print('\r');
//     mySerial.print(jsonStr);
    root.printTo(mySerial);  
    mySerial.print('\r');

}

void reconnect() {
    // Loop until we're reconnected
    while (!client.connected()) {
        Serial.print("Attempting MQTT connection...");
        // Attempt to connect
        if (client.connect("ESP8266Client", "esp32", "mtt@23377")) {
            Serial.println("connected");
            // Once connected, publish an announcement...
            client.publish(TOPIC_PUBLISH, "hello world");
            // ... and resubscribe
            client.subscribe(TOPIC_SUBSCRIBE);
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

}

/*
    json_string_from_server = {
    "jsonCmd" : "read_max_value/read_average_value/get_chipID"
    }
*/

void defaultCommand(char *command) {
    char *json = sCmd.next();
    client.publish(TOPIC_PUBLISH, json);
}

void requestResponse() {
//    Serial.println(deviceCmd);
    Serial.println(esp_arduino);
    char *json = sCmd.next(); //Chỉ cần một dòng này để đọc tham số nhận đươc
    Serial.println(json);
    StaticJsonBuffer<200> jsonBuffer; //tạo Buffer json có khả năng chứa tối đa 200 ký tự
    JsonObject& root = jsonBuffer.parseObject(json);//đặt một biến root mang kiểu json

    String requestCmd = root["request"];
    String response = root["response"];

    if(requestCmd==mac_address_request){
        //txResponse(deviceCmd, requestCmd, mac_address);
    }
    if(response==data){
        client.publish(TOPIC_PUBLISH, json);
    }
}

void defaultCommand(String Command){
  
}

