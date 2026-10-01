# 04. Automotive Odometer Wear-Leveling & Anti-Tearing NVM Service

## 1. The EEPROM Endurance Challenge
Automotive instrument clusters must persist total distance traveled without data corruption over the vehicle's 10–15 year lifetime.
* Standard microcontroller internal EEPROM endurance: **100,000 write cycles** per byte.
* If odometer writes are committed to a single memory address every 100 meters:
  $$\text{Lifetime Distance} = 100,000 \times 100\text{ m} = 10,000\text{ km}$$
  The EEPROM would permanently fail after only **10,000 km of driving**!

---

## 2. The 16-Slot Wear-Leveling Solution
The `odometer_service` allocates a dedicated **16-slot circular ring buffer partition** in NVM (256 bytes total).

```
 ┌──────────┬──────────┬──────────┬──────────┬─────┬──────────┐
 │  Slot 0  │  Slot 1  │  Slot 2  │  Slot 3  │ ... │ Slot 15  │
 │ (16 B)   │ (16 B)   │ (16 B)   │ (16 B)   │     │ (16 B)   │
 └──────────┴──────────┴──────────┴──────────┴─────┴──────────┘
```

Each 16-byte slot contains:
```c
typedef struct {
    uint32_t total_odometer_meters; /**< Total distance in meters */
    uint32_t trip_distance_meters;  /**< Trip distance in meters */
    uint32_t sequence_id;           /**< Monotonically increasing write counter */
    uint16_t crc16;                 /**< CRC-16-CCITT of the above 12 bytes */
    uint16_t padding;               /**< 16-byte alignment */
} odo_slot_t;
```

### Endurance Calculation:
With 16-slot round-robin wear-leveling and a 100-meter commit threshold:
$$\text{Max Lifetime Distance} = 16 \times 100,000 \times 100\text{ m} = 160,000\text{ km}$$
For an electric scooter or light commercial vehicle driven 15,000 km/year, the EEPROM remains functional for **over 10.6 years of continuous operation**.

---

## 3. Power-Loss Anti-Tearing Recovery Algorithm

If the vehicle's 12V battery is disconnected or key-off occurs precisely in the middle of writing a 16-byte slot (tearing):

1. **Write Sequence:** Writes are executed atomically with CRC-16 calculated over the uncommitted data.
2. **Boot Recovery Sequence:**
   - On startup, the firmware scans all 16 slots.
   - For each slot, the 12-byte payload is read and its CRC-16-CCITT is computed.
   - Any slot with an invalid CRC (e.g. half-written bytes from sudden power cut) is immediately rejected.
   - The slot with the **highest valid `sequence_id`** is adopted as the active odometer state.
3. **Zero Data Loss:** The system seamlessly falls back to the previous valid slot recorded just 100 meters prior.
