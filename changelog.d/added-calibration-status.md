Added an optional `calibration_status` diagnostic text sensor reporting the
active calibration mode and the NVS stock-calibration result, with unit tests for
all status values. Also moved UART validation to `final_validate_device_schema`
to remove the deprecated `check_uart_settings` call.
