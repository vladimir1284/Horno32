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

HornoMqttSettingsService::HornoMqttSettingsService(PsychicHttpServer *server,
                                                   ESP32SvelteKit *sveltekit) : _httpEndpoint(HornoMqttSettings::read,
                                                                                              HornoMqttSettings::update,
                                                                                              this,
                                                                                              server,
                                                                                              LIGHT_BROKER_SETTINGS_PATH,
                                                                                              sveltekit->getSecurityManager(),
                                                                                              AuthenticationPredicates::IS_AUTHENTICATED),
                                                                                _fsPersistence(HornoMqttSettings::read,
                                                                                               HornoMqttSettings::update,
                                                                                               this,
                                                                                               sveltekit->getFS(),
                                                                                               LIGHT_BROKER_SETTINGS_FILE)
{
    // configure settings service update handler to update LED state
    addUpdateHandler([&](const String &originId)
                     { onConfigUpdated(); },
                     false);
}

void HornoMqttSettingsService::begin()
{
    _httpEndpoint.begin();
    _fsPersistence.readFromFS();
}

void HornoMqttSettingsService::onConfigUpdated()
{
    // Notify the MQTT client about the updated configuration
    // _mqttSettingsService->setStatusTopic(_state.stateTopic);

    // Optionally, you can also log or handle the updated configuration here
    ESP_LOGI(LIGHT_TAG, "MQTT Configuration updated");
}
