// #include "secrets3.h"
// #include <Arduino.h>
// #include <WiFiClientSecure.h>
// #include <MQTTClient.h>
// #include <ArduinoJson.h>
// #include "WiFi.h"

// // LED
// #define RED_PIN 22
// #define GREEN_PIN 21
// #define BLUE_PIN 17

// #define RED_CHANNEL 1
// #define BLUE_CHANNEL 2
// #define GREEN_CHANNEL 3

// #define FREQ 12000
// #define RESOLUTION 8

// // Button
// #define BUTTON_PIN 16

// // The MQTT topics that this device should publish/subscribe
// #define AWS_IOT_PUBLISH_TOPIC   "esp32/pub"
// #define AWS_IOT_SUBSCRIBE_TOPIC "esp32/sub"

// WiFiClientSecure net = WiFiClientSecure();
// MQTTClient client = MQTTClient(256);

// void blink(int id)
// {
// 	int r;
// 	int g;
// 	int b;

// 	switch (id)
// 	{
// 		case 1:
// 			r = 0;
// 			g = 0;
// 			b = 255;
// 			break;
// 		case 2:
// 			r = 255;
// 			g = 255;
// 			b = 0;
// 			break;
// 		case 3:
// 			r = 0;
// 			g = 255;
// 			b = 0;
// 			break;
// 		case 4:
// 			r = 255;
// 			g = 0;
// 			b = 0;
// 			break;
// 	}

// 	ledcWrite(RED_CHANNEL, r);
// 	ledcWrite(GREEN_CHANNEL, g);
// 	ledcWrite(BLUE_CHANNEL, b);
// 	delay(500);
// 	ledcWrite(RED_CHANNEL, 0);
// 	ledcWrite(GREEN_CHANNEL, 0);
// 	ledcWrite(BLUE_CHANNEL, 0);
// }

// void messageHandler(String &topic, String &payload) {
//   	Serial.println("incoming: " + topic + " - " + payload);

// 	StaticJsonDocument<200> doc;
// 	deserializeJson(doc, payload);
// 	const int id = doc["id"];
	
// 	if (id != ID)
// 	{
// 		const Color color = doc["color"];
// 		blink(id);
// 	}
// }

// void connectAWS()
// {
// 	WiFi.mode(WIFI_STA);
// 	WiFi.begin(WIFI_SSID, WIFI_PASSWORD);

// 	Serial.println("Connecting to Wi-Fi");

//   	while (WiFi.status() != WL_CONNECTED){
//     	delay(500);
//     	Serial.print(".");
//   	}

//   	// Configure WiFiClientSecure to use the AWS IoT device credentials
//   	net.setCACert(AWS_CERT_CA);
//   	net.setCertificate(AWS_CERT_CRT);
//   	net.setPrivateKey(AWS_CERT_PRIVATE);

//   	// Connect to the MQTT broker on the AWS endpoint we defined earlier
//   	client.begin(AWS_IOT_ENDPOINT, 8883, net);

//   	// Create a message handler
//   	client.onMessage(messageHandler);

//   	Serial.print("Connecting to AWS IOT");

//   	while (!client.connect(THINGNAME)) {
//     	Serial.print(".");
//     	delay(100);
//   	}

//   	if (!client.connected()){
//     	Serial.println("AWS IoT Timeout!");
//     	return;
//   	}

//   	// Subscribe to a topic
//   	client.subscribe(AWS_IOT_SUBSCRIBE_TOPIC);

//   	Serial.println("AWS IoT Connected!");
// }

// void publishMessage()
// {
//   	StaticJsonDocument<200> doc;
//   	doc["time"] = millis();
// 	doc["id"] = ID;
// 	doc["color"] = COLOR;
//   	char jsonBuffer[512];
//   	serializeJson(doc, jsonBuffer); // print to client

//   	client.publish(AWS_IOT_PUBLISH_TOPIC, jsonBuffer);
// }

// void setup() {
//   	Serial.begin(9600);
// 	delay(10);

//   	connectAWS();

// 	pinMode(BUTTON_PIN, INPUT);

// 	ledcAttachPin(RED_PIN, RED_CHANNEL);
// 	ledcAttachPin(GREEN_PIN, GREEN_CHANNEL);
// 	ledcAttachPin(BLUE_PIN, BLUE_CHANNEL);

// 	ledcSetup(RED_CHANNEL, FREQ, RESOLUTION);
// 	ledcSetup(GREEN_CHANNEL, FREQ, RESOLUTION);
// 	ledcSetup(BLUE_CHANNEL, FREQ, RESOLUTION);
// }

// void loop() {
// 	int buttonState = digitalRead(BUTTON_PIN);
// 	if (buttonState == HIGH) {
// 		publishMessage();
// 		blink(ID);
// 	}
//   	client.loop();
//   	delay(1000);
// }