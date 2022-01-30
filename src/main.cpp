#include "secrets1.h"
#include <Arduino.h>
#include <WiFiClientSecure.h>
#include <MQTTClient.h>
#include <ArduinoJson.h>
#include "WiFi.h"
#include <Adafruit_GFX.h>
#include <Adafruit_NeoMatrix.h>
#include <Adafruit_NeoPixel.h>
#include <WiFiManager.h>
#include <ESP32Encoder.h>
#include <Preferences.h>

#define BUTTON_PIN 22
#define MATRIX_PIN 23
#define POT_A_PIN 12
#define POT_B_PIN 13

// The MQTT topics that this device should publish/subscribe
#define AWS_IOT_PUBLISH_TOPIC   "esp32/pub"
#define AWS_IOT_SUBSCRIBE_TOPIC "esp32/sub"

Preferences preferences;

WiFiManager wiFiManager;
WiFiClientSecure net = WiFiClientSecure();
MQTTClient client = MQTTClient(256);

Adafruit_NeoMatrix matrix = Adafruit_NeoMatrix(8, 8, MATRIX_PIN,
  NEO_MATRIX_TOP     + NEO_MATRIX_RIGHT +
  NEO_MATRIX_COLUMNS + NEO_MATRIX_PROGRESSIVE,
  NEO_GRB            + NEO_KHZ800);

ESP32Encoder encoder;
long oldEncoderCount = 0;

const uint8_t MAX_BRIGHTNESS = 125;
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
		default:
			color = matrix.Color(255, 255, 255);
			break;   
	}
	
	for (int i = 0; i <= MAX_BRIGHTNESS; i+=10)
	{
		matrix.fillScreen(color);
		matrix.setBrightness(i);
		matrix.show();
		delay(2);
	}

	for (int i = MAX_BRIGHTNESS; i >= 0; i--)
	{
		matrix.fillScreen(color);
		matrix.setBrightness(i);
		matrix.show();
		delay(10);
	}

	matrix.setBrightness(brightness);
	matrix.fillScreen(defaultColor);
	matrix.show();

	inBlink--;
	Serial.println("Blink performed");
}

void connectWiFi()
{
	wiFiManager.autoConnect("LightBoxAccessPoint", "merryChristmas");
	wiFiManager.setWiFiAutoReconnect(true);
	Serial.println("Connected to " + wiFiManager.getWiFiSSID());

  	// Configure WiFiClientSecure to use the AWS IoT device credentials
  	net.setCACert(AWS_CERT_CA);
  	net.setCertificate(AWS_CERT_CRT);
  	net.setPrivateKey(AWS_CERT_PRIVATE);
	Serial.println("AWS IoT device credentials added");
}

void messageHandler(String &topic, String &payload) {
  	Serial.println("incoming: " + topic + " - " + payload);

	StaticJsonDocument<200> doc;
	deserializeJson(doc, payload);

	if (doc.containsKey("id"))
	{
		int id = doc["id"];
		if (id != ID)
		{
			blink(id);
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
  	char jsonBuffer[512];
  	serializeJson(doc, jsonBuffer); // print to client

  	client.publish(AWS_IOT_PUBLISH_TOPIC, jsonBuffer);

	Serial.println("Published message");
}

void setup() {
  	Serial.begin(9600);
	delay(10);

	matrix.begin();
	matrix.fillScreen(matrix.Color(0, 0, 0));
	matrix.show();

	connectWiFi();
  	connectAWS();

	pinMode(BUTTON_PIN, INPUT_PULLUP);

	ESP32Encoder::useInternalWeakPullResistors=UP;
	encoder.attachHalfQuad(POT_A_PIN, POT_B_PIN);
	oldEncoderCount = encoder.getCount();

	preferences.begin("light-box", false);
	brightness = preferences.getLong("brightness", 1);
	preferences.end();

	matrix.setBrightness(brightness);
	matrix.fillScreen(defaultColor);
	matrix.show();

	Serial.println("Setup complete");
}

void loop() {
  	if (!client.connected()){
		Serial.println("AWS IOT disconnected");
    	connectAWS();
  	}

	int buttonState = digitalRead(BUTTON_PIN);
	if (buttonState == LOW) {
		publishMessage();
		blink(ID);
	}

  	client.loop();
	
	if (inBlink == 0) 
	{
		long encoderCount = encoder.getCount();
		boolean brightnessChanged = false;

		if (encoderCount > oldEncoderCount && brightness < MAX_BRIGHTNESS)
		{
			brightness++;
			brightnessChanged = true;
		}
		else if (encoderCount < oldEncoderCount && brightness > 0)
		{
			brightness--;
			brightnessChanged = true;
		}

		oldEncoderCount = encoderCount;

		if (brightnessChanged)
		{
			preferences.begin("light-box", false);
			preferences.putLong("brightness", brightness);
			preferences.end();
			matrix.setBrightness(brightness);
			matrix.fillScreen(defaultColor);
			matrix.show();
		}
	}
}