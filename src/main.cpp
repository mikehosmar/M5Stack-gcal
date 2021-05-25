

#define M5STACK_MPU6886
#include <Arduino.h>
#include <HTTPClient.h>
#include <M5Stack.h>
#include <WiFi.h>
#include <PubSubClient.h>

// #include "display.h"
// #include "motion.h"

// The screen is 320 x 240 pixels
#define LCD_MAX_X 320
#define LCD_MAX_Y 240
#define BACKGROUND 0xFA00

WiFiClient espClient;
PubSubClient client(espClient);
unsigned long lastMsg = 0;
#define MSG_BUFFER_SIZE (50)
char msg[MSG_BUFFER_SIZE];
int value = 0;

bool callback(char *topic, byte *payload, unsigned int length);
void reconnect();

String mqtt_user;
String mqtt_pass;

// the setup routine runs once when M5Stack starts up
void setup()
{
  // Initialize the M5Stack object
  M5.begin();
  Wire.begin();
  // M5.IMU.Init();  // this line must be after Wire.begin()
  if (!SPIFFS.begin(true))
  {
    Serial.println("SPIFFS Mount Failed");
    return;
  }
  /*
    Power chip connected to gpio21, gpio22, I2C device
    Set battery charging voltage and current
    If used battery, please call this function in your project
  */
  M5.Power.begin();

  // Set up and turn off the speaker output to avoid most of the anoying sounds
  pinMode(25, OUTPUT);
  M5.Speaker.mute();

  M5.Lcd.setTextSize(7);
  M5.Lcd.setBrightness(40);
  M5.Lcd.setTextColor(BLACK, BACKGROUND);
  M5.Lcd.setTextDatum(MC_DATUM);

  M5.Lcd.print("Startup");

  // Wifi connection
  Serial.begin(115200);
  delay(10);
  Serial.println("Connecting");

  auto wifi_info = SPIFFS.open("/wifi.txt");
  if (!wifi_info)
  {
    M5.Lcd.print(
        "Create wifi.txt file with ssid and pass separated by a \";\" in "
        "SPIFFS");
    delay(5);
  }

  String ssid = wifi_info.readStringUntil(';');
  String pass = wifi_info.readString();
  // Serial.printf("Wifi info: %s, %s\n", ssid, pass);
  WiFi.begin(ssid.c_str(), pass.c_str());
  wifi_info.close();

  while (WiFi.status() != WL_CONNECTED)
  {
    delay(100);
    Serial.print(".");
  }

  if (WiFi.status() == WL_CONNECTED)
  {
    Serial.println("");
    Serial.println("WiFi connected");
    Serial.println("IP address: ");
    Serial.println(WiFi.localIP());
  }
  else
  {
    Serial.println("Not connected- Working without WIFI");
  }

  // =======================================================================================
  // Project Specific Setup
  // =======================================================================================

  auto mqtt_info = SPIFFS.open("/mqtt.txt");
  if (!mqtt_info)
  {
    M5.Lcd.print(
        "Create mqtt.txt file with host, user and password separated by a \";\" in "
        "SPIFFS");
    delay(5);
  }

  String mqtt_server = mqtt_info.readStringUntil(';');
  mqtt_user = mqtt_info.readStringUntil(';');
  mqtt_pass = mqtt_info.readString();

  // retieve data periodically
  client.setServer(mqtt_server.c_str(), 1883);
  client.setCallback(callback);
}

// the loop routine runs over and over again forever
void loop()
{
  M5.update();
  if (!client.connected())
  {
    reconnect();
  }
  client.loop();
}

bool callback(char *topic, byte *payload, unsigned int length)
{
  String lcd_text;
  static String data_old, data;
  data_old = data;
  data = "";
  for (int i = 0; i < length; i++)
  {
    data += (char)payload[i];
  }

  if (data_old != data)
  {
    lcd_text = data;
    // lcd_text.remove(0, 1 + (lcd_text.length() > 17)); //only wors for strings shorter than 255
    // if (lcd_text.endsWith("0"))
    //   lcd_text.remove(lcd_text.length() - 1);
    Serial.println(lcd_text);

    M5.Lcd.clear(BACKGROUND);

    int idx = lcd_text.lastIndexOf(";");
    if (idx >= 0)
    {
      String second = lcd_text.substring(idx + 1);
      second = second.substring(12, 17);
      lcd_text.remove(idx);
      M5.Lcd.setTextSize(4);
      int fh = M5.Lcd.fontHeight() / 2 + 4;
      M5.Lcd.drawString(lcd_text, 160, 120 - fh);
      M5.Lcd.drawString(second, 160, 120 + fh);
    }
    else
    {
      M5.Lcd.setTextSize(6);
      M5.Lcd.drawString(lcd_text, 160, 120);
    }
    M5.Lcd.setTextSize(2);
    M5.Lcd.drawString(String(M5.Power.getBatteryLevel()), 0, 0);
  }
  // delay(5000);
  // M5.Power.deepSleep();
}

void reconnect()
{
  // Loop until we're reconnected
  while (!client.connected())
  {
    Serial.print("Attempting MQTT connection...");
    // Create a random client ID
    String clientId = "ESP32Client-CalDisp";
    // Attempt to connect
    if (client.connect(clientId.c_str(), mqtt_user.c_str(), mqtt_pass.c_str()))
    {
      Serial.println("connected");
      // ... and resubscribe
      client.subscribe("calendar/BusyOrNot");
    }
    else
    {
      Serial.print("failed, rc=");
      Serial.print(client.state());
      Serial.println(" try again in 5 seconds");
      // Wait 5 seconds before retrying
      delay(5000);
    }
  }
}
