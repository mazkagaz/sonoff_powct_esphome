import esphome.codegen as cg
from esphome.components import sensor, uart, text_sensor, api
import esphome.config_validation as cv
from esphome.const import (
    CONF_ID,
    CONF_VOLTAGE,
    DEVICE_CLASS_CURRENT,
    DEVICE_CLASS_POWER,
    DEVICE_CLASS_VOLTAGE,
    STATE_CLASS_MEASUREMENT,
    UNIT_AMPERE,
    UNIT_VOLT,
    UNIT_WATT,
)

CODEOWNERS = ["@berfenger"]
DEPENDENCIES = ["uart", "api"]

cse7761_ns = cg.esphome_ns.namespace("cse7761")
CSE7761Component = cse7761_ns.class_(
    "CSE7761Component", cg.PollingComponent, uart.UARTDevice)

CONF_CURRENT_1 = "current_1"
CONF_CURRENT_2 = "current_2"
CONF_ACTIVE_POWER_1 = "active_power_1"
CONF_ACTIVE_POWER_2 = "active_power_2"

# ID pour le Text Sensor
CONF_DEBUG_SENSOR_HEX_ID = "debug_sensor_hex_id"
CONF_DEBUG_SENSOR_BIN_ID = "debug_sensor_bin_id"

CONFIG_SCHEMA = (
    cv.Schema(
        {
            cv.GenerateID(): cv.declare_id(CSE7761Component),
            cv.Optional(CONF_VOLTAGE): sensor.sensor_schema(
                unit_of_measurement=UNIT_VOLT,
                accuracy_decimals=1,
                device_class=DEVICE_CLASS_VOLTAGE,
                state_class=STATE_CLASS_MEASUREMENT,
            ),
            cv.Optional(CONF_CURRENT_1): sensor.sensor_schema(
                unit_of_measurement=UNIT_AMPERE,
                accuracy_decimals=2,
                device_class=DEVICE_CLASS_CURRENT,
                state_class=STATE_CLASS_MEASUREMENT,
            ),
            cv.Optional(CONF_CURRENT_2): sensor.sensor_schema(
                unit_of_measurement=UNIT_AMPERE,
                accuracy_decimals=2,
                device_class=DEVICE_CLASS_CURRENT,
                state_class=STATE_CLASS_MEASUREMENT,
            ),
            cv.Optional(CONF_ACTIVE_POWER_1): sensor.sensor_schema(
                unit_of_measurement=UNIT_WATT,
                accuracy_decimals=1,
                device_class=DEVICE_CLASS_POWER,
                state_class=STATE_CLASS_MEASUREMENT,
            ),
            cv.Optional(CONF_ACTIVE_POWER_2): sensor.sensor_schema(
                unit_of_measurement=UNIT_WATT,
                accuracy_decimals=1,
                device_class=DEVICE_CLASS_POWER,
                state_class=STATE_CLASS_MEASUREMENT,
            ),
            cv.Optional(CONF_DEBUG_SENSOR_HEX_ID): cv.use_id(text_sensor.TextSensor),
            cv.Optional(CONF_DEBUG_SENSOR_BIN_ID): cv.use_id(text_sensor.TextSensor),
        }
    )
    .extend(cv.polling_component_schema("60s"))
    .extend(uart.UART_DEVICE_SCHEMA)
    .extend(cv.COMPONENT_SCHEMA)
)

FINAL_VALIDATE_SCHEMA = uart.final_validate_device_schema(
    "cse7761", baud_rate=38400, require_rx=True, require_tx=True
)


async def to_code(config):
    var = cg.new_Pvariable(config[CONF_ID])
    await cg.register_component(var, config)
    await uart.register_uart_device(var, config)
    
    for key in [
        CONF_VOLTAGE,
        CONF_CURRENT_1,
        CONF_CURRENT_2,
        CONF_ACTIVE_POWER_1,
        CONF_ACTIVE_POWER_2,
    ]:
        if key not in config:
            continue
        conf = config[key]
        sens = await sensor.new_sensor(conf)
        cg.add(getattr(var, f"set_{key}_sensor")(sens))
    # Si le text_sensor est configuré, appelez le setter C++
    if debug_sensor_hex_config := config.get(CONF_DEBUG_SENSOR_HEX_ID):
        debug_sensor_hex = await cg.get_variable(debug_sensor_hex_config)
        cg.add(var.set_debug_text_sensor_hex(debug_sensor_hex))
    if debug_sensor_bin_config := config.get(CONF_DEBUG_SENSOR_BIN_ID):
        debug_sensor_bin = await cg.get_variable(debug_sensor_bin_config)
        cg.add(var.set_debug_text_sensor_bin(debug_sensor_bin))

