#!/usr/bin/env python3
# -*- coding: utf-8 -*-
import can
import paho.mqtt.client as mqtt
import json
import struct

# MQTT settings
MQTT_BROKER = "localhost"
MQTT_PORT = 1883

# CAN settings
CAN_INTERFACE = "can0"
DHT22_CAN_ID = 0x100

# MQTT connection
client = mqtt.Client()
client.connect(MQTT_BROKER, MQTT_PORT, 60)

# CAN connection
bus = can.interface.Bus(channel=CAN_INTERFACE, bustype='socketcan')

print("DHT22 CAN -> MQTT Bridge started...")
print("Expected: CAN ID 0x100, 4 byte [temp_high,temp_low,hum_high,hum_low]")
print("-" * 50)

while True:
    message = bus.recv()

    if message.arbitration_id == DHT22_CAN_ID:
        # 4 byte: 2 byte temperature + 2 byte humidity
        # '>hh' = big-endian, 2  signed shorts (for negative temperature)
        temp_raw, hum_raw = struct.unpack('>hh', message.data[:4])

        # DHT22  sends (real value * 10)  (25.5 Celcius = 255)
        temp = temp_raw / 10.0
        hum = hum_raw / 10.0

        # Publsih to MQTT
        payload = json.dumps({
            "temperature": temp,
            "humidity": hum,
            "unit_temp": "C",
            "unit_hum": "%"
        })

        client.publish("sensor/dht22", payload)
        client.publish("sensor/temp", json.dumps({"value": temp}))
        client.publish("sensor/hum", json.dumps({"value": num}))

        print(f"Temperature: {temp}C | Humidity: %{hum}")
