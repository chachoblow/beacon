#include "beacon-wesley-secrets.h"
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
#define POT_A_PIN 13
#define POT_B_PIN 12

// Amazon Root CA
static const char AWS_ROOT_CA_CERT[] PROGMEM = R"EOF(
-----BEGIN CERTIFICATE-----
MIIDQTCCAimgAwIBAgITBmyfz5m/jAo54vB4ikPmljZbyjANBgkqhkiG9w0BAQsF
ADA5MQswCQYDVQQGEwJVUzEPMA0GA1UEChMGQW1hem9uMRkwFwYDVQQDExBBbWF6
b24gUm9vdCBDQSAxMB4XDTE1MDUyNjAwMDAwMFoXDTM4MDExNzAwMDAwMFowOTEL
MAkGA1UEBhMCVVMxDzANBgNVBAoTBkFtYXpvbjEZMBcGA1UEAxMQQW1hem9uIFJv
b3QgQ0EgMTCCASIwDQYJKoZIhvcNAQEBBQADggEPADCCAQoCggEBALJ4gHHKeNXj
ca9HgFB0fW7Y14h29Jlo91ghYPl0hAEvrAIthtOgQ3pOsqTQNroBvo3bSMgHFzZM
9O6II8c+6zf1tRn4SWiw3te5djgdYZ6k/oI2peVKVuRF4fn9tBb6dNqcmzU5L/qw
IFAGbHrQgLKm+a/sRxmPUDgH3KKHOVj4utWp+UhnMJbulHheb4mjUcAwhmahRWa6
VOujw5H5SNz/0egwLX0tdHA114gk957EWW67c4cX8jJGKLhD+rcdqsq08p8kDi1L
93FcXmn/6pUCyziKrlA4b9v7LWIbxcceVOF34GfID5yHI9Y/QCB/IIDEgEw+OyQm
jgSubJrIqg0CAwEAAaNCMEAwDwYDVR0TAQH/BAUwAwEB/zAOBgNVHQ8BAf8EBAMC
AYYwHQYDVR0OBBYEFIQYzIU07LwMlJQuCFmcx7IQTgoIMA0GCSqGSIb3DQEBCwUA
A4IBAQCY8jdaQZChGsV2USggNiMOruYou6r4lK5IpDB/G/wkjUu0yKGX9rbxenDI
U5PMCCjjmCXPI6T53iHTfIUJrU6adTrCC2qJeHZERxhlbI1Bjjt/msv0tadQ1wUs
N+gDS63pYaACbvXy8MWy7Vu33PqUXHeeE6V/Uq2V8viTO96LXFvKWlJbYK8U90vv
o/ufQJVtMVT8QtPHRh8jrdkPSHCa2XV4cdFyQzR1bldZwgJcJmApzyMZFo6IQ6XU
5MsI+yMRQ+hDKXJioaldXgjUkK642M4UwtBV8ob2xJNDd2ZhwLnoQdeXeGADbkpy
rqXRfboQnoZsG4q5WTP468SQvvG5
-----END CERTIFICATE-----
)EOF";

const char AWS_IOT_ENDPOINT[] = "a23wplj2ed83c6-ats.iot.us-west-2.amazonaws.com";

Preferences preferences;

WiFiManager wiFiManager;
WiFiClientSecure net = WiFiClientSecure();
MQTTClient client = MQTTClient(256);

Adafruit_NeoMatrix matrix = Adafruit_NeoMatrix(8, 8, MATRIX_PIN,
											   NEO_MATRIX_TOP + NEO_MATRIX_RIGHT +
												   NEO_MATRIX_COLUMNS + NEO_MATRIX_PROGRESSIVE,
											   NEO_GRB + NEO_KHZ800);

ESP32Encoder encoder;
long oldEncoderCount = 0;

// At lower levels of brightness, the LED matrix colors are unpredictable (e.g., a yellow/white
//  color is sometimes just yellow).
const uint8_t BRIGHTNESS_START = 5;
const uint8_t MAX_BRIGHTNESS = 25;
const uint8_t MAX_BLINK = 125;
uint8_t brightness = 0;
uint16_t defaultColor = matrix.Color(255, 255, 212);

// === Light methods ===

void blink(int red, int blue, int green)
{
	uint16_t color = matrix.Color(red, green, blue);

	for (int i = BRIGHTNESS_START; i <= MAX_BLINK; i += 10)
	{
		matrix.fillScreen(color);
		matrix.setBrightness(i);
		matrix.show();
		delay(4);
	}

	for (int i = MAX_BLINK; i >= BRIGHTNESS_START; i--)
	{
		matrix.fillScreen(color);
		matrix.setBrightness(i);
		matrix.show();
		delay(15);
	}

	matrix.setBrightness(brightness);
	matrix.fillScreen(defaultColor);
	matrix.show();

	Serial.println("Blink performed");
}

void handleBrightness()
{
	long encoderCount = encoder.getCount();
	boolean brightnessChanged = false;

	if (encoderCount > oldEncoderCount && brightness < MAX_BRIGHTNESS)
	{
		brightness = brightness < BRIGHTNESS_START ? BRIGHTNESS_START : brightness + 1;
		brightnessChanged = true;
	}
	else if (encoderCount < oldEncoderCount && brightness > 0)
	{
		brightness = brightness <= BRIGHTNESS_START ? 0 : brightness - 1;
		brightnessChanged = true;
	}

	oldEncoderCount = encoderCount;

	if (brightnessChanged)
	{
		preferences.begin("beacon");
		preferences.putLong("brightness", brightness);
		preferences.end();
		matrix.setBrightness(brightness);
		matrix.fillScreen(defaultColor);
		matrix.show();
	}
}

// === WiFi methods ===

void connectWiFi()
{
	wiFiManager.autoConnect("BeaconAccessPoint", "MerryChristmasYaFilthyAnimal");
	wiFiManager.setWiFiAutoReconnect(true);
	Serial.println("Connected to " + wiFiManager.getWiFiSSID());

	// Configure WiFiClientSecure to use the AWS IoT device credentials
	net.setCACert(AWS_ROOT_CA_CERT);
	net.setCertificate(AWS_DEVICE_CERT);
	net.setPrivateKey(AWS_PRIVATE_KEY);
	Serial.println("AWS IoT device credentials added");
}

// === AWS methods ===

void messageHandler(String &topic, String &payload)
{
	Serial.println("incoming: " + topic + " - " + payload);

	StaticJsonDocument<200> doc;
	deserializeJson(doc, payload);

	if (doc.containsKey("id"))
	{
		const char *id = doc["id"];
		if (strcmp(id, ID) != 0)
		{
			int red = doc["red"];
			int blue = doc["blue"];
			int green = doc["green"];
			blink(red, blue, green);
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
	while (!client.connect(THINGNAME))
	{
		Serial.print(".");
		delay(100);
	}
	Serial.println();

	if (!client.connected())
	{
		Serial.println("AWS IoT Timeout!");
		return;
	}

	for (auto topic : AWS_IOT_SUBSCRIBE_TOPICS)
	{
		Serial.println(topic);
		client.subscribe(topic);
	}

	Serial.println("Connected");
}

void publishMessage()
{
	StaticJsonDocument<200> doc;
	doc["time"] = millis();
	doc["id"] = ID;
	doc["red"] = RED;
	doc["blue"] = BLUE;
	doc["green"] = GREEN;
	char jsonBuffer[512];
	serializeJson(doc, jsonBuffer); // print to client

	for (auto topic : AWS_IOT_PUBLISH_TOPICS)
	{
		Serial.println(topic);
		client.publish(topic, jsonBuffer);
	}

	Serial.println("Published message");
}

// === Setup methods ===

void setupMatrix()
{
	matrix.begin();
	matrix.setBrightness(BRIGHTNESS_START);
}

void flash()
{
	matrix.fillScreen(matrix.Color(255, 255, 255));
	matrix.show();
	delay(500);
	matrix.fillScreen(matrix.Color(0, 0, 0));
	matrix.show();
	delay(500);
}

void setupEncoder()
{
	pinMode(BUTTON_PIN, INPUT_PULLUP);

	ESP32Encoder::useInternalWeakPullResistors = UP;
	encoder.attachHalfQuad(POT_A_PIN, POT_B_PIN);
	oldEncoderCount = encoder.getCount();
}

void initializeMatrixColorAndBrightness()
{
	preferences.begin("beacon");
	brightness = preferences.getLong("brightness", 1);
	preferences.end();

	matrix.setBrightness(brightness);
	matrix.fillScreen(defaultColor);
	matrix.show();
}

// === Core methods ===

void setup()
{
	Serial.begin(9600);
	delay(10);

	Serial.println("Setup started");

	setupMatrix();
	flash();
	connectWiFi();
	flash();
	connectAWS();
	flash();
	setupEncoder();
	initializeMatrixColorAndBrightness();

	Serial.println("Setup complete");
}

void loop()
{
	if (!client.connected())
	{
		Serial.println("AWS IOT disconnected");
		connectAWS();
	}

	int buttonState = digitalRead(BUTTON_PIN);
	if (buttonState == LOW)
	{
		publishMessage();
		blink(RED, BLUE, GREEN);
	}

	client.loop();
	handleBrightness();
}