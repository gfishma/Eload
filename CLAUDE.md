# CLAUDE.md

This file provides guidance to Claude Code (claude.ai/code) when working with code in this repository.

## Build & Flash

PlatformIO is at `/c/Users/10631/.platformio/penv/Scripts/platformio.exe`. Python (with pyserial) is at the same path.

```bash
# PlatformIO build
/c/Users/10631/.platformio/penv/Scripts/platformio.exe run

# Flash with ST-Link
/c/Users/10631/.platformio/penv/Scripts/platformio.exe run -t upload

# Test via serial (COM5)
/c/Users/10631/.platformio/penv/Scripts/python.exe -c "
import serial,time; s=serial.Serial('COM5',115200,timeout=2);
time.sleep(1); s.write(b'>status\r\n'); time.sleep(1);
print(s.read(999).decode('ascii',errors='replace'))"
```

Target: `genericSTM32F407VGT6` (Cortex-M4F, 168MHz, 1MB Flash, 128KB RAM). Linker: `STM32F407VGTx_FLASH.ld`.

## Serial Monitor

`tools/serial_monitor.py` captures STM32 UART output (COM5, 115200 baud):

```bash
# Capture 10 seconds, send a command
python tools/serial_monitor.py --capture 10 --cmd ">status\r\n"
```

## Architecture (Refactored Jun 2025)

This is a **CC-mode electronic load** firmware on STM32F407VGT6 with **FreeRTOS** (CMSIS-RTOS v1) and STM32 HAL.

### File Layout

```
Core/
├── Inc/
│   ├── eload.h          ← Core state machine, globals, protection thresholds
│   ├── eload_ui.h       ← LCD display + keypad interface
│   ├── calibration.h    ← 16-point piecewise linear calibration
│   ├── config.h         ← Tunable parameters
│   └── app_tasks.h      ← FreeRTOS task declarations
├── Src/
│   ├── eload.c          ← State machine: ON/OFF, DAC write, cal apply, protection
│   ├── eload_cmd.c      ← UART command parser: >set_curr, >help, >status, >eload *
│   ├── eload_ui.c       ← LCD refresh + 4-key debounce
│   ├── calibration.c    ← Auto-sweep, sample, EEPROM save/load, interp apply
│   ├── app_tasks.c      ← Thin FreeRTOS glue (delegates to eload_*)
│   ├── config.c         ← Global init, calls eload_init()
│   └── freertos.c       ← Task creation, Display_Task LCD init
iCore/
├── AD5667/              ← DAC driver (I2C2, 0x0F, 16-bit)
├── 24cxx/               ← CAT24C08 EEPROM driver (I2C1, 0xA4, **8-bit addr**)
├── Tftlcd/              ← TFT LCD (SPI, 240x320)
├── Temp/                ← NTC thermistor
└── adc_dma/             ← STM32 ADC via DMA
```

### Key Global

```c
// eload.h
eload_t g_eload;  // single global instance with all state
```

### FreeRTOS Tasks

| Task | What it does |
|------|-------------|
| `OLEDControl` | Calls `eload_cmd_dispatch()` for UART command parsing |
| `DisplayUpdate` | Calls `eload_ui_display()` to refresh LCD |
| `KeyProcess` | Calls `eload_ui_keypad()` — SET=ON/OFF, ▲▼=adjust, CONFIG=step |
| `DataAcquisition` | Calls `eload_read_measurements()` via DVM CH1/CH2 |
| `MonitorProtection` | Reads NTC, calls `eload_check_protection()` |

### Calibration System

16-point piecewise linear calibration, 0-2A (expandable to 13A):

```
50, 100, 150, 200, 300, 400, 500, 650, 800, 900,
1000, 1150, 1300, 1500, 1700, 2000 mA
```

- Each point: set current → wait 1s → DVM sample 10× → discard hi/lo → avg 8
- Saved to CAT24C08 EEPROM (I2C1 addr 0xA4, **8-bit memory addressing**)
- Applied via `Calibration_Apply(target_ma)` → returns DAC-compensated value
- Persists across flash/reflash (external EEPROM)
- Coeff applied in `eload_set_output()` automatically

### Serial Commands (all `>` prefixed)

| Command | Action |
|---------|--------|
| `>set_curr(XXXX.XXmA)` | Set target current |
| `>output(on/off)` | Toggle output |
| `>help` | Show command list |
| `>status` | Show V/I/P/T/Cal state |
| `>eload cal` | Run 16-point calibration |
| `>eload info` | Print calibration table |

### Keypad Layout

| Key | Function |
|-----|----------|
| SET | ON/OFF toggle |
| ▲ | Current + step |
| ▼ | Current - step |
| CONFIG | Cycle step 1→100→1000 mA |

### Protection

| Type | Threshold | Action |
|------|-----------|--------|
| OTP | 70°C | Output OFF |
| OVP | 30V | Output OFF |
| OPP | 50W | Output OFF |

### Critical Fixes (session history)

- **EEPROM addressing**: Must use `I2C_MEMADD_SIZE_8BIT` for CAT24C08. 16-bit causes 1-byte shift → all data corrupted.
- **DAC zero**: Use `AD5667_WriteData_without(0)` for true 0V (bypasses +19mV offset in `WritData_setcurr`).
- **Cal load timing**: Must happen in `Config_Init()` (pre-FreeRTOS) so keypad has coefficients ready.
- **AD9910 driver**: SPI Mode 2 (CPOL=1) with BSRR bit-bang via PE8/10/12/14/PA5. CFR3 PLL bits: EN=bit8, VCO=bits26:24, N=bits7:1.

### 13A Upgrade Checklist

When high-power supply arrives, change 3 lines:
1. `calibration.h`: `CAL_POINT_COUNT` → 24+
2. `calibration.c`: replace `cal_setpoints[]` with 24-pt 0-13A array
3. `config.h` + `eload.h`: `MAX_CURRENT` → 13000
