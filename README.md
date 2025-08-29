# Remote Access Terminal – Embedded Face Recognition System

[![Language: C](https://img.shields.io/badge/language-C-blue.svg)]()
[![Language: Python](https://img.shields.io/badge/language-Python-yellow.svg)]()
[![Board: KL25Z](https://img.shields.io/badge/board-NXP%20KL25Z-green.svg)]()

---

## 🔹 Overview
**Remote Access Terminal** is a hybrid **embedded + computer vision** project.  
It integrates a **Python-based face recognition system** with **embedded C firmware** running on the **NXP KL25Z microcontroller**.  

- **Face recognition** authenticates users via webcam.  
- **UART communication** securely transfers events to the KL25Z.  
- **KL25Z firmware** manages sensors, LCD output, and IR remote control.  
- **SQLite3 database** stores access logs for auditing.  

This makes it a **smart access control system** blending biometrics with embedded hardware.

---

## 🔹 Features

### 🖥️ Python (PC Side)
- Real-time face recognition using **OpenCV** + `face_recognition` library.
- Sends authentication timestamps to KL25Z over **UART**.
- Logs all recognized users into **SQLite database**.
- Utility scripts for viewing/deleting logs.

### ⚙️ Embedded KL25Z (C Firmware)
- **LCD (I²C)** → displays system info and authentication status.
- **IR Remote** → provides user input capability.
- **DHT11 Sensor** → temperature and humidity readings (`temp_mois.c/h`).
- **Structured codebase**: clean separation of modules (`LCD.c`, `IRremote.c`, etc.).

### 📊 Database Utilities
- `sqlite_items.py` → view all entries in the access log.  
- `sqlite_delete.py` → clear the log table.  

---

## 🔹 System Architecture

```markdown

flowchart LR
    Webcam -->|Face Recognition| Python
    Python -->|UART: timestamp or code| KL25Z
    KL25Z -->|LCD Output| LCD
    KL25Z -->|Sensor Data| DHT11
    KL25Z -->|IR Remote Input| IR_Remote
    Python -->|Log entries| SQLite
```

---

## 🔹 Project Structure

```
Remote-Access-Terminal/
│── embedded/        # KL25Z firmware
│   ├── main.c
│   ├── MainFile.c
│   ├── MainFile.h
│   ├── LCD.c
│   ├── LCD.h
│   ├── IRremote.c
│   ├── IRremote.h
│   ├── temp_mois.c
│   ├── temp_mois.h
│
│── python/          # Face recognition + database scripts
│   ├── face_recognition_212Project.py
│   ├── sqlite_items.py
│   ├── sqlite_delete.py
│   ├── access_logs.db   (ignored in .gitignore if not needed)
│
│── README.md
│── .gitignore
```

---

## 🔹 How It Works

1. **Face Recognition (Python)**  
   - Loads known faces (e.g., `me.jpg`, `biden.jpg`).  
   - Matches webcam input against stored encodings.  
   - On recognition → sends timestamp string to KL25Z.  
   - On unknown → sends `"0"` code.  
   - Logs all recognized users to SQLite.  

2. **KL25Z Firmware (Embedded C)**  
   - Listens on UART.  
   - Displays authentication status on **LCD**.  
   - Can interact with user via **IR remote**.  
   - Continuously reads **DHT11** sensor values.  

3. **Database Management**  
   - View log entries with `sqlite_items.py`.  
   - Delete logs with `sqlite_delete.py`.  

---

## 🔹 Requirements

### 🔧 Hardware
- **Board:** NXP KL25Z (ARM Cortex-M0+)
- **Peripherals:**
  - I²C LCD (20x4 recommended)
  - IR Remote + Receiver
  - DHT11 sensor

### 💻 Software
- **Embedded Side:**  
  - Keil uVision / MCUXpresso / or GCC ARM Toolchain  
- **Python Side:**  
  - Python 3.8+  
  - Install dependencies:  
    ```bash
    pip install face_recognition opencv-python numpy pyserial
    ```

---

## 🔹 Usage

### 1. Embedded Side
1. Flash `embedded/` firmware to KL25Z.  
2. Connect LCD, IR receiver, and DHT11.  

### 2. Python Side
1. Place known face images (`me.jpg`, `biden.jpg`, etc.) in the working directory.  
2. Run the main script:  
   ```bash
   python face_recognition_212Project.py
   ```
3. Press **Q** to exit the program.  

### 3. Logs
- View logs:  
  ```bash
  python sqlite_items.py
  ```  
- Delete logs:  
  ```bash
  python sqlite_delete.py
  ```  

---

## 🔹 Demo Flow
1. User appears in front of webcam.  
2. Python recognizes user and logs event.  
3. Timestamp sent to KL25Z.  
4. KL25Z displays status on LCD.  
5. IR remote + sensors provide extended interaction.  

---

## 🔹 Future Improvements
- Add RTC (DS1302) and EEPROM for offline timestamp storage.  
- Extend database with web dashboard for monitoring.  
- Support wireless communication (e.g., Wi-Fi or MQTT).  

---

## 🔹 Author
Developed by **Ulaş Sakın**  
2025 – Embedded Systems + Computer Vision Project
