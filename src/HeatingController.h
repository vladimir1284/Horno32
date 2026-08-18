#ifndef HEATING_CONTROLLER_H
#define HEATING_CONTROLLER_H

#include <Arduino.h>

// Time-proportioning control: over each PWM_PERIOD_MS window, the output
// pin is held HIGH for (power% * period) and LOW the rest. This is plain
// digitalWrite bit-banging, not hardware LEDC PWM — the SSR modules on
// this board switch far too slowly (seconds-scale window) for LEDC's
// integer-Hz frequency range to express correctly.
#define PWM_PERIOD_MS 10000 // 10 second window

// I2t protection (sliding window RMS-power limiting)
#define I2T_WINDOW_SIZE 120       // seconds
#define I2T_SAMPLE_INTERVAL 1000  // ms
#define MAX_I2T_VALUE 200000.0    // adjust based on resistor specs
#define POWER_REDUCTION_FACTOR 0.95

// Temperature derating
#define TEMP_SAFETY_THRESHOLD 120.0
#define TEMP_DERATING_FACTOR 0.85

#define MAX_POWER_PERCENT 100
#define MIN_POWER_PERCENT 0

class HeatingController
{
public:
    // hard_max_duty_percent: absolute safety ceiling (see PinConfig.h,
    // HEATER_*_SAFE_MAX_DUTY_PERCENT). Not settable at runtime by design —
    // change it by recompiling, never expose it over REST/MQTT.
    HeatingController(int pin, float hard_max_duty_percent);

    bool begin();

    void setDesiredPower(float power_percent);
    void setOvenTemperature(float temperature_celsius);
    void update(); // call regularly (every <= a few hundred ms)

    float getDesiredPower() const { return desired_power_percent; }
    float getActualPower() const { return actual_power_percent; }
    float getHardMaxDuty() const { return hard_max_duty_percent; }
    float getCurrentI2t() const { return current_i2t_value; }
    float getMaxI2t() const;
    bool isI2tLimiting() const;
    float getI2tUtilization() const; // 0.0 - 1.0

private:
    int pin;
    float hard_max_duty_percent;
    bool output_state;
    unsigned long window_start;
    unsigned long last_sample_time;

    float desired_power_percent;
    float actual_power_percent;

    float i2t_samples[I2T_WINDOW_SIZE * (1000 / I2T_SAMPLE_INTERVAL)];
    int i2t_sample_index;
    float current_i2t_value;

    float oven_temperature;
    bool temperature_valid;

    void updateI2tValue();
    float calculatePowerLimit();
    float getTemperatureDerating() const;
};

#endif // HEATING_CONTROLLER_H
