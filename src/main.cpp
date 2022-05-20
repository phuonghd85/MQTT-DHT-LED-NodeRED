#include <Arduino.h>
#include "DHT.h"
#include <WiFi.h>
#include <PubSubClient.h>
#include "ArduinoJson.h"
#include <Wire.h>
// Thông tin về wifi

#define ssid "2anhem"
#define password "boo112904"
#define mqtt_server "broker.mqtt-dashboard.com"

const uint16_t mqtt_port = 1883; //Port của MQTT

#define topic1 "esp32/temperature"
#define topic2 "esp32/humidity"

#define DHTPIN 5
#define DHTTYPE DHT11

DHT dht(DHTPIN, DHTTYPE);

WiFiClient espClient;
PubSubClient client(espClient);

void setup()
{
  Serial.begin(115200);

  Wire.begin();
  pinMode(A0, INPUT); //gan A0 cho LDR


  setup_wifi();                             //thực hiện kết nối Wifi
  client.setServer(mqtt_server, mqtt_port); // cài đặt server và lắng nghe client ở port 1883
  client.setCallback(callback);             // gọi hàm callback để thực hiện các chức năng publish/subcribe

  //    if (!client.connected())
  //    { // Kiểm tra kết nối
  //      reconnect();
  //    }
  client.subscribe("led");
  pinMode(26, OUTPUT);
  dht.begin();
}

// Hàm kết nối wifi
void setup_wifi()
{
  delay(10);
  Serial.println();
  Serial.print("Connecting to ");
  Serial.println(ssid);
  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED)
  {
    delay(500);
    Serial.print(".");
  }
  // in ra thông báo đã kết nối và địa chỉ IP của ESP8266
  Serial.println("");
  Serial.println("WiFi connected");
  Serial.println("IP address: ");
  Serial.println(WiFi.localIP());
}

// Hàm call back để nhận dữ liệu
void callback(char *topic, byte *payload, unsigned int length)
{
  //-----------------------------------------------------------------
  //in ra tên của topic và nội dung nhận được
  Serial.print("Co tin nhan moi tu topic: ");
  Serial.println(topic);
  char p[length + 1];
  memcpy(p, payload, length);
  p[length] = NULL;
  String message(p);

  if (String(topic) == "led")
  {
    if (message == "on")
    {
      digitalWrite(26, HIGH);
    }
    if (message == "off")
    {
      digitalWrite(26, LOW);
    }
  }
  Serial.println(message);
  Serial.write(payload, length);
  Serial.println();
  //-------------------------------------------------------------------------
}

//Hàm reconnect thực hiện kết nối lại khi mất kết nối với MQTT Broker
void reconnect()
{
  while (!client.connected()) // Chờ tới khi kết nối
  {
    String clientId = "ESP8266Client-Mackcest";
    if (client.connect("ESP8266Client-0123")) //kết nối vào broker
    {
        Serial.println("Đã kết nối:");
        //đăng kí nhận dữ liệu từ topic
      }
      else
      {
        // in ra trạng thái của client khi không kết nối được với broker
        Serial.print("Lỗi:, rc=");
        Serial.print(client.state());
        Serial.println(" try again in 5 seconds");
        // Đợi 5s
        delay(5000);
      }
    }
  }

  unsigned long lastMsg = 0;
  void loop()
  {
    if (!client.connected()){// Kiểm tra kết nối
            reconnect();
    }
    client.loop();

    long now = millis();
    if (now - lastMsg > 2000)
    {
      lastMsg = now;
      int temperature = dht.readTemperature();

      char tempString[8];
      dtostrf(temperature, 1, 2, tempString);
      Serial.print("Temperature: ");
      Serial.println(tempString);
      client.publish("esp32/temperature", tempString);

      int humidity = dht.readHumidity();
      // Convert the value to a char array
      char humString[8];
      dtostrf(humidity, 1, 2, humString);
      Serial.print("Humidity: ");
      Serial.println(humString);
      client.publish("esp32/humidity", humString);
    }
  }