#ifndef HORNO_STATE_H
#define HORNO_STATE_H

#include <ESP32SvelteKit.h>
#include <HttpEndpoint.h>
#include <PsychicHttpServer.h>

#define HORNO_SETTINGS_ENDPOINT_PATH "/rest/hornoState"
#define HORNO_SETTINGS_SOCKET_PATH "/ws/hornoState"

#define DEFAULT_LED_STATE false
#define OFF_STATE "OFF"
#define ON_STATE "ON"

// Note that the built-in LED is on when the pin is low on most NodeMCU boards.
// This is because the anode is tied to VCC and the cathode to the GPIO 4 (Arduino pin 2).
#define LED_ON 0x1
#define LED_OFF 0x0
#define LED_PIN 2

class HornoState
{
public:
   bool on = false;
   float setpoint = 0;
   float temperature = 0;

   static void read(HornoState &settings, JsonObject &root)
   {
      root["on"] = settings.on;
      root["setpoint"] = settings.setpoint;
      root["temperature"] = settings.temperature;
   }

   static StateUpdateResult update(JsonObject &root, HornoState &hornoState)
   {
      bool hasChanged = false;

      // Update 'on' state
      if (root.containsKey("on"))
      {
         boolean newOn = root["on"];
         if (hornoState.on != newOn)
         {
            hornoState.on = newOn;
            hasChanged = true;
         }
      }

      // Update setpoint
      if (root.containsKey("setpoint"))
      {
         float newSetpoint = root["setpoint"];
         if (hornoState.setpoint != newSetpoint)
         {
            hornoState.setpoint = newSetpoint;
            hasChanged = true;
         }
      }

      // Update temperature (usually read-only, but included for completeness)
      if (root.containsKey("temperature"))
      {
         float newTemperature = root["temperature"];
         if (hornoState.temperature != newTemperature)
         {
            hornoState.temperature = newTemperature;
            hasChanged = true;
         }
      }

      return hasChanged ? StateUpdateResult::CHANGED : StateUpdateResult::UNCHANGED;
   }

   static void haRead(HornoState &settings, JsonObject &root)
   {
      root["state"] = settings.on ? "ON" : "OFF";
      root["current_temperature"] = settings.temperature;
      root["target_temperature"] = settings.setpoint;
   }

   static StateUpdateResult haUpdate(JsonObject &root, HornoState &hornoState)
   {
      bool hasChanged = false;

      // Handle state (on/off)
      if (root.containsKey("state"))
      {
         String state = root["state"];
         boolean newOn = false;
         if (state.equals("ON"))
         {
            newOn = true;
         }
         else if (!state.equals("OFF"))
         {
            return StateUpdateResult::ERROR;
         }

         if (hornoState.on != newOn)
         {
            hornoState.on = newOn;
            hasChanged = true;
         }
      }

      // Handle target temperature (setpoint)
      if (root.containsKey("target_temperature"))
      {
         float newSetpoint = root["target_temperature"];
         if (hornoState.setpoint != newSetpoint)
         {
            hornoState.setpoint = newSetpoint;
            hasChanged = true;
         }
      }

      // Handle current temperature (usually sensor input, but included for completeness)
      if (root.containsKey("current_temperature"))
      {
         float newTemperature = root["current_temperature"];
         if (hornoState.temperature != newTemperature)
         {
            hornoState.temperature = newTemperature;
            hasChanged = true;
         }
      }

      return hasChanged ? StateUpdateResult::CHANGED : StateUpdateResult::UNCHANGED;
   }
};

class HornoStateService : public StatefulService<HornoState>
{
public:
   HornoStateService(PsychicHttpServer *server,
                     ESP32SvelteKit *sveltekit);
   void begin();

   void updateTemp(float newTemperature);

private:
   HttpEndpoint<HornoState> _httpEndpoint;
   void onConfigUpdated();
};

#endif // TEMPERATURE_STATE_H
