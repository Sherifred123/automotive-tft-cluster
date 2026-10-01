# 05. System Design Rationale & Architectural Trade-offs

This document details the core engineering trade-offs, performance benchmarks, and architectural design rationale for the Automotive Instrument Cluster & Telemetry firmware.

---

### 1. Zero-Flicker Needle Rendering: Dirty-Rectangle Delta Updates (vs Full Framebuffer)
* **Design Challenge:** How to render an analog gauge needle at 30+ FPS without screen tearing or frame stuttering on memory-constrained hardware.
* **Architectural Trade-Off:**
  * *Traditional Double-Buffering:* Requires 153.6 KB RAM for a single 320x240 RGB565 buffer (307.2 KB for double-buffering), exceeding MCU internal SRAM on cost-sensitive automotive microcontrollers.
  * *Dirty-Rectangle Streaming:* The graphics engine retains the previous needle angle $\theta_{old}$. When $\theta_{new}$ is calculated, the previous needle vector is erased by drawing it in the background theme color directly over SPI into the display controller's internal GRAM, followed immediately by rasterizing the new needle in accent color.
* **Benchmark:** Modifying ~200 pixels instead of 76,800 pixels per frame reduces SPI bus traffic by >97% and completes in under 1 ms—completely invisible to the human eye with zero tearing and zero flicker.

---

### 2. Direct GRAM Streaming on Ultra-Constrained RAM Hardware
* **Design Challenge:** Driving a 320x240 color TFT panel on resource-constrained microcontrollers (e.g., PIC18F46K22 with only 3.8 KB of RAM).
* **Architectural Solution:**
  * The display controller's (ST7789) on-chip GRAM is treated as the primary video memory.
  * The graphics pipeline is 100% streaming:
    1. **Text Glyphs:** 1-bit compressed glyphs are read from Program Flash (`const`) and streamed pixel-by-pixel.
    2. **Needles & Arcs:** Integer Bresenham algorithms calculate pixel coordinates on the fly and stream directly via 16 MHz hardware SPI.
  * **Result:** The entire graphics subsystem operates with **less than 300 bytes of static RAM**, preserving SRAM for CAN buffers and system state machines.

---

### 3. Deterministic Trigonometry: Fixed-Point Q15 LUT (vs `<math.h>` Floating-Point)
* **Design Challenge:** Coordinate transformations for needle pointers require continuous sine and cosine evaluations. Standard C `<math.h>` floating-point functions (`sin()`, `cos()`) introduce non-deterministic execution times.
* **Architectural Solution:**
  * Software float emulation on 8-bit MCUs or Cortex-M0/M3 cores bloats Flash by 8 KB and requires over 1,000 instruction cycles per calculation.
  * A 91-entry Q15 sine lookup table in Flash consumes only 182 bytes.
  * By leveraging 4-quadrant symmetry and fixed-point bit shifts `(radius * cos_val) >> 15` with integer rounding, coordinate calculations execute in **< 20 CPU cycles with < 0.1% angular error**.

---

### 4. Odometer NVM Integrity: 16-Slot Round-Robin Wear-Leveling
* **Design Challenge:** Preserving vehicle mileage in non-volatile memory across unexpected power loss (sudden battery disconnection) without exceeding EEPROM write endurance.
* **Architectural Solution:**
  * Standard EEPROM cells support ~100,000 write cycles. Writing to a single address every 100 meters exhausts the endurance limit in just 10,000 km.
  * A **16-slot round-robin wear-leveling ring buffer** extends endurance to **160,000 km (>10 years of vehicle lifecycle)**.
  * Each 16-byte record stores a monotonic sequence counter, trip/total distance, and a CRC-16-CCITT checksum.
  * **Torn-Write Protection:** If power cuts mid-write, the incomplete slot fails CRC verification on the next power cycle, and the recovery algorithm automatically rolls back to the preceding valid slot.

---

### 5. CAN Communication Watchdog & Graceful Needle Dynamics
* **Design Challenge:** Safe behavior when the vehicle telemetry bus disconnects or encounters bus-off while driving at highway speeds.
* **Architectural Solution:**
  * A **500 ms loss-of-communication watchdog timer** monitors incoming J1939 telemetry (`PGN 0x18FEE600`, `0x0CF00400`).
  * If no valid message arrives within 500 ms:
    1. The `[CAN FAULT]` tell-tale activates at a 2.0 Hz warning cadence.
    2. Needles do not snap to zero abruptly (which distracts the driver); instead, an exponential decay filter ($\alpha = 0.90$) glides the needle down to zero over a smooth 1-second interval.

---

### 6. Layered Hardware Abstraction Layer (HAL) & Portable Architecture
* **Design Challenge:** Creating a single core codebase capable of running on bare-metal 8-bit MCUs, 32-bit ARM Cortex-M4 platforms with RTOS, and Host PC simulation.
* **Architectural Solution:**
  * Strict separation between Application, Services, Core Graphics, and HAL (`hal_display`, `hal_can`, `hal_nvm`, `hal_timer`).
  * **Host PC Port (`port/host_sim`):** Enables 100% automated Unity test harness execution in CI pipelines.
  * **PIC18 Port (`port/pic18f`):** Direct hardware register access for resource-constrained bare-metal targets.
  * **STM32 Port (`port/stm32f4`):** Leverages FreeRTOS queues and non-blocking SPI DMA streaming.
