# ==============================================================================
# Makefile for Automotive Digital TFT Instrument Cluster (automotive-tft-cluster)
# Author: S. Sherifred Singh (Senior Embedded Firmware Engineer)
# ==============================================================================

CC ?= gcc
ifeq ($(CC),cc)
  CC = gcc
endif
CFLAGS ?= -std=c99 -Wall -Wextra -Werror -pedantic -O2 -I. -Ifirmware/hal -Ifirmware/core_gfx -Ifirmware/services -Ifirmware/app -Ifirmware/port/host_sim -Itests/unity

BUILD_DIR = build

# Core Firmware Sources
FW_SRCS = \
	firmware/core_gfx/gfx_trig_lut.c \
	firmware/core_gfx/gfx_dirty_rect.c \
	firmware/core_gfx/font_embedded.c \
	firmware/core_gfx/gfx_engine.c \
	firmware/services/can_telemetry.c \
	firmware/services/odometer_service.c \
	firmware/services/telltale_manager.c \
	firmware/app/cluster_screens.c \
	firmware/app/cluster_app.c \
	firmware/port/host_sim/port_host_display.c \
	firmware/port/host_sim/port_host_can.c \
	firmware/port/host_sim/port_host_nvm.c \
	firmware/port/host_sim/port_host_timer.c \
	firmware/port/host_sim/port_host_gpio.c

# Test Sources
TEST_SRCS = \
	tests/unity/unity.c \
	tests/unit/test_gfx_trig_lut.c \
	tests/unit/test_gfx_engine.c \
	tests/unit/test_can_telemetry.c \
	tests/unit/test_odometer_wear_leveling.c \
	tests/unit/test_telltale_manager.c \
	tests/integration/test_cluster_integration.c \
	tests/test_runner.c

# Simulator Source
SIM_SRCS = tools/sim_dashboard/main_dashboard_sim.c

.PHONY: all test sim clean

all: test sim

ifeq ($(OS),Windows_NT)
  MKDIR_BUILD = if not exist $(BUILD_DIR) mkdir $(BUILD_DIR)
  RM_BUILD = if exist $(BUILD_DIR) rmdir /s /q $(BUILD_DIR)
else
  MKDIR_BUILD = mkdir -p $(BUILD_DIR)
  RM_BUILD = rm -rf $(BUILD_DIR)
endif

$(BUILD_DIR):
	@$(MKDIR_BUILD)

test: $(BUILD_DIR)
	$(CC) $(CFLAGS) -o $(BUILD_DIR)/test_runner.exe $(FW_SRCS) $(TEST_SRCS)
	$(BUILD_DIR)/test_runner.exe

sim: $(BUILD_DIR)
	$(CC) $(CFLAGS) -o $(BUILD_DIR)/sim_dashboard.exe $(FW_SRCS) $(SIM_SRCS)
	@echo "Build complete: $(BUILD_DIR)/sim_dashboard.exe"

clean:
	@$(RM_BUILD)
