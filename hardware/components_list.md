# 🔧 Components List – Solar Powered RO System

This document lists all the major hardware components used in the **Solar Powered Reverse Osmosis (RO) Water Purification System**, along with their purpose in the system.

---

## ⚡ Power Supply Components

| Component | Specification | Purpose |
|---------|---------------|---------|
| Solar Panel | 12V, 20–40W | Primary renewable power source |
| Solar Charge Controller | 12V PWM | Regulates charging and protects battery |
| Rechargeable Battery | 12V Lead Acid / Li-ion | Stores solar energy for continuous operation |
| Buck Converter / Regulator | 12V → 5V | Supplies regulated power to electronics |

---

## 💧 Water Purification Components

| Component | Specification | Purpose |
|---------|---------------|---------|
| RO Membrane | Standard RO membrane | Removes dissolved salts and impurities |
| RO Pump | 12V DC High Pressure | Drives water through RO membrane |
| Pre-Filters | Sediment & Carbon | Protects RO membrane |
| Tubing & Connectors | Food-grade | Water flow connections |
| Overhead Tank | Raw water tank | Stores input water |
| Output Tank | Purified water tank | Stores filtered water |

---

## 🧠 Control & Processing

| Component | Model | Purpose |
|---------|------|---------|
| Microcontroller | Arduino Uno | Central control and processing unit |
| Relay Module | 1-channel 5V | Switches RO pump ON/OFF |
| EEPROM (Internal) | Arduino EEPROM | Stores TDS & threshold values |

---

## 📊 Sensors

| Sensor | Model | Function |
|------|-------|----------|
| TDS Sensor | Analog TDS Module | Measures water quality |
| Flow Sensor | YF-S201 / Equivalent | Measures water flow rate |
| Temperature & Humidity Sensor | DHT11 | Monitors environmental conditions |
| Float Sensor (Input Tank) | Mechanical | Detects water availability |
| Float Sensor (Output Tank) | Mechanical | Detects tank full/empty status |

---

## 🖥 User Interface & Alerts

| Component | Specification | Purpose |
|---------|---------------|---------|
| LCD Display | 16×2 I2C LCD | Displays system parameters |
| Joystick Module | 5-way + Button | Menu navigation & control |
| Buzzer | 5V Active Buzzer | TDS warning alert |

---

## 🔌 Miscellaneous Components

| Component | Purpose |
|---------|---------|
| Jumper Wires | Electrical connections |
| Breadboard / PCB | Circuit prototyping |
| Resistors & Diodes | Signal conditioning & protection |
| Enclosure | Protects electronics |

---

## 📌 Notes
- All components are **low-cost and easily available**
- System is designed for **rural & off-grid deployment**
- Components can be scaled based on power and water demand

---

**Project:** Solar Powered RO Water Purification System  
**Department:** ECE, CMR Institute of Technology  
**Academic Year:** 2025–26  
