#include "secrets2.h"
#include <Arduino.h>
#include <WiFiClientSecure.h>
#include <MQTTClient.h>
#include <ArduinoJson.h>
#include "WiFi.h"
#include <Adafruit_GFX.h>
#include <Adafruit_NeoMatrix.h>
#include <Adafruit_NeoPixel.h>
#include <Preferences.h>

// Button
#define BUTTON_PIN 16

// Matrix
#define MATRIX_PIN 23

// Potentiometer
#define POT_PIN 36

// The MQTT topics that this device should publish/subscribe
#define AWS_IOT_PUBLISH_TOPIC   "esp32/pub"
#define AWS_IOT_SUBSCRIBE_TOPIC "esp32/sub"

Preferences preferences;

WiFiClientSecure net = WiFiClientSecure();
MQTTClient client = MQTTClient(256);

Adafruit_NeoMatrix matrix = Adafruit_NeoMatrix(8, 8, MATRIX_PIN,
  NEO_MATRIX_TOP     + NEO_MATRIX_RIGHT +
  NEO_MATRIX_COLUMNS + NEO_MATRIX_PROGRESSIVE,
  NEO_GRB            + NEO_KHZ800);

uint8_t brightness = 0;
uint16_t defaultColor = matrix.Color(255, 255, 255);
uint inBlink = 0;

void blink(int id)
{
	inBlink++;

	uint16_t color = 0;
	switch (id)
	{
		case 1:
			color = matrix.Color(0, 0, 255);
			break;
		case 2:
			color = matrix.Color(255, 255, 0);
			break;
		case 3:
			color = matrix.Color(0, 255, 0);
			break;
		case 4:
			color = matrix.Color(255, 0, 0);
			break;
	}
	
	for (int i = 0; i <= 255; i+=10)
	{
		matrix.fill(color);
		matrix.setBrightness(i);
		matrix.show();
		delay(2);
	}

	for (int i = 255; i >= 0; i--)
	{
		matrix.fill(color);
		matrix.setBrightness(i);
		matrix.show();
		delay(4);
	}

	matrix.fill();
	matrix.show();

	inBlink--;
}

void connectWiFi()
{
	WiFi.mode(WIFI_STA);

	preferences.begin("credentials", false);
	String ssid = preferences.getString("ssid", "");
	String password = preferences.getString("password", "");

	if (ssid == "" || password == "")
	{
		Serial.println("Using default credentials");
		WiFi.begin(WIFI_SSID, WIFI_PASSWORD);
		Serial.print("Connecting to " + String(WIFI_SSID));
	}
	else
	{
		Serial.println("Using saved credentials");
		Serial.println("SSID: " + ssid);
		Serial.println("Password: " + password);
		WiFi.begin(ssid.c_str(), password.c_str());
		Serial.print("Connecting to " + ssid);
		
	}

	preferences.end();

	int retries = 0;
  	while (WiFi.status() != WL_CONNECTED && retries < 20) {
    	delay(500);
    	Serial.print(".");
		retries++;
  	}

	if (retries == 20)
	{
		Serial.println("Using default credentials");
		WiFi.begin(WIFI_SSID, WIFI_PASSWORD);
		Serial.print("Connecting to " + String(WIFI_SSID));
	}

	Serial.println();
	Serial.println("Connected");

  	// Configure WiFiClientSecure to use the AWS IoT device credentials
  	net.setCACert(AWS_CERT_CA);
  	net.setCertificate(AWS_CERT_CRT);
  	net.setPrivateKey(AWS_CERT_PRIVATE);
}

void messageHandler(String &topic, String &payload) {
  	Serial.println("incoming: " + topic + " - " + payload);

	StaticJsonDocument<200> doc;
	deserializeJson(doc, payload);

	if (doc.containsKey("ssid") && doc.containsKey("password"))
	{
		const String ssid = doc["ssid"];
		const String password = doc["password"];

		preferences.begin("credentials", false);
		preferences.putString("ssid", ssid);
		preferences.putString("password", password);

		Serial.println("Saved new credentials");
		Serial.println("SSID: " + ssid);
		Serial.println("Password: " + password);

		preferences.end();

		connectWiFi();
	}

	if (doc.containsKey("id"))
	{
		int id = doc["id"];
		if (id != ID)
		{
			for (int i = 0; i < 1; i++)
			{
				blink(id);
			}
		}
	}
}

void connectAWS()
{
  	// Connect to the MQTT broker on the AWS endpoint we defined earlier
  	client.begin(AWS_IOT_ENDPOINT, 8883, net);

  	// Create a message handler
  	client.onMessage(messageHandler);

  	Serial.print("Connecting to AWS IOT");

  	while (!client.connect(THINGNAME)) {
    	Serial.print(".");
    	delay(100);
  	}

	Serial.println();

  	if (!client.connected()){
    	Serial.println("AWS IoT Timeout!");
    	return;
  	}

  	// Subscribe to a topic
  	client.subscribe(AWS_IOT_SUBSCRIBE_TOPIC);

  	Serial.println("Connected");
}

void publishMessage()
{
  	StaticJsonDocument<200> doc;
  	doc["time"] = millis();
	doc["id"] = ID;
	doc["color"] = COLOR;
  	char jsonBuffer[512];
  	serializeJson(doc, jsonBuffer); // print to client

  	client.publish(AWS_IOT_PUBLISH_TOPIC, jsonBuffer);
}

void setup() {
  	Serial.begin(9600);
	delay(10);

	connectWiFi();
  	connectAWS();

	pinMode(BUTTON_PIN, INPUT);

	matrix.begin();
	matrix.show();
}

void loop() {
	if (WiFi.status() != WL_CONNECTED)
	{
		Serial.println("WiFi disconnected");
		connectWiFi();
	}

  	if (!client.connected()){
		Serial.println("AWS IOT disconnected");
    	connectAWS();
  	}

	int buttonState = digitalRead(BUTTON_PIN);
	if (buttonState == HIGH) {
		publishMessage();
		blink(ID);
	}

  	client.loop();

	if (inBlink == 0) 
	{
		int potValue = analogRead(POT_PIN);
		brightness = map(potValue, 0, 4095, 0, 255);
		matrix.setBrightness(brightness);
		matrix.fill(defaultColor);
		matrix.show();
	}
}