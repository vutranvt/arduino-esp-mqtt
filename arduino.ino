#include <String.h>
#include <ArduinoJson.h>
#include <SoftwareSerial.h>
#include <SerialCommand.h>  // Thêm vào sketch thư viện Serial Command
const byte RX = 3;          // Chân 3 được dùng làm chân RX
const byte TX = 2;          // Chân 2 được dùng làm chân TX

double ammAdc1 = 0;
double ammAdc2 = 0;
double ammAdc3 = 0;
int counter1 = 0;
int counter2 = 0;
int counter3 = 0;

bool CONFIG_MAX_VALUE = false;

SoftwareSerial mySerial = SoftwareSerial(RX, TX);

// SerialCommand sCmd(mySerial); // Khai báo biến sử dụng thư viện Serial Command

// int red = 4, blue = 5; // led đỏ đối vô digital 4, led xanh đối vô digital 5

void setup() {
    // Khởi tạo Serial ở baudrate 57600 để debug ở serial monitor
    Serial.begin(115200);
    // Khởi tạo Serial ở baudrate 57600 cho cổng Serial thứ hai, dùng cho việc kết nối với ESP8266
    mySerial.begin(115200);

    // Một số hàm trong thư viện Serial Command
    sCmd.addCommand("ARDUINO", configReadValue); //Khi có lệnh thì thực hiện hàm "configReadValue"
    // Serial.println("Da san sang nhan lenh");
}

void loop() {
    sCmd.readSerial();
    //Bạn không cần phải thêm bất kỳ dòng code nào trong hàm loop này cả
}

// hàm led_red sẽ được thực thi khi gửi hàm LED_RED
void led() {
    Serial.println("LED");
    char *json = sCmd.next(); //Chỉ cần một dòng này để đọc tham số nhận đươc
    Serial.println(json);
    StaticJsonBuffer<200> jsonBuffer; //tạo Buffer json có khả năng chứa tối đa 200 ký tự
    JsonObject& root = jsonBuffer.parseObject(json);//đặt một biến root mang kiểu json

    int redStatus = root["led"][0];//json -> tham số root --> phần tử thứ 0. Đừng lo lắng nếu bạn không có phần tử này, không có bị lỗi đâu!
    int blueStatus = root["led"][1];//json -> tham số root --> phần tử thứ 0. Đừng lo lắng nếu bạn không có phần tử này, không có bị lỗi đâu!


    //kiểm thử giá trị
    Serial.print(F("redStatus "));
    Serial.println(redStatus);
    Serial.print(F("blueStatus "));
    Serial.println(blueStatus);

    //xuất ra màn hình
    digitalWrite(red, redStatus);
    digitalWrite(blue, blueStatus);     
}



/*
json_string_from_server = {
    "jsonCmd" : "read_max_value/read_average_value"
}
*/
void configReadValue() {
    char *json = sCmd.next(); //Chỉ cần một dòng này để đọc tham số nhận đươc
    Serial.println(json);
    StaticJsonBuffer<200> jsonBuffer; //tạo Buffer json có khả năng chứa tối đa 200 ký tự
    JsonObject& root = jsonBuffer.parseObject(json);//đặt một biến root mang kiểu json

    string cmd = root["jsonCmd"];

    if(strcmp(cmd, "read_max_value")==0) {
        CONFIG_MAX_VALUE = true;
    }
    if(strcmp(cmd, "read_average_value")) {
        CONFIG_MAX_VALUE = false;
    }
}

void jsonStringEncoder(double data1, double data2, double data3) {
    StaticJsonBuffer<200> jsonBuffer;
    JsonObject& root = jsonBuffer.createObject();
    root["adc1"] = data1;
    root["adc2"] = data2;
    root["adc3"] = data3;
}

/*
// Send analog data to Server
json_string = {
"adc1" : 189,
"adc2" : 18,
"adc3" : 1
}
*/
void TaskSendData(void *pvParameters) {
    (void) pvParameters;

    double data1 = 0;
    double data2 = 0;
    double data3 = 0;

    data1 = ampAdc1;
    ampAdc1 = 0;
    counter1 = 0;

    data2 = ampAdc2;
    ampAdc2 = 0;
    counter2 = 0;

    data3 = ampAdc3;
    ampAdc3 = 0;
    counter3 = 0;

    StaticJsonBuffer<200> jsonBuffer;
    JsonObject& root = jsonBuffer.createObject();
    root["adc1"] = data1;
    root["adc2"] = data2;
    root["adc3"] = data3;

    root.printTo(mySerial);
    delay(4000);
}
