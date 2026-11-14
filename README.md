JsquaredZKITTSCANNER
====================

Knight Rider (KITT) style LED scanner for Arduino, using the SoftPWM library.

Features
--------

- Bright scanning head with a fading tail that trails behind it, like the original
- Head glides between LEDs in sub-steps instead of hopping
- Brief hold at each end while the tail keeps fading
- Gamma corrected brightness so the dim tail steps are visible
- Software PWM on every LED pin (no hardware PWM pins needed)
- Pot on A1 controls scan speed (25 to 400 ms per LED)
- Pot on A0 controls maximum LED brightness (40% to 100%), i.e. power consumption
- Current settings printed over serial at 9600 baud once per round trip

Wiring
------

- LEDs on digital pins 2 to 10, left to right, each through a resistor
- Speed pot wiper on A1
- Brightness pot wiper on A0

Tuning
------

The constants at the top of the sketch control the look:

- `SUBSTEPS` frames per LED, higher is smoother
- `TAIL_FALLOFF` fraction of brightness kept per LED behind the head, 0.3 short tail, 0.5 long tail
- `END_HOLD_RATIO` pause at each end as a fraction of the time per LED

Requires the SoftPWM library (Library Manager: "SoftPWM" by Brett Hagman).
