#ifndef HEATING_CONTROLLER_H
#define HEATING_CONTROLLER_H

#include <Arduino.h>
#include "esp_log.h"

// Configuration defines
#define PWM_PERIOD_MS           10000    // 10 seconds period in milliseconds
#define PWM_FREQUENCY           1        // 0.1 Hz (10 second period)
#define PWM_RESOLUTION          10       // 10-bit resolution (0-1023)

// I²t Protection Configuration
#define I2T_WINDOW_SIZE         120      // Sliding window size in seconds
#define I2T_SAMPLE_INTERVAL     1000     // Sample interval in milliseconds
#define MAX_I2T_VALUE           200000.0 // Maximum I²t value (adjust based on resistor specs)
#define POWER_REDUCTION_FACTOR  0.95     // Factor to reduce power when approaching limits

// Temperature compensation
#define TEMP_SAFETY_THRESHOLD   120.0    // Temperature in °C above which to be more cautious
#define TEMP_DERATING_FACTOR    0.85     // Reduce max I²t when temperature is high

// Power limits
#define MAX_POWER_PERCENT       100      // Maximum power output percentage
#define MIN_POWER_PERCENT       0        // Minimum power output percentage
#define POWER_STEPS             100      // Number of discrete power steps

// PWM Configuration - now properly defined
#define MAX_PWM_CHANNELS        8        // ESP32 has 8 LEDC channels
#define MAX_PWM_TIMERS          4        // ESP32 has 4 LEDC timers

class HeatingController {
private:
    static const char* TAG;
    static uint8_t next_available_channel;
    static uint8_t next_available_timer;
    
    // PWM control - now properly managed per instance
    int pwm_pin;
    uint8_t pwm_channel;
    uint8_t pwm_timer;
    uint32_t pwm_duty_cycle;
    
    // Power control
    float desired_power_percent;
    float actual_power_percent;
    float limited_power_percent;
    
    // I²t protection
    float i2t_samples[I2T_WINDOW_SIZE * (1000 / I2T_SAMPLE_INTERVAL)];
    int i2t_sample_index;
    float current_i2t_value;
    unsigned long last_sample_time;
    
    // Temperature compensation
    float oven_temperature;
    bool temperature_valid;
    
    // Internal methods
    void updateI2tValue();
    float calculatePowerLimit();
    void updatePWM();
    float getTemperatureDerating() const; // Fixed const-correctness
    uint32_t powerToNearestDutyCycle(float power_percent);
    
public:
    HeatingController(int pin);
    ~HeatingController();
    
    // Initialization
    bool begin();
    
    // Main control methods
    void setDesiredPower(float power_percent);
    void setOvenTemperature(float temperature_celsius);
    void update(); // Call this regularly in main loop
    
    // Status getters
    float getDesiredPower() const { return desired_power_percent; }
    float getActualPower() const { return actual_power_percent; }
    float getLimitedPower() const { return limited_power_percent; }
    float getCurrentI2t() const { return current_i2t_value; }
    float getMaxI2t() const;
    bool isTemperatureValid() const { return temperature_valid; }
    float getOvenTemperature() const { return oven_temperature; }
    
    // Protection status
    bool isI2tLimiting() const;
    float getI2tUtilization() const; // Returns 0-1.0 (0-100%)
};

#endif // HEATING_CONTROLLER_H