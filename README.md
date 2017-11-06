# tec-temp-controller

Temperature controller

Equipment: Arduino Uno, 10k NTC Thermistor, Thermoelectric cooling (TEC), AD590, 555 timer and H-Bridge.

Temperature controller works between 5C and 45C.

Setup: Arduino Uno is connected to the H-Bridge, the H-Bridge controls the direction
of the current to the TEC. The thermistor is attached to the metal plate where the TEC is.
The thermistor works as one of the resistors in the 555 timer, giving different frequencies
at different resistances.
