# 02. Graphics Engine Design: Q15 Integer Math & Dirty Rectangles

## 1. Why `<math.h>` is Prohibited in Resource-Constrained Firmware
In an 8-bit MCU (PIC18) or cost-sensitive 32-bit MCU without a Hardware Floating Point Unit (FPU), calling `sin()`, `cos()`, or `double` math introduces:
1. **Flash Bloat:** Software floating-point emulation libraries consume 6 to 10 KB of Flash ROM.
2. **Execution Latency:** Floating-point division and trigonometry take thousands of instruction cycles.
3. **Non-Deterministic Jitter:** Interrupt latency is impacted during multi-cycle software float routines.

---

## 2. Fixed-Point Q15 Trigonometry Engine
The `gfx_trig_lut` module implements a **91-entry precomputed sine lookup table** representing the first quadrant ($0^\circ$ to $90^\circ$):

$$\text{LUT}[i] = \text{round}(32767 \times \sin(i^\circ))$$

By leveraging 4-quadrant symmetry:
* **Quadrant I ($0^\circ - 90^\circ$):** $\sin(\theta) = \text{LUT}[\theta]$
* **Quadrant II ($91^\circ - 180^\circ$):** $\sin(\theta) = \text{LUT}[180 - \theta]$
* **Quadrant III ($181^\circ - 270^\circ$):** $\sin(\theta) = -\text{LUT}[\theta - 180]$
* **Quadrant IV ($271^\circ - 359^\circ$):** $\sin(\theta) = -\text{LUT}[360 - \theta]$
* **Cosine:** $\cos(\theta) = \sin(\theta + 90^\circ)$

### Vector Endpoint Calculation:
```c
int32_t prod_x = cos_val * (int32_t)radius;
int32_t prod_y = sin_val * (int32_t)radius;

/* Symmetric integer rounding */
int32_t dx = (prod_x >= 0) ? ((prod_x + 16384) >> 15) : -((-prod_x + 16384) >> 15);
int32_t dy = (prod_y >= 0) ? ((prod_y + 16384) >> 15) : -((-prod_y + 16384) >> 15);

p_out->x = (int16_t)(xc + dx);
p_out->y = (int16_t)(yc + dy);
```
**Total Flash Consumption:** Only **182 bytes**. Execution time: **< 20 CPU cycles**.

---

## 3. Dirty-Rectangle Delta Rendering

```
   Previous Frame (Angle θ1)                 Next Frame (Angle θ2)
   ┌───────────────────────┐                 ┌───────────────────────┐
   │                       │                 │                       │
   │       \ Needle Old    │  ============>  │       | Needle New    │
   │        \              │  1. Erase Old   │       |               │
   │         O Hub         │  2. Draw New    │       O Hub           │
   │                       │                 │                       │
   └───────────────────────┘                 └───────────────────────┘
   [ Dirty Bounding Box 1 ]                  [ Dirty Bounding Box 2 ]
```

1. Instead of clearing and redrawing the whole $320 \times 240$ frame ($153.6\text{ KB}$), the engine tracks the previous needle angle $\theta_{old}$.
2. When the needle moves, the old needle vector is erased by redrawing it in the background color.
3. The new needle vector is rasterized in `COLOR_AUTO_NEEDLE` over hardware SPI.
4. Total pixels pushed per frame update: **< 250 pixels** instead of $76,800$ pixels (a **99.6% reduction in SPI bus traffic**).
