#ifndef OvenPidController_h
#define OvenPidController_h

#include <Arduino.h>
#include <HornoSettingsService.h>

// Classic aggressive/conservative gain-switching PID (Brett Beauregard's
// "Improving the Beginner's PID" adaptive tuning): far from setpoint uses
// aggressive gains for fast approach, close to setpoint switches to
// conservative gains for stability. Matches the agg*/cons* fields already
// present in HornoSettingsService.
#define PID_GAP_THRESHOLD 10.0 // degrees C; not user-configurable yet

class OvenPidController
{
public:
    explicit OvenPidController(HornoSettingsService *settingsService);

    void reset();

    // Call at a fixed-ish interval; returns desired power 0-100%.
    float compute(float input, float setpoint);

private:
    HornoSettingsService *_settingsService;
    float _integral;
    float _lastInput;
    unsigned long _lastTime;
    bool _initialized;
};

#endif
