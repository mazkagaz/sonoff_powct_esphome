Added the stock 18-coefficient calibration core model, stock conversions,
quadratic application, 500 mA range selection, identity fallback and NVS payload
parser, with host tests. Added opt-in `calibration_mode: stock` with a read-only
NVS probe; `offset` remains the default and hardware accuracy validation is
pending.
