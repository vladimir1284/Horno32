#ifndef HornoStateService_h
#define HornoStateService_h

/**
 *   ESP32 SvelteKit
 *
 *   A simple, secure and extensible framework for IoT projects for ESP32 platforms
 *   with responsive Sveltekit front-end built with TailwindCSS and DaisyUI.
 *   https://github.com/theelims/ESP32-sveltekit
 *
 *   Copyright (C) 2018 - 2023 rjwats
 *   Copyright (C) 2023 - 2024 theelims
 *
 *   All Rights Reserved. This software may be modified and distributed under
 *   the terms of the LGPL v3 license. See the LICENSE file for details.
 **/

#include <HornoMqttSettingsService.h>

#include <EventSocket.h>
#include <HttpEndpoint.h>
#include <MqttEndpoint.h>
#include <EventEndpoint.h>
#include <WebSocketServer.h>
#include <ESP32SvelteKit.h>

#define DEFAULT_ON_STATE false
#define DEFAULT_MODE "auto"
#define OFF_STATE "OFF"
#define ON_STATE "ON"

#define LIGHT_SETTINGS_ENDPOINT_PATH "/rest/hornoState"
#define LIGHT_SETTINGS_SOCKET_PATH "/ws/hornoState"
#define LIGHT_SETTINGS_EVENT "horno"

class HornoState
{
public:
    bool on;
    String mode; // "auto" (PID sobre setpoint) o "manual" (potencias fijas)
    float setpoint;
    float manualPowerTop;
    float manualPowerBottom;
    float temperature;
    float actualPowerTop;
    float actualPowerBottom;

    // Read-only info, not settable through update(): the hard safety
    // ceiling each heater is built with (see PinConfig.h). Shown to the
    // user so "why does actual power never pass X%" is never a mystery.
    float hardMaxDutyTop;
    float hardMaxDutyBottom;

    static void read(HornoState &settings, JsonObject &root)
    {
        root["on"] = settings.on;
        root["mode"] = settings.mode;
        root["setpoint"] = settings.setpoint;
        root["manual_power_top"] = settings.manualPowerTop;
        root["manual_power_bottom"] = settings.manualPowerBottom;
        root["temperature"] = settings.temperature;
        root["actual_power_top"] = settings.actualPowerTop;
        root["actual_power_bottom"] = settings.actualPowerBottom;
        root["hard_max_duty_top"] = settings.hardMaxDutyTop;
        root["hard_max_duty_bottom"] = settings.hardMaxDutyBottom;
    }

    static StateUpdateResult update(JsonObject &root, HornoState &hornoState)
    {
        bool changed = false;

        if (root.containsKey("on"))
        {
            bool newOn = root["on"];
            changed |= hornoState.on != newOn;
            hornoState.on = newOn;
        }

        if (root.containsKey("mode"))
        {
            String newMode = root["mode"].as<String>();
            changed |= hornoState.mode != newMode;
            hornoState.mode = newMode;
        }

        if (root.containsKey("setpoint"))
        {
            float newSetpoint = root["setpoint"];
            changed |= hornoState.setpoint != newSetpoint;
            hornoState.setpoint = newSetpoint;
        }

        if (root.containsKey("manual_power_top"))
        {
            float newPower = root["manual_power_top"];
            changed |= hornoState.manualPowerTop != newPower;
            hornoState.manualPowerTop = newPower;
        }

        if (root.containsKey("manual_power_bottom"))
        {
            float newPower = root["manual_power_bottom"];
            changed |= hornoState.manualPowerBottom != newPower;
            hornoState.manualPowerBottom = newPower;
        }

        return changed ? StateUpdateResult::CHANGED : StateUpdateResult::UNCHANGED;
    }

    static void homeAssistRead(HornoState &settings, JsonObject &root)
    {
        root["state"] = settings.on ? ON_STATE : OFF_STATE;
        root["current_temperature"] = settings.temperature;
        root["target_temperature"] = settings.setpoint;
    }

    static StateUpdateResult homeAssistUpdate(JsonObject &root, HornoState &hornoState)
    {
        bool changed = false;

        if (root.containsKey("state"))
        {
            String state = root["state"];
            bool newOn = false;
            if (state.equals(ON_STATE))
            {
                newOn = true;
            }
            else if (!state.equals(OFF_STATE))
            {
                return StateUpdateResult::ERROR;
            }
            changed |= hornoState.on != newOn;
            hornoState.on = newOn;
        }

        if (root.containsKey("target_temperature"))
        {
            float newSetpoint = root["target_temperature"];
            changed |= hornoState.setpoint != newSetpoint;
            hornoState.setpoint = newSetpoint;
        }

        return changed ? StateUpdateResult::CHANGED : StateUpdateResult::UNCHANGED;
    }
};

class HornoStateService : public StatefulService<HornoState>
{
public:
    HornoStateService(PsychicHttpServer *server,
                      ESP32SvelteKit *sveltekit,
                      HornoMqttSettingsService *hornoMqttSettingsService);

    void begin();

    void setTemp(float temp);
    void setActualPower(float top, float bottom);
    void setHardMaxDuty(float top, float bottom);

private:
    HttpEndpoint<HornoState> _httpEndpoint;
    EventEndpoint<HornoState> _eventEndpoint;
    // MqttEndpoint<HornoState> _mqttEndpoint;
    WebSocketServer<HornoState> _webSocketServer;
    // PsychicMqttClient *_mqttClient;
    // HornoMqttSettingsService *_hornoMqttSettingsService;

    void registerConfig();
    void onConfigUpdated();
};

#endif
