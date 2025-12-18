# TinyML-based Classification of Emergency Driving

This repository contains an end-to-end Tiny Machine Learning (TinyML) project developed as a semester project for the course **ETTML-01 Tiny Machine Learning** in the **Electronics Engineering** program at **Aarhus University, Campus Herning**.

The project demonstrates how a complete TinyML pipeline can be designed, implemented, and deployed on a resource-constrained microcontroller, without reliance on cloud-based solutions.

---

## Project Overview

The goal of the project is to classify driving behavior as either **normal driving** or **emergency driving (udrykningskørsel)** based solely on accelerometer data.

The system is built around a **Particle Photon 2** microcontroller connected to an **ADXL343 accelerometer** and performs:

- Real-time sensor data acquisition
- Feature extraction on-device
- Local machine learning inference
- Embedded decision logic and visualization

The final system runs continuously on the microcontroller and indicates detected emergency driving using a LED pattern resembling an ambulance emergency signal.

---

## System Architecture

**End-to-end TinyML pipeline:**

1. Data acquisition using a custom DAQ firmware on Photon 2
2. Data transmission via TCP to a laptop-based server
3. Offline preprocessing and feature extraction in Python
4. Training and evaluation of multiple ML models
5. Model selection based on accuracy and embedded constraints
6. Conversion of the trained model to C using `emlearn`
7. Deployment and real-world testing on the Photon 2

---

## Hardware

- **Microcontroller:** Particle Photon 2
- **Sensor:** ADXL343 3-axis accelerometer (I²C)
- **Sampling rate:** 500 Hz
- **Power:** USB power bank (during mobile testing)
- **Output:** 3 blue LEDs indicating emergency driving

---

## Machine Learning

### Models Evaluated
- Random Forest
- Extra Trees
- Tiny CNN
- Tiny CNN v2
- Tiny Depthwise CNN (best offline performance)

Due to embedded constraints and implementation feasibility, the final deployed model is:

- **Random Forest with top 10 features**
- Accuracy ≈ **86%** (offline evaluation)

### Feature Engineering
- Time-domain features
- Frequency-domain features
- FFT-based Welch features (offline)
- Goertzel-based frequency features (embedded-friendly)

Feature importance was used extensively to reduce computational complexity while preserving model performance.

---

## Embedded Implementation

- Sliding windows: 2 seconds
- Window overlap: 50%
- Debounce logic: 3 consecutive identical predictions required
- Fully local inference (no cloud connectivity)

The model was validated both in Python and in C to ensure identical predictions before deployment.

---

## Results

The system was tested in a real ambulance during both normal driving and emergency driving.

- Stable and reliable operation on the microcontroller
- Correct detection of emergency driving in most scenarios
- Increased false positives during high-speed normal driving (motorway), highlighting dataset bias and generalization challenges

A short video demonstrating the system in operation can be found here:  
▶️ https://www.youtube.com/shorts/jz6B_zdqWPw

---

## Repository Structure

```text
.
├── data/               # Raw and processed accelerometer data (CSV)
├── python/             # Preprocessing, feature extraction and training scripts
├── models/             # Trained models and exported emlearn headers
├── firmware/           # Photon 2 firmware (DAQ + final TinyML system)
├── hardware/           # Wiring diagrams and hardware documentation (PDF)
├── figures/            # Plots and figures used in the report
└── README.md
