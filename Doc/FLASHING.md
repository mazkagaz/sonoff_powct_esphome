**English** | [Français](FLASHING_FR.md)

# Flashing the Sonoff POW CT — choosing the right image

This guide explains which file to flash, why an **app-only** image can leave the
device in a boot loop, and how to recover. It applies to ESPHome and Tasmota.

> **Safety first.** The POW CT is connected to mains. Work with the device
> unplugged, never connect mains and a USB/serial adapter at the same time, and
> use a proper 3.3 V supply with a common ground. If you are not comfortable with
> mains wiring, ask a qualified person.

## 1. The three boot stages

An ESP32 does not boot an application directly:

1. the **ROM bootloader** runs first and looks for a boot image in flash;
2. the **second-stage bootloader** (ESP-IDF or Tasmota) prepares the flash and
   selects the application;
3. the **application** starts and initializes Wi-Fi, the display, the CSE7761,
   etc.

If the ROM cannot read a valid second-stage bootloader, the serial log usually
shows `invalid header: 0xffffffff` and resets. That happens **before** the
application: the CSE7761 and its calibration are not involved.

## 2. Factory image vs app-only/OTA image

| | Factory image | App-only / OTA image |
|---|---|---|
| Contains | Bootloader + partition table + application | Application only |
| Used for | First flash, recovery | OTA update, or writing to the app partition |
| Written at | `0x0` (merged image; the tool handles offsets) | App partition offset, which depends on the partition table |
| Boots alone? | Yes | Only if a matching bootloader/partition table is already present |

A factory image is a **complete installation**. An app-only image is a
**replacement program**: it needs the existing bootloader and partition table to
point to the right place.

### ESPHome

- The **ESPHome Web Flasher** and the **Install** action in the dashboard write a
  complete factory image (bootloader + partition table + application)
  automatically.
- If you use **Manual download**, choose the **factory/merged** format if your
  ESPHome version offers it (often named `firmware.factory.bin`). The file named
  `firmware.bin` is the application image and is not suitable for a first flash.
- The exact file names depend on the ESPHome version. If you are unsure, use the
  Web Flasher/dashboard rather than guessing.
- Never write an app-only `firmware.bin` at `0x0` or `0x10000`.

### Tasmota

- `tasmota32.factory.bin` is the **complete installation image** for the first
  flash.
- `tasmota32.bin` is the **application** for OTA updates (or for the app offset).
  It is not a first-flash image.
- The Tasmota Web Installer selects the factory image automatically. If you
  flashed `tasmota32.bin` at the start of flash, erase and flash the factory
  image again.

## 3. Choosing the right file

| Situation | Use |
|---|---|
| First flash, blank or unknown device | Factory image |
| Recovery after a failed flash | Factory image after `erase_flash` |
| Normal update of a working device | OTA / app-only image |
| You only have `firmware.bin`/`tasmota32.bin` | Do not write it at `0x0`; obtain a factory image or use the Web Flasher |

**About offsets:** the tested stock POWCT layout places the application at
`0x20000`, while a default ESPHome 4 MB layout often places app0 at `0x10000`.
These values belong to their respective partition tables. A factory image writes
its own table; an app-only image is only valid at the offset of the table
currently on the device. If you do not know the table, use a factory image.

## 4. Manual recovery with esptool

Use `esptool.py` (or `esptool` in newer releases). Replace `<PORT>` with your
serial port. The standard POWCT is an ESP32; adjust `--chip` only if your tools
report a different chip.

### 4.1 Back up first (recommended)

`erase_flash` wipes the NVS, including factory calibration data. If you may want
to return to the stock firmware, back up the full flash first:

```bash
esptool.py --chip esp32 --port <PORT> read_flash 0 0x400000 backup.bin
```

Adjust the size if `flash_id` reports a different flash size.

### 4.2 Erase

```bash
esptool.py --chip esp32 --port <PORT> erase_flash
```

### 4.3 Flash a factory image

```bash
esptool.py --chip esp32 --port <PORT> --baud 460800 write_flash 0x0 <factory-image>.bin
```

- `0x0` is correct for a merged **factory** image.
- If your adapter is unreliable at 460800, omit `--baud` or use `115200`.
- If the esptool stub fails on your setup, you can try `--no-stub` (older
  documentation may mention `-nostub`; modern esptool uses `--no-stub`).
- Put the device in bootloader mode by holding GPIO0 low during reset, then
  release it after the connection is established.

### 4.4 After flashing

Let the device boot and create its Wi-Fi access point. Use OTA/app-only images
for later updates; do not re-flash the app-only image at `0x0`.

## 5. If it still shows `invalid header: 0xffffffff`

The ROM cannot read a valid bootloader. Collect:

- the exact file, type (factory/app-only), command and offset used;
- the full serial log from reset (not only the repeating lines);
- `esptool.py --chip esp32 --port <PORT> flash_id` (flash manufacturer, size,
  voltage);
- `espefuse.py --port <PORT> summary` in read-only mode (secure boot, flash
  encryption, VDD_SDIO);
- a photo of both sides of the board and the flash part number;
- the 3.3 V rail at boot (oscilloscope if possible) and the supply used.

This distinguishes a flash-mode/size/voltage problem, an eFuse problem, a power
problem and a hardware-variant problem.

## 6. Hardware variants

The repository was developed on the standard Sonoff POW CT. Other variants,
including the **POW CT “Ring” 100 A**, may use a different board, flash chip or
fuse configuration. They are **not guaranteed** to work with the same board
definition. If you have such a variant, please provide the diagnostics above
(`flash_id`, `espefuse summary`, photos, log) so it can be documented.

## 7. Checklist

- [ ] I am using a **factory** image for the first flash, not an app-only image.
- [ ] I backed up the full flash if I may need the stock calibration.
- [ ] I know my serial port and the `--chip` reported by esptool.
- [ ] I have the full serial log if the boot still fails.
- [ ] I can provide `flash_id`, `espefuse summary`, and board photos.

## 8. Official references

- ESPHome Web Flasher: <https://web.esphome.io/>
- ESPHome physical connection guide:
  <https://esphome.io/guides/physical_device_connection/>
- Tasmota getting started: <https://tasmota.github.io/docs/Getting-Started/>
- Tasmota flashing: <https://tasmota.github.io/docs/Flashing/>
- Espressif esptool documentation:
  <https://docs.espressif.com/projects/esptool/en/latest/esp32/>
