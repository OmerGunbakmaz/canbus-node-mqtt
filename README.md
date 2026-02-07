# CAN Bus to MQTT Bridge - DHT22 Sensor

An IoT project that reads temperature and humidity from a DHT22 sensor via STM32F407, transmits over CAN Bus to a Raspberry Pi, publishes to MQTT, and visualizes on a Node-RED dashboard.

```
DHT22 → STM32F407 → CAN Bus (500kbps) → MCP2515 → Raspberry Pi → MQTT → Node-RED
```

## Hardware

**STM32 Side:**
- STM32F407VGT6 Discovery Board
- DHT22 sensor (Data → PE3)
- CAN transceiver (TJA1050 / MCP2551) — CAN_TX → PD1, CAN_RX → PD0

**Raspberry Pi Side:**
- Can Tranceiver *SN65HVD230* 
- Raspberry Pi 3/4 with MCP2515 CAN module (SPI)
- 120Ω termination resistors on both ends

**LED Indicators (Discovery Board):**
- Green (PD12) — CAN initialized
- Orange (PD13) — Data transmitting (toggles every 500ms)
- Red (PD14) — CAN init error

## Project Structure

```
├── hardware/
│   └── stm32/                # STM32 firmware (CubeIDE project)
│       ├── Core/Src/
│       │   ├── main.c        # Main loop: read     DHT22, send over CAN
│       │   ├── can.c         # CAN driver(ID: 0x100, 4 bytes)
│       │   └── dht22.c       # DHT22 bit-bang driver using TIM3
│       └── Core/Inc/
│           ├── can.h
│           └── dht22.h
│
├── software/
│   ├── raspberry/
│   │   └── can2mqtt.py       # CAN→MQTT bridge script
│   └── nodered/
│       └── flows.json        # Node-RED dashboard flow
│
└── README.md
```

## Setup

### Raspberry Pi

```bash
# CAN interface
sudo apt-get install can-utils
# Add to /boot/config.txt:
#   dtparam=spi=on
#   dtoverlay=mcp2515-can0,oscillator=8000000,interrupt=25
sudo reboot

sudo ip link set can0 type can bitrate 500000
sudo ip link set can0 up

# MQTT & Python
sudo apt-get install mosquitto mosquitto-clients
pip3 install python-can paho-mqtt

# Run
python3 software/raspberry/can2mqtt.py
```

### STM32

Open `hardware/stm32` in STM32CubeIDE, build and flash.

### Node-RED

Start Node-RED, import `software/nodered/flows.json`. Dashboard at `http://localhost:1880`

## Data Format

CAN ID `0x100` carries 4 bytes: `[TempH, TempL, HumH, HumL]` (values × 10, big-endian signed).

Example: `[00 FF 01 2C]` → Temperature: 25.5°C, Humidity: 30.0%

MQTT topic `sensor/dht22`:
```json
{ "temperature": 25.5, "humidity": 30.0, "unit_temp": "C", "unit_hum": "%" }
```

**Ömer Günbakmaz** — [@OmerGunbakmaz](https://github.com/OmerGunbakmaz)
