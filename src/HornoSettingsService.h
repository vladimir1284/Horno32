#ifndef HornoSettingsService_h
#define HornoSettingsService_h

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

#include <HttpEndpoint.h>
#include <FSPersistence.h>
#include <SettingValue.h>
#include <ESP32SvelteKit.h>

#define HORNO_BROKER_SETTINGS_FILE "/config/hornoSettings.json"
#define HORNO_BROKER_SETTINGS_PATH "/rest/hornoSettings"

class HornoSettings
{
public:
    // Maximum duty cycle % for the upper resistance
    int maxPowerUp = 80;

    // Maximum duty cycle % for the lower resistance
    int maxPowerDown = 80;

    // Aggressive PID tuning parameters (fast response)
    double aggKp = 4.0;
    double aggKi = 0.2;
    double aggKd = 1.0;

    // Conservative PID tuning parameters (slow, stable response)
    double consKp = 1.0;
    double consKi = 0.05;
    double consKd = 0.25;

    // Reads the settings from a JsonObject and stores them into the HornoSettings instance
    static void read(HornoSettings &settings, JsonObject &root)
    {
        root["max_power_up"] = settings.maxPowerUp;
        root["max_power_down"] = settings.maxPowerDown;

        root["agg_kp"] = settings.aggKp;
        root["agg_ki"] = settings.aggKi;
        root["agg_kd"] = settings.aggKd;

        root["cons_kp"] = settings.consKp;
        root["cons_ki"] = settings.consKi;
        root["cons_kd"] = settings.consKd;
    }

    // Updates internal state with new values from a JsonObject
    static StateUpdateResult update(JsonObject &root, HornoSettings &settings)
    {
        bool changed = false;

        if (root.containsKey("max_power_up")) {
            settings.maxPowerUp = root["max_power_up"];
            changed = true;
        }

        if (root.containsKey("max_power_down")) {
            settings.maxPowerDown = root["max_power_down"];
            changed = true;
        }

        if (root.containsKey("agg_kp")) {
            settings.aggKp = root["agg_kp"];
            changed = true;
        }

        if (root.containsKey("agg_ki")) {
            settings.aggKi = root["agg_ki"];
            changed = true;
        }

        if (root.containsKey("agg_kd")) {
            settings.aggKd = root["agg_kd"];
            changed = true;
        }

        if (root.containsKey("cons_kp")) {
            settings.consKp = root["cons_kp"];
            changed = true;
        }

        if (root.containsKey("cons_ki")) {
            settings.consKi = root["cons_ki"];
            changed = true;
        }

        if (root.containsKey("cons_kd")) {
            settings.consKd = root["cons_kd"];
            changed = true;
        }

        return changed ? StateUpdateResult::CHANGED : StateUpdateResult::UNCHANGED;
    }
};


class HornoSettingsService : public StatefulService<HornoSettings>
{
public:
    HornoSettingsService(PsychicHttpServer *server, ESP32SvelteKit *sveltekit);
    void begin();
    void onConfigUpdated();

private:
    HttpEndpoint<HornoSettings> _httpEndpoint;
    FSPersistence<HornoSettings> _fsPersistence;
};

#endif // end HornoSettingsService_h
