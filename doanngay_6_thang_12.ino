//add thread lib for writing non-blocking code.
#include <Thread.h>
#include <ThreadController.h>

#include <ESP8266WiFi.h>
#include <SoftwareSerial.h>

//add json lib for handle json string
#include <ArduinoJson.h>

//add PubSubClient lib for communicate in MQTT protocol
#include <PubSubClient.h>

//I have already separated the code into two-part: state and logic code for the code look cleaner, modular and easy to maintain.
#include "Memory.h"

//Define MQTT parameters.
#define AIO_SERVER      "13.229.222.102"
#define AIO_SERVERPORT  1883
#define AIO_USERNAME    "sammy"
#define AIO_KEY         "12345678"

WiFiClient espClient;
PubSubClient client(espClient);

//Initialize state object
Memory mem(0,0,0);

SoftwareSerial uart1(D1, D2); // RX, TX

//Declare some thread
ThreadController controll = ThreadController();
Thread* readUARTThread = new Thread(); //khai báo tiểu trình đọc UART
Thread* chart2MqttThread = new Thread(); //khai báo tiểu gửi đồ thị
Thread* device2MqttThread = new Thread(); //khai báo tiểu gửi đồ thị

//Define SSID, PASSWORD wifi
#define WIFI_SSID "tuanphuong"
#define WIFI_PASSWORD "phuong1997"

//This callback function will be called when the MQTT topic which we have already subscribe had new data.
void callback(char* topic, byte* payload, unsigned int length) {
  char message[length + 1];
  for (int i = 0; i < length; i++)
  {
    message[i] = (char)payload[i];
  }
  message[length] = '\0';

  StaticJsonDocument<1024> doc;
  deserializeJson(doc, message);

  StaticJsonDocument<128> doc_send;

  if (doc.containsKey("d_01")) {
    doc_send["d_01"] = doc["d_01"];
  }

  if (doc.containsKey("d_02")) {
    doc_send["d_02"] = doc["d_02"];
  }

  if (doc.containsKey("d_03")) {
    doc_send["d_03"] = doc["d_03"];
  }

  if (doc.containsKey("d_04")) {
    doc_send["d_04"] = doc["d_04"];
  }

  if (doc.containsKey("d_05")) {
    doc_send["d_05"] = doc["d_05"];
  }

  String strDocSend = "";
  serializeJson(doc_send, strDocSend);

  uart1.println(strDocSend); //Send jsonControl to UART
  Serial.println(strDocSend);
}

//This function checks the MQTT connect state and reconnect if the connection has an interruption.
void reConnect() {
  // Loop until we're reconnected
  while (!client.connected()) {
    String clientId = "ESP8266Client-";
    clientId += String(random(0xffff), HEX);
    bool connState = client.connect(clientId.c_str(), AIO_USERNAME, AIO_KEY);

    if (connState) {
      client.subscribe("FWpfOR6wyKZIoYj");
      Serial.println("Complete subscribe topic");
    } else {
      Serial.print("failed, rc=");
      Serial.print(client.state());
      Serial.println(" try again in 5 seconds");
      delay(5000);
    }
  }
}

void setup()
{

  Serial.begin(9600);
  uart1.begin(9600);

  WiFi.begin(WIFI_SSID, WIFI_PASSWORD);
  Serial.print("Connecting to Wi-Fi");
  while (WiFi.status() != WL_CONNECTED)
  {
    Serial.print(".");
    delay(300);
  }
  Serial.println();
  Serial.print("Connected with IP: ");
  Serial.println(WiFi.localIP());
  Serial.println();

  //Initial MQTT connect and set default callback function.
  client.setServer(AIO_SERVER, 1883);
  client.setCallback(callback);

  //Initialize which function will be called when the thread start and interval time to start this thread. 
  readUARTThread->onRun(readUARTCallback);
  readUARTThread->setInterval(100);
  chart2MqttThread->onRun(putChartCallback);
  chart2MqttThread->setInterval(1000);

  device2MqttThread->onRun(putDeviceCallback);
  device2MqttThread->setInterval(1000);

  //Add thread list into thread pipeline for easier to debug.
  controll.add(readUARTThread);
  controll.add(chart2MqttThread);
  controll.add(device2MqttThread);
}

void loop()
{
  if (!client.connected()) {
    reConnect();
  }
  client.loop();

  //Start the thread controller.
  controll.run();
}

// callback for readUARTCallback
void readUARTCallback() {
  if (uart1.available() > 0) {
    String jsonCharts;
    while (uart1.available() > 0) {
      // read the incoming byte:
      char incomingByte = uart1.read();
      jsonCharts = jsonCharts + incomingByte + "";
    }
    Serial.println(jsonCharts);
    mem.setState(jsonCharts); //Lưu lại trạng thái sau này dùng
  }
}


//This function will push the temperature, humidity and ground humidity into the MQTT topic.
void putChartCallback() {
  StaticJsonDocument<128> updateData;

  String chart_01 = String(mem.getChart01());
  String chart_02 = String(mem.getChart02());
  String chart_03 = String(mem.getChart03());
  
  updateData["chart_01"] = chart_01;
  updateData["chart_02"] = chart_02;
  updateData["chart_03"] = chart_03;

  String strDocSend = "";
  char msg[128];

  serializeJson(updateData, strDocSend);
  strDocSend.toCharArray(msg, 128);
  client.publish("Qx3rlKNqKPJAISs", msg);
}


//This function will push the state of devices into the MQTT topic.
void putDeviceCallback() {
  StaticJsonDocument<128> updateData;

  int d01 = mem.getDevice01();
  int d02 = mem.getDevice02();
  int d03 = mem.getDevice03();
  int d04 = mem.getDevice04();
  int d05 = mem.getDevice05();

  updateData["d01"] = d01;
  updateData["d02"] = d02;
  updateData["d03"] = d03;
  updateData["d04"] = d04;
  updateData["d05"] = d05;
  
  String strDocSend = "";
  char msg[128];

  serializeJson(updateData, strDocSend);
  strDocSend.toCharArray(msg, 128);
  Serial.println(msg);
  client.publish("Qx3rlKNqKPJAISs", msg);
}
