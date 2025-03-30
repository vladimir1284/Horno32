#ifndef LightStateService_h
#define LightStateService_h

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

#include <LightMqttSettingsService.h>

#include <EventSocket.h>
#include <HttpEndpoint.h>
#include <MqttEndpoint.h>
#include <EventEndpoint.h>
#include <WebSocketServer.h>

#define DEFAULT_LED_STATE false
#define OFF_STATE "OFF"
#define ON_STATE "ON"

#define LIGHT_SETTINGS_ENDPOINT_PATH "/rest/lightState"
#define LIGHT_SETTINGS_SOCKET_PATH "/ws/lightState"
#define LIGHT_SETTINGS_EVENT "led"

class LightState
{
public:
    bool ledOn;
    int16_t modbusRegs[41 + 11]; // number of modbus registers
    uint16_t workingHours;
    uint16_t temperature;
    uint16_t batteryLevel;
    int16_t  oilPressure;
    uint16_t rpm;
    uint64_t modbusQualityTotalReads;
    uint64_t modbusQualityErrors;
    uint16_t modbusErrors;
    uint16_t modbusFails;
    boolean  modbusState;
    boolean  isWorking;
    boolean  isMainPower;
    boolean  isBatteryOk;
    boolean  isBatteryLow;
    boolean  isBatteryHigh;
    boolean  isHighTemp;
    boolean  isLowOilPress;
    boolean  isOverSpeed;
    boolean  isOverVoltage;
    boolean  cmdStop;
    
    static void read(LightState &settings, JsonObject &root)
    {
        root["led_on"] = settings.ledOn;

        uint16_t PT = settings.modbusRegs[0x2F];
        uint16_t CT = settings.modbusRegs[0x30];
        
        root["VA"] = (settings.modbusRegs[0x00] / 10.0) * PT;
        root["VB"] = (settings.modbusRegs[0x01] / 10.0) * PT;
        root["VC"] = (settings.modbusRegs[0x02] / 10.0) * PT;
        root["IA"] = (settings.modbusRegs[0x03] / 1000.0) * CT;
        root["IB"] = (settings.modbusRegs[0x04] / 1000.0) * CT;
        root["IC"] = (settings.modbusRegs[0x05] / 1000.0) * CT;
        root["Pt"] = settings.modbusRegs[0x07] * CT * PT;
        root["PA"] = settings.modbusRegs[0x08] * CT * PT;
        root["PB"] = settings.modbusRegs[0x09] * CT * PT;
        root["PC"] = settings.modbusRegs[0x0A] * CT * PT;
        root["Qt"] = settings.modbusRegs[0x0B] * CT * PT;
        root["QA"] = settings.modbusRegs[0x0C] * CT * PT;
        root["QB"] = settings.modbusRegs[0x0D] * CT * PT;
        root["QC"] = settings.modbusRegs[0x0E] * CT * PT;
        root["St"] = settings.modbusRegs[0x0F] * CT * PT;
        root["SA"] = settings.modbusRegs[0x10] * CT * PT;
        root["SB"] = settings.modbusRegs[0x11] * CT * PT;
        root["SC"] = settings.modbusRegs[0x12] * CT * PT;
        root["pft"] = settings.modbusRegs[0x13] / 1000.0;
        root["pfA"] = settings.modbusRegs[0x14] / 1000.0;
        root["pfB"] = settings.modbusRegs[0x15] / 1000.0;
        root["pfC"] = settings.modbusRegs[0x16] / 1000.0;
        root["UAB"] = (settings.modbusRegs[0x17] / 10.0) * PT;
        root["UBC"] = (settings.modbusRegs[0x18] / 10.0) * PT;
        root["UCA"] = (settings.modbusRegs[0x19] / 10.0) * PT;
        root["fA"] = settings.modbusRegs[0x1A] / 100.0;
        root["fB"] = settings.modbusRegs[0x1B] / 100.0;
        root["fC"] = settings.modbusRegs[0x1C] / 100.0;
        root["pPenergy"] = settings.modbusRegs[0x1D] * 65536 + settings.modbusRegs[0x1E];
        root["rPenergy"] = settings.modbusRegs[0x1F] * 65536 + settings.modbusRegs[0x20];
        root["pQenergy"] = settings.modbusRegs[0x21] * 65536 + settings.modbusRegs[0x22];
        root["rQenergy"] = settings.modbusRegs[0x23] * 65536 + settings.modbusRegs[0x24];
        root["tPenergy"] = settings.modbusRegs[0x25] * 65536 + settings.modbusRegs[0x26];
        root["tQenergy"] = settings.modbusRegs[0x27] * 65536 + settings.modbusRegs[0x28];

        root["imbalanceVoltage"] = settings.modbusRegs[0x32] / 10.0;
        root["imbalanceCurrent"] = settings.modbusRegs[0x33] / 10.0;

        root["isWorking"] = settings.isWorking;
        root["isMainPower"] = settings.isMainPower;
        // root["isStartFail"] = settings.isStartFail;
        root["isBatteryOk"] = settings.isBatteryOk;
        root["isBatteryLow"] = settings.isBatteryLow;
        root["isBatteryHigh"] = settings.isBatteryHigh;
        root["isHighTemp"] = settings.isHighTemp;
        root["isLowOilPress"] = settings.isLowOilPress;
        root["isOverSpeed"] = settings.isOverSpeed;
        root["isOverVoltage"] = settings.isOverVoltage;
        
        root["workingHours"] = settings.workingHours / 10.0;
        root["temperature"] = settings.temperature / 10.0;
        root["batteryLevel"] = settings.batteryLevel / 10.0;
        root["oilPressure"] = settings.oilPressure / 10.0;
        root["rpm"] = settings.rpm;
        root["cmdStop"] = settings.cmdStop;
                
        root["modbusQualityTotalReads"] = settings.modbusQualityTotalReads;
        root["modbusQualityErrors"] = settings.modbusQualityErrors;
        root["modbusErrors"] = settings.modbusErrors;
        root["modbusFails"] = settings.modbusFails;
        root["modbusState"] = settings.modbusState;

    }

    static StateUpdateResult update(JsonObject &root, LightState &lightState)
    {
        boolean newState = root["cmdStop"] | DEFAULT_LED_STATE;
        if (lightState.cmdStop != newState)
        {
            lightState.cmdStop = newState;
            return StateUpdateResult::CHANGED;
        }
        return StateUpdateResult::UNCHANGED;
    }

    static void homeAssistRead(LightState &settings, JsonObject &root)
    {
        root["state"] = settings.cmdStop ? ON_STATE : OFF_STATE;
    }

    static StateUpdateResult homeAssistUpdate(JsonObject &root, LightState &lightState)
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
        if (lightState.cmdStop != newState)
        {
            lightState.cmdStop = newState;
            return StateUpdateResult::CHANGED;
        }
        return StateUpdateResult::UNCHANGED;
    }
};

class LightStateService : public StatefulService<LightState>
{
public:
    LightStateService(PsychicHttpServer *server,
                      EventSocket *socket,
                      SecurityManager *securityManager,
                      PsychicMqttClient *mqttClient,
                      LightMqttSettingsService *lightMqttSettingsService);

    void begin();

private:
    HttpEndpoint<LightState> _httpEndpoint;
    EventEndpoint<LightState> _eventEndpoint;
    MqttEndpoint<LightState> _mqttEndpoint;
    WebSocketServer<LightState> _webSocketServer;
    PsychicMqttClient *_mqttClient;
    LightMqttSettingsService *_lightMqttSettingsService;

    void registerConfig();
    void onConfigUpdated();
};

#endif
