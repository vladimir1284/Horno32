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

#define DEFAULT_LED_STATE false
#define OFF_STATE "OFF"
#define ON_STATE "ON"

#define LIGHT_SETTINGS_ENDPOINT_PATH "/rest/hornoState"
#define LIGHT_SETTINGS_SOCKET_PATH "/ws/hornoState"
#define LIGHT_SETTINGS_EVENT "led"

class HornoState
{
public:
    bool ledOn;
    float temperature;

    static void read(HornoState &settings, JsonObject &root)
    {
        root["led_on"] = settings.ledOn;
        root["temperature"] = settings.temperature;
    }

    static StateUpdateResult update(JsonObject &root, HornoState &hornoState)
    {
        boolean newState = root["led_on"] | DEFAULT_LED_STATE;
        if (hornoState.ledOn != newState)
        {
            hornoState.ledOn = newState;
            return StateUpdateResult::CHANGED;
        }
        return StateUpdateResult::UNCHANGED;
    }

    static void homeAssistRead(HornoState &settings, JsonObject &root)
    {
        root["state"] = settings.ledOn ? ON_STATE : OFF_STATE;
    }

    static StateUpdateResult homeAssistUpdate(JsonObject &root, HornoState &hornoState)
    {
        String state = root["state"];
        // parse new led state
        boolean newState = false;
        if (state.equals(ON_STATE))
        {
            newState = true;
        }
        else if (!state.equals(OFF_STATE))
        {
            return StateUpdateResult::ERROR;
        }
        // change the new state, if required
        if (hornoState.ledOn != newState)
        {
            hornoState.ledOn = newState;
            return StateUpdateResult::CHANGED;
        }
        return StateUpdateResult::UNCHANGED;
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
