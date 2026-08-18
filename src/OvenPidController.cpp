#include "OvenPidController.h"

OvenPidController::OvenPidController(HornoSettingsService *settingsService)
    : _settingsService(settingsService),
      _integral(0.0),
      _lastInput(0.0),
      _lastTime(0),
      _initialized(false)
{
}

void OvenPidController::reset()
{
    _integral = 0.0;
    _initialized = false;
}

float OvenPidController::compute(float input, float setpoint)
{
    unsigned long now = millis();

    if (!_initialized)
    {
        _lastInput = input;
        _lastTime = now;
        _initialized = true;
        return 0.0;
    }

    float dt = (now - _lastTime) / 1000.0f;
    if (dt <= 0.0f)
    {
        return constrain(_integral, 0.0f, 100.0f);
    }

    double kp, ki, kd;
    _settingsService->read([&](HornoSettings &settings)
                            {
        float gap = fabs(setpoint - input);
        if (gap > PID_GAP_THRESHOLD) {
            kp = settings.aggKp;
            ki = settings.aggKi;
            kd = settings.aggKd;
        } else {
            kp = settings.consKp;
            ki = settings.consKi;
            kd = settings.consKd;
        } });

    float error = setpoint - input;

    _integral += error * ki * dt;
    _integral = constrain(_integral, 0.0f, 100.0f); // anti-windup

    float derivative = -(input - _lastInput) / dt; // derivative-on-measurement, avoids setpoint-kick

    float output = kp * error + _integral + kd * derivative;
    output = constrain(output, 0.0f, 100.0f);

    _lastInput = input;
    _lastTime = now;

    return output;
}
