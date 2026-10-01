/**
 * @file can_telemetry.c
 * @brief Implementation of Automotive CAN 2.0B / J1939 Telemetry Service.
 * 
 * @author S. Sherifred Singh (Senior Embedded Firmware Engineer)
 * @standard MISRA-C:2012 compliant, ANSI C99
 */

#include "can_telemetry.h"
#include <string.h>

static vehicle_telemetry_t s_telemetry;

void can_telemetry_init(void)
{
    (void)memset(&s_telemetry, 0, sizeof(s_telemetry));
    s_telemetry.battery_soc_pct = 100;
    s_telemetry.battery_voltage_x10 = 520; /* 52.0 V default */
    s_telemetry.ambient_temp_c = 28;       /* 28 C default */
    s_telemetry.is_comm_active = false;
    
    (void)hal_can_init(CAN_BAUD_500K);
}

bool can_telemetry_process_frame(const can_frame_t *p_frame)
{
    if (p_frame == (void *)0) {
        return false;
    }

    bool decoded = false;
    uint32_t id = p_frame->id;

    /* Match J1939 PGNs (Masking priority and source address: 0x00FFFF00) */
    uint32_t pgn_match = id & 0x00FFFF00U;

    if (pgn_match == (CAN_PGN_SPEED_BRAKE & 0x00FFFF00U)) {
        /* Speed: Bytes 0-1 (0.1 km/h per bit) */
        uint16_t raw_speed = (uint16_t)p_frame->data[0] | ((uint16_t)p_frame->data[1] << 8U);
        s_telemetry.speed_kmh_x10 = raw_speed;
        s_telemetry.flag_brake_active = (p_frame->data[2] & 0x01U) != 0U;
        decoded = true;
    } else if (pgn_match == (CAN_PGN_MOTOR_SPEED & 0x00FFFF00U)) {
        /* Motor RPM: Bytes 3-4 (1 RPM per bit) */
        uint16_t raw_rpm = (uint16_t)p_frame->data[3] | ((uint16_t)p_frame->data[4] << 8U);
        s_telemetry.motor_rpm = raw_rpm;
        decoded = true;
    } else if (pgn_match == (CAN_PGN_BATTERY_SOC & 0x00FFFF00U)) {
        /* SOC: Byte 0 (0-100%), Voltage: Bytes 1-2 (0.1 V per bit) */
        s_telemetry.battery_soc_pct = p_frame->data[0];
        if (s_telemetry.battery_soc_pct > 100U) {
            s_telemetry.battery_soc_pct = 100U;
        }
        s_telemetry.battery_voltage_x10 = (uint16_t)p_frame->data[1] | ((uint16_t)p_frame->data[2] << 8U);
        s_telemetry.flag_battery_low = (s_telemetry.battery_soc_pct <= 15U);
        decoded = true;
    } else if (pgn_match == (CAN_PGN_TELLTALES & 0x00FFFF00U)) {
        /* Status Flags: Byte 0 */
        uint8_t flags = p_frame->data[0];
        s_telemetry.flag_turn_left      = (flags & 0x01U) != 0U;
        s_telemetry.flag_turn_right     = (flags & 0x02U) != 0U;
        s_telemetry.flag_high_beam      = (flags & 0x04U) != 0U;
        s_telemetry.flag_hazard         = (flags & 0x08U) != 0U;
        s_telemetry.flag_battery_low    = (flags & 0x10U) != 0U;
        s_telemetry.flag_motor_overtemp = (flags & 0x20U) != 0U;
        decoded = true;
    } else {
        /* Unrecognized PGN */
    }

    if (decoded) {
        s_telemetry.total_frames_rx++;
        s_telemetry.is_comm_active = true;
    }

    return decoded;
}

void can_telemetry_poll(uint32_t current_time_ms)
{
    /* 1. Drain incoming CAN hardware FIFO frames */
    can_frame_t rx_frame;
    while (hal_can_receive(&rx_frame) == HAL_CAN_OK) {
        if (can_telemetry_process_frame(&rx_frame)) {
            s_telemetry.last_rx_timestamp_ms = current_time_ms;
        }
    }

    /* 2. Check Communication Loss Watchdog */
    if (s_telemetry.is_comm_active) {
        if ((current_time_ms - s_telemetry.last_rx_timestamp_ms) > CAN_COMM_TIMEOUT_MS) {
            s_telemetry.is_comm_active = false;
        }
    }

    /* 3. Failsafe Needle Glide Decay if comm lost */
    if (!s_telemetry.is_comm_active) {
        if (s_telemetry.speed_kmh_x10 > 5U) {
            s_telemetry.speed_kmh_x10 = (uint16_t)((s_telemetry.speed_kmh_x10 * 9U) / 10U);
        } else {
            s_telemetry.speed_kmh_x10 = 0U;
        }

        if (s_telemetry.motor_rpm > 50U) {
            s_telemetry.motor_rpm = (uint16_t)((s_telemetry.motor_rpm * 9U) / 10U);
        } else {
            s_telemetry.motor_rpm = 0U;
        }
    }
}

void can_telemetry_get_data(vehicle_telemetry_t *p_out)
{
    if (p_out != (void *)0) {
        *p_out = s_telemetry;
    }
}

void can_telemetry_inject_speed(uint16_t speed_kmh_x10, uint16_t rpm)
{
    can_frame_t f1;
    f1.id = CAN_PGN_SPEED_BRAKE;
    f1.dlc = 8;
    f1.is_extended = true;
    f1.is_rtr = false;
    f1.data[0] = (uint8_t)(speed_kmh_x10 & 0xFFU);
    f1.data[1] = (uint8_t)((speed_kmh_x10 >> 8U) & 0xFFU);
    f1.data[2] = 0;
    (void)can_telemetry_process_frame(&f1);

    can_frame_t f2;
    f2.id = CAN_PGN_MOTOR_SPEED;
    f2.dlc = 8;
    f2.is_extended = true;
    f2.is_rtr = false;
    f2.data[3] = (uint8_t)(rpm & 0xFFU);
    f2.data[4] = (uint8_t)((rpm >> 8U) & 0xFFU);
    (void)can_telemetry_process_frame(&f2);
}

void can_telemetry_inject_battery(uint8_t soc_pct, uint16_t voltage_x10)
{
    can_frame_t f;
    f.id = CAN_PGN_BATTERY_SOC;
    f.dlc = 8;
    f.is_extended = true;
    f.is_rtr = false;
    f.data[0] = soc_pct;
    f.data[1] = (uint8_t)(voltage_x10 & 0xFFU);
    f.data[2] = (uint8_t)((voltage_x10 >> 8U) & 0xFFU);
    (void)can_telemetry_process_frame(&f);
}

void can_telemetry_inject_telltales(bool left, bool right, bool high_beam, bool batt_low, bool fault)
{
    can_frame_t f;
    f.id = CAN_PGN_TELLTALES;
    f.dlc = 8;
    f.is_extended = true;
    f.is_rtr = false;
    f.data[0] = 0;
    if (left)      f.data[0] |= 0x01U;
    if (right)     f.data[0] |= 0x02U;
    if (high_beam) f.data[0] |= 0x04U;
    if (batt_low)  f.data[0] |= 0x10U;
    if (fault)     f.data[0] |= 0x20U;
    (void)can_telemetry_process_frame(&f);
}
