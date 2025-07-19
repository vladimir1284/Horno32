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

#include <HornoSettingsService.h>
#include "esp_log.h"

static const char *TAG = "HornoSettings";

HornoSettingsService::HornoSettingsService(PsychicHttpServer *server,
                                           ESP32SvelteKit *sveltekit) : _httpEndpoint(HornoSettings::read,
                                                                                      HornoSettings::update,
                                                                                      this,
                                                                                      server,
                                                                                      HORNO_BROKER_SETTINGS_PATH,
                                                                                      sveltekit->getSecurityManager(),
                                                                                      AuthenticationPredicates::IS_AUTHENTICATED),
                                                                        _fsPersistence(HornoSettings::read,
                                                                                       HornoSettings::update,
                                                                                       this,
                                                                                       sveltekit->getFS(),
                                                                                       HORNO_BROKER_SETTINGS_FILE)
{
    // configure settings service update handler to update LED state
    addUpdateHandler([&](const String &originId)
                     { onConfigUpdated(); },
                     false);
}

void HornoSettingsService::begin()
{
    _httpEndpoint.begin();
    _fsPersistence.readFromFS();
}

void HornoSettingsService::onConfigUpdated()
{
    // Optionally, you can also log or handle the updated configuration here
    ESP_LOGI(TAG, "Horno Configuration updated");
}
