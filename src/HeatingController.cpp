#include "HeatingController.h"
#include "driver/ledc.h"

const char* HeatingController::TAG = "HeatingController";

// Initialize static members for channel/timer management
uint8_t HeatingController::next_available_channel = 0;
uint8_t HeatingController::next_available_timer = 0;

HeatingController::HeatingController(int pin) {
    pwm_pin = pin;
    
    // Assign unique channel and timer to each instance
    pwm_channel = next_available_channel;
    pwm_timer = next_available_timer;
    
    // Increment for next instance (with wraparound)
    next_available_channel = (next_available_channel + 1) % MAX_PWM_CHANNELS;
    if (next_available_channel == 0) {
        next_available_timer = (next_available_timer + 1) % MAX_PWM_TIMERS;
    }
    
    pwm_duty_cycle = 0;
    desired_power_percent = 0.0;
    actual_power_percent = 0.0;
    limited_power_percent = 0.0;
    
    // Initialize I²t protection arrays
    for (int i = 0; i < (I2T_WINDOW_SIZE * (1000 / I2T_SAMPLE_INTERVAL)); i++) {
        i2t_samples[i] = 0.0;
    }
    i2t_sample_index = 0;
    current_i2t_value = 0.0;
    last_sample_time = 0;
    
    // Initialize temperature
    oven_temperature = 30.0; // Default room temperature
    temperature_valid = false;
}

HeatingController::~HeatingController() {
    // Stop PWM output
    ledc_stop(LEDC_LOW_SPEED_MODE, static_cast<ledc_channel_t>(pwm_channel), 0);
}

bool HeatingController::begin() {
    // Configure LEDC timer
    ledc_timer_config_t timer_config = {
        .speed_mode = LEDC_LOW_SPEED_MODE,
        .duty_resolution = static_cast<ledc_timer_bit_t>(PWM_RESOLUTION),
        .timer_num = static_cast<ledc_timer_t>(pwm_timer),
        .freq_hz = PWM_FREQUENCY,
        .clk_cfg = LEDC_AUTO_CLK
    };
    
    esp_err_t timer_result = ledc_timer_config(&timer_config);
    if (timer_result != ESP_OK) {
        ESP_LOGE(TAG, "Failed to configure LEDC timer %d: %s", pwm_timer, esp_err_to_name(timer_result));
        return false;
    }
    
    // Configure LEDC channel
    ledc_channel_config_t channel_config = {
        .gpio_num = pwm_pin,
        .speed_mode = LEDC_LOW_SPEED_MODE,
        .channel = static_cast<ledc_channel_t>(pwm_channel),
        .intr_type = LEDC_INTR_DISABLE,
        .timer_sel = static_cast<ledc_timer_t>(pwm_timer),
        .duty = 0,
        .hpoint = 0
    };
    
    esp_err_t channel_result = ledc_channel_config(&channel_config);
    if (channel_result != ESP_OK) {
        ESP_LOGE(TAG, "Failed to configure LEDC channel %d: %s", pwm_channel, esp_err_to_name(channel_result));
        return false;
    }
    
    last_sample_time = millis();
    
    ESP_LOGI(TAG, "HeatingController initialized on pin %d (channel %d, timer %d)", 
             pwm_pin, pwm_channel, pwm_timer);
    ESP_LOGI(TAG, "PWM Period: %d ms, I²t Window: %d seconds", PWM_PERIOD_MS, I2T_WINDOW_SIZE);
    
    return true;
}

void HeatingController::setDesiredPower(float power_percent) {
    // Clamp to valid range and round to nearest step
    power_percent = constrain(power_percent, MIN_POWER_PERCENT, MAX_POWER_PERCENT);
    
    // Round to nearest power step
    int power_step = round(power_percent);
    desired_power_percent = static_cast<float>(power_step);
    
    ESP_LOGD(TAG, "Desired power set to: %.1f%%", desired_power_percent);
}

void HeatingController::setOvenTemperature(float temperature_celsius) {
    oven_temperature = temperature_celsius;
    temperature_valid = true;
    ESP_LOGD(TAG, "Oven temperature updated: %.1f°C", oven_temperature);
}

void HeatingController::update() {
    unsigned long current_time = millis();
    
    // Update I²t calculation at regular intervals
    if (current_time - last_sample_time >= I2T_SAMPLE_INTERVAL) {
        updateI2tValue();
        last_sample_time = current_time;
    }
    
    // Calculate power limit based on I²t protection
    float power_limit = calculatePowerLimit();
    
    // Apply power limiting
    limited_power_percent = min(desired_power_percent, power_limit);
    actual_power_percent = limited_power_percent;
    
    // Update PWM output
    updatePWM();
}

void HeatingController::updateI2tValue() {
    // Calculate I²t contribution for current sample
    // I²t = (current)² × time, but since current is proportional to power:
    // I²t ∝ (power)² × time
    float power_fraction = actual_power_percent / 100.0;
    float i2t_contribution = power_fraction * power_fraction * (I2T_SAMPLE_INTERVAL / 1000.0);
    
    // Add new sample to sliding window
    i2t_samples[i2t_sample_index] = i2t_contribution;
    
    // Calculate total I²t over the window
    current_i2t_value = 0.0;
    for (int i = 0; i < (I2T_WINDOW_SIZE * (1000 / I2T_SAMPLE_INTERVAL)); i++) {
        current_i2t_value += i2t_samples[i];
    }
    
    // Move to next sample position (circular buffer)
    i2t_sample_index = (i2t_sample_index + 1) % (I2T_WINDOW_SIZE * (1000 / I2T_SAMPLE_INTERVAL));
    
    ESP_LOGV(TAG, "I²t value: %.2f, Power: %.1f%%, Sample: %d", 
             current_i2t_value, actual_power_percent, i2t_sample_index);
}

float HeatingController::calculatePowerLimit() {
    float max_i2t = getMaxI2t();
    
    // If we're not close to the limit, allow full power
    if (current_i2t_value < max_i2t * 0.8) {
        return MAX_POWER_PERCENT;
    }
    
    // Calculate how much power we can safely apply
    // Solve: (power/100)² × remaining_time ≤ remaining_i2t_capacity
    float remaining_i2t = max_i2t - current_i2t_value;
    float remaining_time_in_window = I2T_SAMPLE_INTERVAL / 1000.0; // Time for next sample
    
    if (remaining_i2t <= 0) {
        return 0.0; // Must stop
    }
    
    // Calculate maximum safe power for next interval
    float max_safe_power_fraction = sqrt(remaining_i2t / remaining_time_in_window);
    float max_safe_power_percent = max_safe_power_fraction * 100.0;
    
    // Apply gradual reduction when approaching limits
    if (current_i2t_value > max_i2t * 0.9) {
        max_safe_power_percent *= POWER_REDUCTION_FACTOR;
    }
    
    return constrain(max_safe_power_percent, MIN_POWER_PERCENT, MAX_POWER_PERCENT);
}

float HeatingController::getTemperatureDerating() const {
    if (!temperature_valid) {
        return 1.0; // No derating if temperature unknown
    }
    
    if (oven_temperature > TEMP_SAFETY_THRESHOLD) {
        return TEMP_DERATING_FACTOR;
    }
    
    return 1.0; // No derating needed
}

float HeatingController::getMaxI2t() const {
    return MAX_I2T_VALUE * getTemperatureDerating();
}

void HeatingController::updatePWM() {
    uint32_t new_duty_cycle = powerToNearestDutyCycle(actual_power_percent);
    
    if (new_duty_cycle != pwm_duty_cycle) {
        pwm_duty_cycle = new_duty_cycle;
        
        esp_err_t result = ledc_set_duty(LEDC_LOW_SPEED_MODE, 
                                       static_cast<ledc_channel_t>(pwm_channel), 
                                       pwm_duty_cycle);
        if (result == ESP_OK) {
            ledc_update_duty(LEDC_LOW_SPEED_MODE, static_cast<ledc_channel_t>(pwm_channel));
            ESP_LOGD(TAG, "PWM updated: %.1f%% power, duty: %lu", actual_power_percent, pwm_duty_cycle);
        } else {
            ESP_LOGE(TAG, "Failed to set PWM duty cycle: %s", esp_err_to_name(result));
        }
    }
}

uint32_t HeatingController::powerToNearestDutyCycle(float power_percent) {
    // Convert power percentage to duty cycle (0 to 2^PWM_RESOLUTION - 1)
    float power_fraction = constrain(power_percent / 100.0, 0.0, 1.0);
    uint32_t max_duty = (1 << PWM_RESOLUTION) - 1;
    
    // Round to nearest duty cycle value
    uint32_t duty_cycle = round(power_fraction * max_duty);
    
    return duty_cycle;
}

bool HeatingController::isI2tLimiting() const {
    float max_i2t = getMaxI2t();
    return current_i2t_value > (max_i2t * 0.8); // Start indicating limiting at 80%
}

float HeatingController::getI2tUtilization() const {
    float max_i2t = getMaxI2t();
    if (max_i2t <= 0) return 1.0;
    
    return constrain(current_i2t_value / max_i2t, 0.0, 1.0);
}