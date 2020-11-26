#include <FS.h>
#include <ESP8266WiFi.h>
#include <ESP8266mDNS.h>
#include <WiFiUdp.h>
#include <ESP8266HTTPUpdateServer.h>
#include <ESP8266WebServer.h>
#include <WiFiManager.h>
#include <DHT.h>
#include <InfluxDbClient.h>
#include <InfluxDbCloud.h>

#define DEVICE_NAME "Temp #3"
#define DHTPIN 5     // Digital pin connected to the DHT sensor
#define DHTTYPE    DHT22     // DHT 22 (AM2302)

#define INFLUXDB_URL "https://eu-central-1-1.aws.cloud2.influxdata.com"
// InfluxDB v2 server or cloud API authentication token (Use: InfluxDB UI -> Data -> Tokens -> <select token>)
#define INFLUXDB_TOKEN "ayIsLDXI63f8bPfVqVJyxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxqJfbNWXJJxjA3OTHVS4YbuvHow=="
// InfluxDB v2 organization id (Use: InfluxDB UI -> User -> About -> Common Ids )
#define INFLUXDB_ORG "marius.motea@example.com"
// InfluxDB v2 bucket name (Use: InfluxDB UI ->  Data -> Buckets)
#define INFLUXDB_BUCKET "marius.motea's Bucket"


int counter;
// current temperature & humidity, updated in loop()
float t = 0.0;
float h = 0.0;
float Vin = 0.00;
int val = 0;

// Set timezone string according to https://www.gnu.org/software/libc/manual/html_node/TZ-Variable.html
// Examples:
//  Pacific Time: "PST8PDT"
//  Eastern: "EST5EDT"
//  Japanesse: "JST-9"
//  Central Europe: "CET-1CEST,M3.5.0,M10.5.0/3"
#define TZ_INFO "CET-1CEST,M3.5.0,M10.5.0/3"

// InfluxDB client instance with preconfigured InfluxCloud certificate
InfluxDBClient client(INFLUXDB_URL, INFLUXDB_ORG, INFLUXDB_BUCKET, INFLUXDB_TOKEN, InfluxDbCloud2CACert);

extern "C" {
#include "user_interface.h"
}

ESP8266WebServer server(80);
ESP8266HTTPUpdateServer httpUpdateServer;
DHT dht(DHTPIN, DHTTYPE);


void goingToSleep() {
  yield();
  delay(100);
  ESP.deepSleep(300e6); //every 300 seconds => 5 minutes
  yield();
  delay(100);
}


void readSensor() {
  t = dht.readTemperature();
  // Read temperature as Fahrenheit (isFahrenheit = true)
  //float newT = dht.readTemperature(true);
  // if temperature read failed, don't change t value
  if (isnan(t)) {
    delay(50);
    t = dht.readTemperature();
  }
  // Read Humidity
  h = dht.readHumidity();
  if (isnan(h)) {
    delay(50);
    h = dht.readHumidity();
  }
}

// Data point
Point sensor("DHT22");

void setup()  {
  dht.begin();
  delay(500);
  WiFi.mode(WIFI_STA);
  pinMode(A0, INPUT); //assigning the input port
  WiFiManager wifiManager;
  wifiManager.setConfigPortalTimeout(120);
  wifiManager.autoConnect("TempSensor");

  rst_info *rinfo;
  rinfo = ESP.getResetInfoPtr();

  if ((*rinfo).reason != REASON_DEEP_SLEEP_AWAKE) {

    httpUpdateServer.setup(&server);

    server.on("/", []() {
      val = analogRead(A0);//reads the analog input
      Vin = (val * 4.2) / 1024.00;
      readSensor();
      server.send(200, "text/plain", "Umiditate " + String(h) + "%, Temp " + String(t) + ", Bat " + String(Vin) + "V");
    });

    server.begin();
  } else {
    val = analogRead(A0);//reads the analog input
    Vin = (val * 4.2) / 1024.00;
    readSensor();
    sensor.addTag("device", DEVICE_NAME);
    sensor.addField("temp", t);
    sensor.addField("hum", h);
    sensor.addField("bat", Vin);
    client.writePoint(sensor);
    goingToSleep();
    delay(10000);

  }
}

void loop() {
  server.handleClient();
  delay(20);
  counter++;
  if (counter == 5000) {
    goingToSleep();
  }
}
