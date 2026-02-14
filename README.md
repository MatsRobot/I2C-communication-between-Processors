# 📂 I2C-Bridge: Multi-Processor Data Exchange

A comprehensive implementation of I2C communication between a Raspberry Pi Pico (Master) and an Arduino (Slave), including multi-bus OLED integration and data handling.

<p align="center">
  <img width="400" src="https://github.com/user-attachments/assets/1b0e9583-226e-419a-a9e9-cb47b47ef424" alt="I2C Hardware Setup" />
</p>

---

## 🚀 The Backstory

The I2C protocol is fundamentally designed for a Master-Slave relationship. While the protocol theoretically supports "Multi-Master" configurations, in practice, this leads to significant clock synchronization issues on the **SCL** line and software-level "arbitration" headaches. 

My objective was to create a reliable system where a Raspberry Pi Pico could act as a central hub (Master) gathering data from various sensors managed by peripheral microcontrollers (Slaves). The primary challenge was the Pico’s MicroPython environment, which currently lacks native, easy-to-use libraries for setting a Slave I2C address, making the Pico-to-Pico slave configuration difficult.

## ✨ Key Features

* **Cross-Platform Communication:** Successfully bridges MicroPython (Pico) and C++ (Arduino) via I2C.
* **Master-Controlled Bus:** Centralized clock management via the Pico to prevent data collisions.
* **Dual-Bus OLED Setup:** Utilizes both I2C0 and I2C1 on the Pico to isolate display traffic from sensor traffic.
* **Sensor Integration:** Features an Arduino-side **MaxSonar** ultrasound transducer integration, passing distance data to the Pico.
* **Interrupt Safety:** Specific code modifications to prevent the Arduino from conflicting with the I2C bus while handling local tasks.

## 🛠️ Hardware Stack

* **Master:** Raspberry Pi Pico (RP2040).
* **Slave:** Arduino (Uno/Nano/Atmega328p).
* **Displays:** 0.91" or 0.96" OLED displays (SSD1306).
* **Sensor:** MaxSonar Ultrasound Transducer.
* **Level Shifters:** (Optional, but recommended) for 3.3V (Pico) to 5V (Arduino) logic translation.



## 📐 How It Works

### The Addressing Logic
In this architecture, the **Pico (Master)** initiates all conversations.
* **Arduino Slave:** Assigned address `10` (`0x0A`). It listens for requests and sends sensor data back in a readable text format.
* **OLED Master-Bus:** Located at address `60` (`0x3C`). 

### Hardware Configuration
The Pico uses a dual-bus approach to manage data flow:
1. **I2C0 (Pins GP16/GP17):** Dedicated to the "External Bus." This connects to the Arduino Slave and the primary OLED.
2. **I2C1:** Dedicated to a secondary OLED for local status monitoring.

### Conflict Resolution
A critical discovery during development was that the Arduino could not safely communicate with the OLED on the same databus while configured as a slave. Attempting to initialize the display on the Arduino side caused interrupt conflicts. The solution was to delegate all display tasks on the shared bus exclusively to the Pico.



## 📊 Data Flow

1.  **Arduino** reads the distance from the **MaxSonar** sensor.
2.  **Pico (Master)** sends a request to Address `0x0A`.
3.  **Arduino** triggers an `onRequest` interrupt and sends the distance string.
4.  **Pico** receives the string, parses it, and updates the **OLED** displays on both I2C0 and I2C1.

<p align="center">
  <img width="200" src="https://github.com/user-attachments/assets/e6ec01bc-53f9-495d-a87c-e3d81cdc774c" alt="Data Display 1" />
  <img width="200" src="https://github.com/user-attachments/assets/2cf411a6-a8f2-44cc-9e6d-16a48410232f" alt="Data Display 2" />
</p>

---

## 📜 Documentation

Detailed code for both the MicroPython Master and the Arduino C++ Slave can be found in the repository. Please ensure common ground is established between the Pico and Arduino to prevent data corruption.

---
