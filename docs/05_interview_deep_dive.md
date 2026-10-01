# 05. Senior Embedded Interview Deep Dive (Pricol & Automotive Tier-1 Focus)

This document provides senior-level architectural talking points and interview answers tailored for **Automotive Instrument Cluster & Telemetry roles (Pricol, Bosch, Continental, Visteon, TVS)**.

---

### Q1: "How do you render an analog needle on a display without causing screen tearing or flickering?"
> **Senior Answer:**  
> *"Flicker occurs when an entire screen is cleared to black and redrawn, creating a visual blank interval. Screen tearing occurs when the display controller's scanline overtakes the SPI framebuffer transfer.  
> In my cluster architecture, I implemented **dirty-rectangle delta updates**. The engine retains the previous needle angle $\theta_{old}$. When $\theta_{new}$ is calculated, the previous needle vector is erased by drawing it in the background theme color directly over SPI into the display controller's internal GRAM, followed immediately by drawing the new needle in red. Because only ~200 pixels are modified instead of 76,800 pixels, the refresh completes in under 1 ms—completely invisible to the human eye with zero tearing and zero flicker."*

---

### Q2: "How can you drive a 320x240 color TFT display on a PIC18F46K22 with only 3.8 KB of RAM?"
> **Senior Answer:**  
> *"A full 320x240 RGB565 framebuffer requires $153.6\text{ KB}$ of RAM, which is 40 times larger than the PIC18's total memory.  
> Instead of keeping a full framebuffer in MCU RAM, I treat the display controller's (ST7789) on-chip GRAM as the primary video memory. My graphics engine is 100% streaming:
> 1. To draw text, 1-bit glyphs are read from Program Flash (`const __far`) and streamed pixel-by-pixel.
> 2. To draw needles and arcs, integer Bresenham algorithms calculate pixel coordinates on the fly and stream directly via 16 MHz hardware SPI.
> 3. Total RAM consumed by the entire graphics engine is **less than 300 bytes**, leaving the rest of the 3.8 KB RAM for CAN buffers and system state machines."*

---

### Q3: "Why did you implement a custom Q15 trigonometry LUT instead of `<math.h>`?"
> **Senior Answer:**  
> *"Standard C `<math.h>` functions (`sin()`, `cos()`) use double-precision floating point. On an 8-bit MCU or Cortex-M4 without FPU, software float emulation bloats Flash by 8 KB and takes over 1,000 instruction cycles per calculation.  
> I implemented a 91-entry Q15 sine table in Flash consuming only 182 bytes. By using 4-quadrant symmetry and fixed-point bit shifts `(radius * cos) >> 15` with symmetric integer rounding, coordinate calculation executes in **under 20 cycles with < 0.1% angular error**."*

---

### Q4: "How do you protect Odometer distance in EEPROM against sudden power loss and endurance wear?"
> **Senior Answer:**  
> *"If you write to the same EEPROM address every 100 meters, you exhaust the 100,000-write limit within 10,000 km.  
> I implemented a **16-slot round-robin wear-leveling ring buffer** in NVM, which increases endurance to **160,000 km (>10 years of vehicle life)**.  
> Each 16-byte slot contains a monotonically increasing `sequence_id` and a CRC-16-CCITT checksum. If power is lost mid-write, the corrupted slot fails CRC check on the next boot, and the firmware automatically recovers the preceding valid slot."*

---

### Q5: "What happens if the CAN bus cable is disconnected while driving at 80 km/h?"
> **Senior Answer:**  
> *"I implemented a **500 ms loss-of-communication watchdog timer** in `can_telemetry`. If no valid speed or battery PGNs arrive within 500 ms:
> 1. The `[CAN FAULT]` yellow tell-tale flashes at 2.0 Hz.
> 2. The Speedometer and RPM needles do not abruptly drop to zero (which looks broken to the driver); instead, an exponential smoothing filter ($\alpha = 0.90$) gently glides the needles down to zero over a 1-second decay period."*

---

### Q6: "How does your codebase support both PIC18 Bare-Metal and STM32 FreeRTOS?"
> **Senior Answer:**  
> *"Through a strict 4-layer Hardware Abstraction Layer (`hal_display`, `hal_can`, `hal_nvm`, `hal_timer`).  
> The core graphics engine, CAN message parser, and Odometer service contain zero hardware-specific registers.  
> On PIC18, `port/pic18f/` implements register-level SPI (`SSP1BUF`) and internal EEPROM.  
> On STM32, `port/stm32f4/` uses FreeRTOS queues, mutexes, and non-blocking SPI + DMA transfers.  
> On PC, `port/host_sim/` allows running automated unit tests and interactive terminal simulations without hardware."*
