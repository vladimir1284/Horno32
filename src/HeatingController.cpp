#include "HeatingController.h"

static const int I2T_SAMPLES_COUNT = I2T_WINDOW_SIZE * (1000 / I2T_SAMPLE_INTERVAL);

HeatingController::HeatingController(int pin, float hard_max_duty_percent)
    : pin(pin),
      hard_max_duty_percent(constrain(hard_max_duty_percent, MIN_POWER_PERCENT, MAX_POWER_PERCENT)),
      output_state(false),
      window_start(0),
      last_sample_time(0),
      desired_power_percent(0.0),
      actual_power_percent(0.0),
      i2t_sample_index(0),
      current_i2t_value(0.0),
      oven_temperature(30.0),
      temperature_valid(false)
{
    for (int i = 0; i < I2T_SAMPLES_COUNT; i++)
    {
        i2t_samples[i] = 0.0;
    }
}

bool HeatingController::begin()
{
    pinMode(pin, OUTPUT);
    digitalWrite(pin, LOW);
    output_state = false;
    window_start = millis();
    last_sample_time = window_start;
    return true;
}

void HeatingController::setDesiredPower(float power_percent)
{
    desired_power_percent = constrain(power_percent, MIN_POWER_PERCENT, MAX_POWER_PERCENT);
}

void HeatingController::setOvenTemperature(float temperature_celsius)
{
    oven_temperature = temperature_celsius;
    temperature_valid = true;
}

void HeatingController::update()
{
    unsigned long now = millis();

    if (now - last_sample_time >= I2T_SAMPLE_INTERVAL)
    {
        updateI2tValue();
        last_sample_time = now;
    }

    // desired_power_percent is what the PID/user see as 0-100%; it is
    // scaled here so their 100% lands exactly on hard_max_duty_percent,
    // never clamped flat at the top of the range (that would throw away
    // control resolution across most of the PID's output span).
    float requested_duty = desired_power_percent * (hard_max_duty_percent / 100.0f);
    float power_limit = calculatePowerLimit();
    actual_power_percent = min(requested_duty, power_limit);

    unsigned long elapsed = now - window_start;
    if (elapsed >= PWM_PERIOD_MS)
    {
        window_start = now;
        elapsed = 0;
    }

    unsigned long on_time = (unsigned long)(PWM_PERIOD_MS * (actual_power_percent / 100.0f));
    bool should_be_on = elapsed < on_time;

    if (should_be_on != output_state)
    {
        output_state = should_be_on;
        digitalWrite(pin, output_state ? HIGH : LOW);
    }
}

void HeatingController::updateI2tValue()
{
    float power_fraction = actual_power_percent / 100.0;
    float i2t_contribution = power_fraction * power_fraction * (I2T_SAMPLE_INTERVAL / 1000.0);

    i2t_samples[i2t_sample_index] = i2t_contribution;

    current_i2t_value = 0.0;
    for (int i = 0; i < I2T_SAMPLES_COUNT; i++)
    {
        current_i2t_value += i2t_samples[i];
    }

    i2t_sample_index = (i2t_sample_index + 1) % I2T_SAMPLES_COUNT;
}

float HeatingController::calculatePowerLimit()
{
    float max_i2t = getMaxI2t();

    if (current_i2t_value < max_i2t * 0.8)
    {
        return MAX_POWER_PERCENT;
    }

    float remaining_i2t = max_i2t - current_i2t_value;
    float remaining_time_in_window = I2T_SAMPLE_INTERVAL / 1000.0;

    if (remaining_i2t <= 0)
    {
        return 0.0;
    }

    float max_safe_power_fraction = sqrt(remaining_i2t / remaining_time_in_window);
    float max_safe_power_percent = max_safe_power_fraction * 100.0;

    if (current_i2t_value > max_i2t * 0.9)
    {
        max_safe_power_percent *= POWER_REDUCTION_FACTOR;
    }

    return constrain(max_safe_power_percent, MIN_POWER_PERCENT, MAX_POWER_PERCENT);
}

float HeatingController::getTemperatureDerating() const
{
    if (!temperature_valid)
    {
        return 1.0;
    }
    if (oven_temperature > TEMP_SAFETY_THRESHOLD)
    {
        return TEMP_DERATING_FACTOR;
    }
    return 1.0;
}

float HeatingController::getMaxI2t() const
{
    return MAX_I2T_VALUE * getTemperatureDerating();
}

bool HeatingController::isI2tLimiting() const
{
    return current_i2t_value > (getMaxI2t() * 0.8);
}

float HeatingController::getI2tUtilization() const
{
    float max_i2t = getMaxI2t();
    if (max_i2t <= 0)
        return 1.0;
    return constrain(current_i2t_value / max_i2t, 0.0, 1.0);
}
