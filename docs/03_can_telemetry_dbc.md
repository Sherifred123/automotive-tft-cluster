# 03. Automotive CAN Telemetry & J1939 Frame Specifications

## 1. CAN Bus Physical & Data Link Layer
* **Standard:** ISO 11898-1 / ISO 11898-2 (High-Speed CAN).
* **Baud Rate:** 500 kbps (Automotive standard).
* **Sample Point:** 80% nominal bit time.
* **Identifier Format:** 29-bit Extended Identifier (SAE J1939).

---

## 2. Parameter Group Number (PGN) Mapping

### PGN 0x18FEE600: Electronic Brake & Speed Telemetry
* **Transmission Rate:** 10 ms (100 Hz)
* **Payload Structure (8 Bytes):**

| Byte | Bit Range | Signal Name | Data Type | Resolution | Range |
| :---: | :---: | :--- | :---: | :---: | :---: |
| 0–1 | 0..15 | `VehicleSpeed` | `uint16_t` | 0.1 km/h per bit | 0.0 to 250.0 km/h |
| 2 | 0 | `BrakeSwitchActive` | `bool` | 1 = Active, 0 = Off | 0 or 1 |
| 3–7 | — | Reserved | — | — | 0xFF |

---

### PGN 0x0CF00400: Electronic Motor Controller (EEC1)
* **Transmission Rate:** 20 ms (50 Hz)
* **Payload Structure (8 Bytes):**

| Byte | Bit Range | Signal Name | Data Type | Resolution | Range |
| :---: | :---: | :--- | :---: | :---: | :---: |
| 0–2 | — | Engine Torque | — | — | — |
| 3–4 | 24..39 | `TractionMotorRPM` | `uint16_t` | 1.0 RPM per bit | 0 to 12,000 RPM |
| 5–7 | — | Reserved | — | — | 0xFF |

---

### PGN 0x18FEE500: High Voltage Battery Management (BMS)
* **Transmission Rate:** 50 ms (20 Hz)
* **Payload Structure (8 Bytes):**

| Byte | Bit Range | Signal Name | Data Type | Resolution | Range |
| :---: | :---: | :--- | :---: | :---: | :---: |
| 0 | 0..7 | `BatteryStateOfCharge` | `uint8_t` | 1% per bit | 0 to 100% |
| 1–2 | 8..23 | `BatteryPackVoltage` | `uint16_t` | 0.1 V per bit | 0.0 to 100.0 V |
| 3 | 24..31 | `BatteryTemperature` | `int8_t` | 1 °C per bit | -40 to +85 °C |
| 4–7 | — | Reserved | — | — | 0xFF |

---

### PGN 0x18FECA00: Vehicle Body & Tell-Tale Status
* **Transmission Rate:** 50 ms (20 Hz)
* **Payload Structure (8 Bytes):**

| Byte | Bit Range | Signal Name | Description |
| :---: | :---: | :--- | :--- |
| 0 | Bit 0 | `TurnLeftActive` | 1 = Turn Left Blinker Active |
| 0 | Bit 1 | `TurnRightActive` | 1 = Turn Right Blinker Active |
| 0 | Bit 2 | `HighBeamActive` | 1 = High Beam Headlight ON |
| 0 | Bit 3 | `HazardActive` | 1 = Hazard 4-way Flashers ON |
| 0 | Bit 4 | `LowBatteryWarning` | 1 = Critical Low Battery Alert |
| 0 | Bit 5 | `MotorOverTemp` | 1 = Inverter / Motor Thermal Alert |

---

## 3. Communication Loss Watchdog & Failsafe
* **Watchdog Interval:** 500 ms.
* **Failsafe Behavior:** If no valid frames arrive within 500 ms:
  1. `is_comm_active` is asserted `false`.
  2. `[CAN FAULT]` yellow tell-tale starts flashing at 2.0 Hz.
  3. Speed and Motor RPM needles gracefully decay to zero using an exponential smoothing filter (decay factor $\alpha = 0.90$) to avoid sudden jarring pointer drops.
