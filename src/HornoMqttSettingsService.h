#ifndef HornoMqttSettingsService_h
#define HornoMqttSettingsService_h

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
#include <MqttSettingsService.h>

#define LIGHT_TAG "💡"

#define LIGHT_BROKER_SETTINGS_FILE "/config/brokerSettings.json"
#define LIGHT_BROKER_SETTINGS_PATH "/rest/brokerSettings"

#ifndef FACTORY_MQTT_STATUS_TOPIC
#define FACTORY_MQTT_STATUS_TOPIC "esp32sveltekit/status"
#endif // end FACTORY_MQTT_STATUS_TOPIC

class HornoMqttSettings
{
public:
    String mqttPath;
    String name;
    String uniqueId;
    String stateTopic;

    static void read(HornoMqttSettings &settings, JsonObject &root)
    {
        root["mqtt_path"] = settings.mqttPath;
        root["name"] = settings.name;
        root["unique_id"] = settings.uniqueId;
        root["status_topic"] = settings.stateTopic;
    }

    static StateUpdateResult update(JsonObject &root, HornoMqttSettings &settings)
    {
        settings.mqttPath = root["mqtt_path"] | SettingValue::format("homeassistant/horno/#{unique_id}");
        settings.name = root["name"] | SettingValue::format("horno-#{unique_id}");
        settings.uniqueId = root["unique_id"] | SettingValue::format("horno-#{unique_id}");
        settings.stateTopic = root["status_topic"] | SettingValue::format(FACTORY_MQTT_STATUS_TOPIC);
        return StateUpdateResult::CHANGED;
    }
};

class HornoMqttSettingsService : public StatefulService<HornoMqttSettings>
{
public:
    HornoMqttSettingsService(PsychicHttpServer *server, ESP32SvelteKit *sveltekit);
    void begin();
    void onConfigUpdated();

private:
    HttpEndpoint<HornoMqttSettings> _httpEndpoint;
    FSPersistence<HornoMqttSettings> _fsPersistence;
    // MqttSettingsService *_mqttSettingsService;
};

#endif // end HornoMqttSettingsService_h
