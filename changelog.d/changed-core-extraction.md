Extracted protocol, conversion, calibration and energy logic into host-testable
`cse7761_core_*` headers and added 21 doctest cases. No intended measurement
change; `millis()` wrap and zero-coefficient division are now guarded.
