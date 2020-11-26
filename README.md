# DHT22-to-InfluxDB
Temperature and humudity logger to InfluxDB using basic circuit with esp8266 and DTH22


![sensors](https://github.com/mariusmotea/DHT22-to-InfluxDB/blob/main/sensors.PNG?raw=true)


### Requirements

 - free InfluDB Cloud account
 - Arduino IDE
 - 1 x ESP-M2 module
 - 1 x DHT22 sensor
 - 1 x 20700 Li battery
 - 1 x MCP1700T-3302E/TT power regulator
 - 1 x 320k SMD resistor (or 100K + 220k in serial)
 - 1 x 470ohm SMD resistor
 - 1 x 100k SMD resistor
 - 1 x 10k SMD resistor
 - 1 x 5k6 SMD resistor
 - 1 x 100nf SMD capacitor
 - 1 x 1uf SMD capacitor
 
### Software

In the header of this sketch you need to edit InfluxDB account credentials and the sensor name. Wifi credentials are setup using WiFi Manager (https://github.com/tzapu/WiFiManager). On first power on the device will remain ~1 minute alive in order to allow debug and firmware upgrades (using /update path), after this the device will go in deep sleep and will wake up every 5 minutes in order to measure and send the data to InfluxDB. ESP-M2 is based on esp8285 and has only 1MB of flash, in order to allow firmware updates select `Flash size 1MB, FS: none, OTA: 502KB`.
 
### The circuit

![circuit](https://github.com/mariusmotea/DHT22-to-InfluxDB/blob/main/circuit.png?raw=true)

All components except the two resistors used for voltage divider are mandatory. The DHT22 need to be powered directly from the battery for stability, otherwise it will fail to measure during esp wake-up from deep sleep. I solder all components directly on the modules as you can see in below image:

![assembly](https://github.com/mariusmotea/DHT22-to-InfluxDB/blob/main/assembly.png?raw=true)

### The case

Print the case using 0.2mm layer height with the body rotated 180 degree (the top on the printer bed) and support enabled everywhere for the DHT22 hole.
