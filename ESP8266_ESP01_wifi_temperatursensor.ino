#include <ESP8266WiFi.h>
#include <BME280I2C.h>
#include <Wire.h>

#define SEALEVELPRESSURE_HPA (1013.25)
#define BME280_ADDRESS (0x76)
//#define BME280_ADDRESS (0x77)

BME280I2C bme; // I2C
WiFiServer server(80);

// WiFi Router Login
const char* SSID = "WLAN_SSID";
const char* PASSWORD = "WLAN_PW";
const char* HOSTNAME = "HOSTNAME";
const char* SENSORID = "Sensor Aussen";

void setup() {
  Serial.begin(115200);
  delay(10);
  Serial.println();

  // Setup I2C
  Wire.begin(0, 2);

  // Connect to the WiFi network (see function below loop)
  // move this function to loop to see if the connection is more stable
  // connectToWiFi(SSID, password);

  checkSensor();

  // Connect to WiFi network
  Serial.println();
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

  // Start the server
  server.begin();
  Serial.println("Server started");

  // Print the IP address
  Serial.print("Use this URL to connect: ");
  Serial.print("http://");
  Serial.print(WiFi.localIP());
  Serial.println("/");
}

void checkSensor()
{
  bool status;
  status = bme.begin();
  if (!status) {
    Serial.println("Could not find a valid BME280 sensor, check wiring!");
    while (1);
  }
  Serial.println("Sensor found");
  delay(1000); // let sensor boot up
}

void loop() {
  // Check if a client has connected
  WiFiClient client = server.available();
  if (!client) {
    return;
  }

  // Wait until the client sends some data
  Serial.println("new client");
  while(!client.available()){
    delay(1);
  }

  // Read the first line of the request
  String request = client.readStringUntil('\r');
  Serial.println(request);
  client.flush();

  // Match the request
  if (request.indexOf("/id") != -1) {
    // Return the response
    client.println("HTTP/1.1 200 OK");
    client.println("Content-Type: text/html");
    client.println(""); //  do not forget this one
/*
    client.println("<!DOCTYPE HTML>");
    client.println("<html>");
*/
    client.print(SENSORID);
//    client.println("</html>");
    delay(1);
  }
  
  if (request.indexOf("/temperature") != -1) {
    // Read and send data
    float Pressure, Temperature, Humidity;
    bme.read(Pressure, Temperature, Humidity);
    Serial.print("Temperature: ");
    Serial.println(Temperature);
    Serial.print("Pressure: ");
    Serial.println(Pressure / 100.0F);
    Serial.print("Humidity: ");
    Serial.println(Humidity);
    // Return the response
    client.println("HTTP/1.1 200 OK");
    client.println("Content-Type: text/html");
    client.println(""); //  do not forget this one
    client.print(String(Temperature));
    delay(1);
  }
    if (request.indexOf("/pressure") != -1) {
    // Read and send data
    float Pressure, Temperature, Humidity;
    bme.read(Pressure, Temperature, Humidity);
    // Return the response
    client.println("HTTP/1.1 200 OK");
    client.println("Content-Type: text/html");
    client.println(""); //  do not forget this one
    client.print(String(Pressure/100.0F));
    delay(1);
  }
  if (request.indexOf("/humidity") != -1) {
    // Read and send data
    float Pressure, Temperature, Humidity;
    bme.read(Pressure, Temperature, Humidity);
    // Return the response
    client.println("HTTP/1.1 200 OK");
    client.println("Content-Type: text/html");
    client.println(""); //  do not forget this one
    client.print(String(Humidity));
    delay(1);
  }
}
