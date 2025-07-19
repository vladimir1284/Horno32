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

#include <HornoStateService.h>

HornoStateService::HornoStateService(PsychicHttpServer *server,
                                     ESP32SvelteKit *sveltekit,
                                     HornoMqttSettingsService *hornoMqttSettingsService) : _httpEndpoint(HornoState::read,
                                                                                                         HornoState::update,
                                                                                                         this,
                                                                                                         server,
                                                                                                         LIGHT_SETTINGS_ENDPOINT_PATH,
                                                                                                         sveltekit->getSecurityManager(),
                                                                                                         AuthenticationPredicates::IS_AUTHENTICATED),
                                                                                           _eventEndpoint(HornoState::read,
                                                                                                          HornoState::update,
                                                                                                          this,
                                                                                                          sveltekit->getSocket(),
                                                                                                          LIGHT_SETTINGS_EVENT),
                                                                                        //    _mqttEndpoint(HornoState::homeAssistRead,
                                                                                        //                  HornoState::homeAssistUpdate,
                                                                                        //                  this,
                                                                                        //                  sveltekit->getMqttClient()),
                                                                                           _webSocketServer(HornoState::read,
                                                                                                            HornoState::update,
                                                                                                            this,
                                                                                                            server,
                                                                                                            LIGHT_SETTINGS_SOCKET_PATH,
                                                                                                            sveltekit->getSecurityManager(),
                                                                                                            AuthenticationPredicates::IS_AUTHENTICATED)
                                                                                        //    _mqttClient(sveltekit->getMqttClient()),
                                                                                        //    _hornoMqttSettingsService(hornoMqttSettingsService)
{
    // configure led to be output
    pinMode(LED_BUILTIN, OUTPUT);

    // // configure MQTT callback
    // _mqttClient->onConnect(std::bind(&HornoStateService::registerConfig, this));

    // // configure update handler for when the horno settings change
    // _hornoMqttSettingsService->addUpdateHandler([&](const String &originId)
    //                                             { registerConfig(); },
    //                                             false);

    // configure settings service update handler to update LED state
    addUpdateHandler([&](const String &originId)
                     { onConfigUpdated(); },
                     false);
}

void HornoStateService::begin()
{
    _httpEndpoint.begin();
    _eventEndpoint.begin();
    _state.ledOn = DEFAULT_LED_STATE;
    _state.temperature = 0;
    onConfigUpdated();
}

void HornoStateService::onConfigUpdated()
{
    digitalWrite(LED_BUILTIN, _state.ledOn ? 1 : 0);
}

void HornoStateService::setTemp(float temp)
    {
        _state.temperature = temp;
    }

void HornoStateService::registerConfig()
{
    return;
    // if (!_mqttClient->connected())
    // {
    //     return;
    // }
    // String configTopic;
    // String subTopic;
    // String pubTopic;

    // JsonDocument doc;
    // _hornoMqttSettingsService->read([&](HornoMqttSettings &settings)
    //                                 {
    // configTopic = settings.mqttPath + "/config";
    // subTopic = settings.mqttPath + "/set";
    // pubTopic = settings.mqttPath + "/state";
    // doc["~"] = settings.mqttPath;
    // doc["name"] = settings.name;
    // doc["unique_id"] = settings.uniqueId; });
    // doc["cmd_t"] = "~/set";
    // doc["stat_t"] = "~/state";
    // doc["schema"] = "json";
    // doc["brightness"] = false;

    // String payload;
    // serializeJson(doc, payload);
    // _mqttClient->publish(configTopic.c_str(), 0, false, payload.c_str());

    // _mqttEndpoint.configureTopics(pubTopic, subTopic);
}
