# LoRa 2.4GHz Performance Study

This repository contains source code and measurement data used in my **bachelor's thesis**, focused on evaluating the **performance of LoRa communication in the 2.4GHz ISM band** under various transmission conditions.

---

## Overview

This repository provides:

- Source code for automated LoRa communication measurements using **Raspberry Pi 4** and the [**SK-iM282A**](https://wireless-solutions.de/products/lora-sk-im282a-starter-kit.html) development kit.
- Raw and cleaned datasets. Edited versions exclude periods when one of the modules crashed due to software bugs.
- Instructions for required libraries and software to run each part of the system.
- Documentation of modifications made to the C++ library provided by **IMST GmbH**.

---

## Measurement Client (`/measurement_client/`)

This folder contains a **C++ program** based on a modified version of the manufacturer’s communication library. It runs on a PC or Raspberry Pi (tested on Pi 4) and:

- Initializes a **Radio Link Test** function.
- Continuously monitors communication output.
- Logs signal data to a `.csv` file.

### Requirements

#### Software
- Linux (Windows is **not supported**)
- [`Make`](https://www.gnu.org/software/make/)
- [ARM GCC Toolchain](https://developer.arm.com/downloads/-/arm-gnu-toolchain-downloads) (`arm-none-linux-gnueabihf-g++`) for cross-compiling to Raspberry Pi

#### Hardware
- Raspberry Pi 4
- SK-iM282A development kit (LoRa 2.4GHz)

### Library Modifications

- **Qt dependencies removed** and replaced with standard C++ equivalents.
- Functions including QT references that were not needed were commented out 
- **Radio Link Test functionality** added using WiMOD LR Base HCI documentation (PDF provided by the manufacturer).

---

## Measured Data (`/measured_data.zip`)

The folder is in a zip format to avoid completely filling up Github Large File Storage.

This folder includes:

- Subfolders with **CSV files** containing raw or edited measurement logs.
- Filenames include:
  - Measurement location
  - Start timestamp
  - Optional `edited` suffix (if corrupted periods were removed)
- Each CSV file:
  - Has a **header** row
  - Includes signal configuration metadata as a comment (lines starting with `#`)

---

## Python Analysis (`/python_visualisation/`)

Python scripts for **visualizing and analyzing** the measured data.

### Features

The main script, `analyze.py`, can generate:

- Boxplots per day and per hour (aggregated across days)
- Time series plots (4-day window or full day for shorter runs) with a 15-minute running average
- Histograms of selected signal metrics
- Packet loss trends over time (uplink/downlink separately)
- Daily packet loss summaries (uplink/downlink)

Selected functions can be enabled and configured in the ```plot_functions``` list variable in the end of the file. 

### Setup

To install the required Python libraries, run:

```bash
cd python_visualisation
pip install -r requirements.txt
```

### Usage
Run the analysis script with:

```bash
python analyze.py <input CSV file> [day offset for time plot, default = 0]
```

### Output
All generated plots will be saved into a folder:

```bash
python_visualisation/figures/<input_filename>/
```

The script also computes the Packet Error Rate (PER) for uplink and downlink based on equations provided by the manufacturer in the WiMOD LR Base HCI documentation and stores it in the same location as graphical outputs in ```stats.txt``` file.

## Repository structure
```
LoRa24GHz-Performance/
├── measurement_client/         # C++ program and modified library
│   ├── WiMODLR/
│   ├── main.cpp
│   ├── WiMOD_LR_Base_HCI_Spec_V1_10-1.pdf
│   └── Makefile
├── measured_data.zip              # CSV files with measured data
│   ├── indoor/
│   ├── outdoor/
│   │   └── FLRC/
│   └── dynamic/
├── Python_visualisation/       # Python analysis and plotting tools
│   ├── analyse.py
│   ├── requirements.txt
│   └── figures/
└── README.md                   # This file
```
