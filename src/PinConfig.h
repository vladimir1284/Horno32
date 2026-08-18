#ifndef PinConfig_h
#define PinConfig_h

// Pin assignments, sourced from Schematic.fzz (Fritzing netlist).
// Assumption: PCB matches this schematic exactly. Verify against the
// physical board silkscreen/traces before relying on these during bring-up.

// MAX6675 thermocouple amplifier (SPI-like bit-bang)
#define THERMO_CLK_PIN 5
#define THERMO_CS_PIN 18
#define THERMO_DO_PIN 19

// HT1621 LCD
#define LCD_CS_PIN 21
#define LCD_WR_PIN 22
#define LCD_DATA_PIN 23

// Buzzer driver (transistor U8 via resistor R8)
#define BUZZER_PIN 25

// Heater SSR drivers (transistors U9/U10).
// TODO: which physical relay (RY1 on GPIO27 / RY2 on GPIO26) feeds the
// top vs bottom resistance is NOT derivable from the schematic alone.
// Confirm during hardware bring-up (turn on one at a time, see which
// resistor heats) and fix these defines if swapped.
#define HEATER_TOP_PIN 27
#define HEATER_BOTTOM_PIN 26

// Hard safety ceiling on duty cycle, in percent. This is a firmware
// constant, not a user setting (unlike HornoSettings.maxPowerUp/Down,
// which stay user-adjustable and are capped BY this value, never above
// it). Exists because the installed resistances are off-the-shelf
// electric-stove elements, not oven-rated ones: run one at 100% duty in
// free air with nothing drawing the heat off it and it will burn out
// fast. This is the number that defines what "100%" means to the PID
// and to the user-facing power sliders.
//
// PLACEHOLDER — not measured yet. Determine empirically per resistance
// before any unattended run: start low (e.g. 20-30%), short exposures
// (1-2 min) with the resistor in free air as worst case, watch/feel for
// discoloration, smell, or the element glowing unevenly; raise in small
// steps between tests, always with time to cool between attempts. Stop
// well below the point where you see any of those signs. Fix these two
// values once you have real numbers — top and bottom may differ since
// "top" and "bottom" could be different resistor models.
#define HEATER_TOP_SAFE_MAX_DUTY_PERCENT 40
#define HEATER_BOTTOM_SAFE_MAX_DUTY_PERCENT 40

#endif
