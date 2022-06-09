# Room Weather

Room weather is an embedded IOT system for monitoring temperature and humidity conditions and alerting of anomalous events. 
The system uses the MQTT protocol to publish/subscribe information over a TCP/IP connection to the internet. 
The front end of the system is implemented using the web based Graphical User Interface based on Node-Red flows.

## Getting Started

### Hardware requirements
  - ESP-WROOM-32 Development board
  - DHT11 Temperature/Humidity Sensor
  - Piezo Buzzer
  - 100Ω Resistor(for LED)
  - LED

#### Schematic

![Room-Weather-0](https://github.com/pdewar/Room-Weather/blob/1f2ce739e955589ead66d188c82135e1b2f0f435/images/Circuit%20Setup_bb.jpeg)

#### Pin configuration

| DHT11 Sensor       | ESP32-Wroom Dev Board |
| ------------------ | --------------------  |
| VCC                | 3V3                   |
| GND                | GND                   |
| DATA_PIN           | GPIO_NUM_32           |

| Piezo Buzzer       | ESP32-Wroom Dev Board |
| ------------------ | --------------------  |
| VCC                | GPIO_NUM_25           |
| GND                | GND                   |

| LED                | ESP32-Wroom Dev Board |
| ------------------ | --------------------  |
| Cathode            | Resistor              |
| Anode              | GPIO_NUM_33           |
| Resistor           | GND                   |


### Software requirements

1. [Install Eclipse Mosquitto](https://mosquitto.org/download/)
2. [Install Node-RED](https://nodered.org/docs/getting-started/local)
4. [Install PlatformIO Core](http://docs.platformio.org/page/core.html)
5. Download [project source code](https://github.com/pdewar/Room-Weather/archive/refs/heads/main.zip)
6. Extract ZIP archive
7. Add your wifi ssid and password to Room-Weather-main/Room Weather/include/password.h file 
8. Run these commands:

```sh
# Change directory to project
> cd Room-Weather-main/"Room Weather"

# Build specific environment
> platformio run -e esp32dev

# Upload firmware for the specific environment
> platformio run -e esp32dev --target upload

# Clean build files
> platformio run --target clean
```

9. Run Node-RED

10. Go to local host in web browser: http://localhost:1880

11. Import flow from the right side menu by clicking import and uploading the flow.json file provided 

12. Click deploy button at top of page

13. Go to Node red dashboard in web browser: http://localhost:1880/ui

![Room-Weather-1](https://github.com/pdewar/Room-Weather/blob/1f2ce739e955589ead66d188c82135e1b2f0f435/images/UI.png)

