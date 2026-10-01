# 01. System Architecture & 4-Layer HAL Design

## 1. Architectural Philosophy
The `automotive-tft-cluster` firmware is structured strictly in a **4-Layer Decoupled Architecture** conforming to MISRA-C:2012 guidelines and zero-heap static memory allocation.

```
 ┌─────────────────────────────────────────────────────────────────────────────────────────┐
 │                                   APPLICATION LAYER                                     │
 │  ┌─────────────────────────┐   ┌───────────────────────────┐   ┌─────────────────────┐  │
 │  │   cluster_ui_manager    │   │     can_telemetry_mgr     │   │   odometer_service  │  │
 │  │ (Screens, Themes, Gfx)  │   │  (DBC, Timeouts, Failsafe)│   │ (Wear-Level, CRC16) │  │
 │  └────────────┬────────────┘   └─────────────┬─────────────┘   └──────────┬──────────┘  │
 ├───────────────┼──────────────────────────────┼────────────────────────────┼─────────────┤
 │               ▼                              ▼                            ▼             │
 │                                   CORE GRAPHICS & MATH                                  │
 │  ┌───────────────────────────────────────────────────────────────────────────────────┐  │
 │  │ • gfx_engine.c: Integer Bresenham Lines, Thick Needles, Arc Bars, Fast Glyphs     │  │
 │  │ • gfx_trig_lut.c: Fixed-Point Q15 Sine/Cosine Table (Zero <math.h> dependency)    │  │
 │  │ • gfx_dirty_rect.c: Bounding Box Diff Calculator & Selective Eraser               │  │
 │  │ • font_embedded.c: 1-Bit Compressed Industrial Fonts in Flash Memory             │  │
 │  └───────────────────────────────────────────┬───────────────────────────────────────┘  │
 ├──────────────────────────────────────────────┼──────────────────────────────────────────┤
 │                                              ▼                                          │
 │                               HARDWARE ABSTRACTION LAYER (HAL)                          │
 │  ┌──────────────────┐   ┌───────────────────┐   ┌───────────────────┐   ┌────────────┐  │
 │  │    hal_display   │   │      hal_can      │   │      hal_nvm      │   │  hal_timer │  │
 │  └────────┬─────────┘   └─────────┬─────────┘   └─────────┬─────────┘   └─────┬──────┘  │
 ├───────────┼───────────────────────┼───────────────────────┼───────────────────┼─────────┤
 │           ▼                       ▼                       ▼                   ▼         │
 │  ┌───────────────────────────────────────────────────────────────────────────────────┐  │
 │  │                                   TARGET PORTS                                    │  │
 │  │  • port/pic18f/   : PIC18F46K22 Bare-Metal (XC8, Zero-Heap, Streaming SPI, EEPROM) │  │
 │  │  • port/stm32f4/  : STM32F4xx Reference Port (FreeRTOS Queues & SPI DMA Templates)│  │
 │  │  • port/host_sim/ : Native PC Virtual Simulator & 100% Automated Unity Test Suite  │  │
 │  └───────────────────────────────────────────────────────────────────────────────────┘  │
 └─────────────────────────────────────────────────────────────────────────────────────────┘
```

---

## 2. Multi-Rate Execution Schedule

To balance real-time bus responsiveness with smooth visual rendering, the application runs on a deterministic multi-rate cycle:

| Task / Subsystem | Execution Period | Frequency | Purpose |
| :--- | :---: | :---: | :--- |
| **CAN Ingestion & Watchdog** | **10 ms** | 100 Hz | Poll hardware CAN FIFO, decode J1939 frames, enforce 500 ms comms-loss timer. |
| **Odometer Distance Update** | **20 ms** | 50 Hz | Accumulate millimeter/meter distance delta from speed vector. |
| **Gauges Needle Render** | **33 ms** | 30 Hz | Partial dirty-rectangle refresh for Speedometer and Battery needles. |
| **Tell-Tale & Blinker Engine** | **50 ms** | 20 Hz | 1.5 Hz turn signal flash cadence, fault warning evaluation. |
| **Footer & NVM Commit** | **500 ms** | 2 Hz | Odometer NVM sync, Gear indicator update, ambient temp refresh. |

---

## 3. Zero-Heap Static Allocation
* **No `malloc()` or `free()`** anywhere in the runtime firmware.
* Total static RAM consumption across all services: **< 1.8 KB**, allowing effortless execution on an 8-bit PIC18F46K22 with 3.8 KB RAM.
