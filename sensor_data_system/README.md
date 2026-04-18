# 🚀 FreeRTOS Sensor Data Pipeline with Fault Detection

## 📌 Overview

This project simulates a **real embedded system** (like a drone or satellite) using **FreeRTOS**.

It demonstrates how multiple tasks work together to:

* Read sensor data
* Process it
* Log it
* Detect faults
* React to faults in real-time

---

## 🧠 System Architecture

```
Sensor Task → Processing Task → Logger Task
                  ↓
           Fault Detection Task
                  ↓
        Notification → Sensor Task (control)
```

---

## ⚙️ Tasks Description

### 🔹 Sensor Task

* Generates dummy sensor data
* Sends data to processing task
* Adjusts speed based on system state

  * Normal → fast
  * Fault → slow

---

### 🔹 Processing Task

* Receives sensor data
* Processes the data (basic transformation)
* Sends data to:

  * Logger (all data)
  * Fault Detection (only faulty data)
* Notifies sensor task about system state

---

### 🔹 Logger Task

* Receives all processed data
* Prints/logs data continuously

---

### 🔹 Fault Detection Task

* Receives only faulty data
* Detects:

  * Overheat (> 80)
  * Sensor fault (< 20)
* Prints alert messages

---

## 🔄 System Behavior

### ✅ Normal Condition

* Sensor runs at normal speed
* Data flows continuously
* Logger prints all values

---

### ⚠️ Fault Condition

* Fault detected in processing stage
* Fault task prints alert
* Sensor task is notified
* Sensor slows down automatically

---

### 🔁 Recovery

* When data returns to normal
* Sensor resumes normal speed

---

## 🧩 FreeRTOS Concepts Used

* ✅ Tasks
* ✅ Queues (inter-task communication)
* ✅ Task Notifications (control signaling)
* ✅ Task Priorities (real-time behavior)

---

## 💡 Key Learnings

* Difference between **data flow (queues)** and **control flow (notifications)**
* Designing **event-driven systems**
* Implementing **real-time fault handling**
* Understanding **task scheduling and priorities**

---

## 🧪 Expected Output (Example)

```
Sensor data: 45
Processing data: 45
Logging: 45

Sensor data: 85
Processing data: 85
⚠️ Overheat detected!
Sensor slowing down...

Sensor data: 40
Sensor back to normal mode...
```

---

## ⚠️ Note

* This project currently uses **simulated sensor data**
* Hardware testing on ESP32 is planned

---

## 🚀 Future Improvements

* Add real sensor input (ESP32 + sensors)
* Add mutex for safe shared resource handling
* Improve data processing logic
* Optimize task communication

---

## 🎯 Conclusion

This project demonstrates a **complete RTOS-based system design**, combining:

* Data processing pipeline
* Fault detection
* Real-time system response


---
