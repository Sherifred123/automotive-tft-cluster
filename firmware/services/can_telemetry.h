/**
 * @file can_telemetry.h
 * @brief Automotive CAN 2.0B / J1939 Telemetry Ingestion and Watchdog Service.
 * 
 * Decodes standard automotive powertrain and battery telemetry PGNs:
 *   - PGN 0x18FEE600: Vehicle Speed & Brake Status
 *   - PGN 0x0CF00400: Engine / Traction Motor RPM & Torque
 *   - PGN 0x18FEE500: Battery Pack State of Charge (SOC %) & Voltage
 *   - PGN 0x18FECA00: Vehicle Status, Tell-Tale Indicators, Diagnostics
 * 
 * Features a 500 ms loss-of-communication watchdog with smooth failsafe needle decay.
 * 
 * @author S. Sherifred Singh (Senior Embedded Firmware Engineer)
 * @standard MISRA-C:2012 compliant, ANSI C99
 */

#ifndef CAN_TELEMETRY_H
#define CAN_TELEMETRY_H

#include <stdint.h>
#include <stdbool.h>
#include "../hal/hal_can.h"

#ifdef __cplusplus
extern "C" {
#endif

/* Standard J1939 PGNs (Parameter Group Numbers) */
#define CAN_PGN_SPEED_BRAKE   (0x18FEE600U)
#define CAN_PGN_MOTOR_SPEED   (0x0CF00400U)
#define CAN_PGN_BATTERY_SOC   (0x18FEE500U)
#define CAN_PGN_TELLTALES     (0x18FECA00U)

/* CAN Communication Loss Watchdog Timeout (Milliseconds) */
#define CAN_COMM_TIMEOUT_MS   (500U)

/* Decoded Live Vehicle Telemetry Structure */
typedef struct {
    uint16_t speed_kmh_x10;    /**< Vehicle speed in 0.1 km/h (e.g. 725 = 72.5 km/h) */
    uint16_t motor_rpm;        /**< Motor / Engine RPM (0 to 12000 RPM) */
    uint8_t  battery_soc_pct;  /**< Battery State of Charge (0 to 100%) */
    uint16_t battery_voltage_x10; /**< Battery pack voltage in 0.1 V (e.g. 524 = 52.4 V) */
    int8_t   ambient_temp_c;   /**< Outside ambient temperature in deg C */
    
    /* Indicator & Warning Flags */
    bool flag_turn_left;
    bool flag_turn_right;
    bool flag_high_beam;
    bool flag_hazard;
    bool flag_battery_low;
    bool flag_motor_overtemp;
    bool flag_brake_active;
    
    /* Communication Quality */
    bool is_comm_active;       /**< True if valid CAN frames received within 500ms */
    uint32_t total_frames_rx;  /**< Diagnostic counter of total valid frames */
    uint32_t last_rx_timestamp_ms; /**< Timestamp of last valid frame */
} vehicle_telemetry_t;

/**
 * @brief Initialize CAN Telemetry service and acceptance filter masks.
 */
void can_telemetry_init(void);

/**
 * @brief Ingest a single raw CAN frame from the Rx buffer and update telemetry data.
 * @param p_frame Pointer to received CAN frame.
 * @return true if frame was recognized and decoded, false if ignored.
 */
bool can_telemetry_process_frame(const can_frame_t *p_frame);

/**
 * @brief Periodic telemetry update loop (run at 100 Hz / 10 ms).
 * Handles hardware FIFO polling, timeout monitoring, and failsafe decays.
 * @param current_time_ms Current monotonic system tick in milliseconds.
 */
void can_telemetry_poll(uint32_t current_time_ms);

/**
 * @brief Get read-only copy of the active decoded telemetry.
 * @param p_out Destination pointer to copy telemetry into.
 */
void can_telemetry_get_data(vehicle_telemetry_t *p_out);

/**
 * @brief Manually inject a telemetry frame (used by tests & PC simulator).
 */
void can_telemetry_inject_speed(uint16_t speed_kmh_x10, uint16_t rpm);
void can_telemetry_inject_battery(uint8_t soc_pct, uint16_t voltage_x10);
void can_telemetry_inject_telltales(bool left, bool right, bool high_beam, bool batt_low, bool fault);

#ifdef __cplusplus
}
#endif

#endif /* CAN_TELEMETRY_H */
