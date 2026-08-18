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

#include <ESP32SvelteKit.h>
#include <HornoMqttSettingsService.h>
#include <HornoSettingsService.h>
#include <HornoStateService.h>
#include <PsychicHttpServer.h>
#include "HT1621_custom.h" // Include the HT1621 library
#include <max6675.h>
#include "median_filter.h"
#include "PinConfig.h"
#include "HeatingController.h"
#include "OvenPidController.h"
#include "Buzzer.h"

MedianFilter tempSensor = MedianFilter();

// Temp sensor
MAX6675 thermocouple(THERMO_CLK_PIN, THERMO_CS_PIN, THERMO_DO_PIN);

// Define the HT1621 LCD object
HT1621 lcd;

#define SERIAL_BAUD_RATE 115200

// Demo counters
int count = 0;

unsigned long previousScreenMillis = 0; // Store the last time the display was updated
#define SCREEN_UPDATE_INTERVAL 500      // Interval in milliseconds

unsigned long previousSensorMillis = 0; // Store the last time the display was updated
#define SENSOR_READ_INTERVAL 100        // ms

PsychicHttpServer server;

ESP32SvelteKit esp32sveltekit(&server, 120);

HornoMqttSettingsService hornoMqttSettingsService = HornoMqttSettingsService(&server,
                                                                             &esp32sveltekit);

HornoSettingsService hornoSettingsService = HornoSettingsService(&server,
                                                                 &esp32sveltekit);

HornoStateService hornoStateService = HornoStateService(&server,
                                                        &esp32sveltekit,
                                                        &hornoMqttSettingsService);

HeatingController heaterTop(HEATER_TOP_PIN, HEATER_TOP_SAFE_MAX_DUTY_PERCENT);
HeatingController heaterBottom(HEATER_BOTTOM_PIN, HEATER_BOTTOM_SAFE_MAX_DUTY_PERCENT);
OvenPidController ovenPid(&hornoSettingsService);
Buzzer buzzer(BUZZER_PIN);

#define CONTROL_LOOP_INTERVAL 250 // ms

void controlHeaters(void *pvParameters)
{
    bool wasOn = false;

    while (true)
    {
        float temperature = tempSensor.getValue();
        bool turnedOn = false;

        hornoStateService.read([&](HornoState &state)
                                {
            float desiredTop = 0;
            float desiredBottom = 0;

            if (state.on)
            {
                turnedOn = !wasOn;

                if (state.mode == "manual")
                {
                    desiredTop = state.manualPowerTop;
                    desiredBottom = state.manualPowerBottom;
                }
                else
                {
                    float output = ovenPid.compute(temperature, state.setpoint);
                    desiredTop = output;
                    desiredBottom = output;
                }
            }
            else
            {
                ovenPid.reset();
            }

            wasOn = state.on;

            hornoSettingsService.read([&](HornoSettings &settings)
                                      {
                heaterTop.setDesiredPower(min(desiredTop, (float)settings.maxPowerUp));
                heaterBottom.setDesiredPower(min(desiredBottom, (float)settings.maxPowerDown)); }); });

        if (turnedOn)
        {
            buzzer.beepShort();
        }

        heaterTop.setOvenTemperature(temperature);
        heaterBottom.setOvenTemperature(temperature);
        heaterTop.update();
        heaterBottom.update();

        hornoStateService.setActualPower(heaterTop.getActualPower(), heaterBottom.getActualPower());

        vTaskDelay(pdMS_TO_TICKS(CONTROL_LOOP_INTERVAL));
    }
}

void updateScreen(void *pvParameters)
{
    while (true)
    {
        lcd.printCelsius(tempSensor.getValue());
        Serial.print(thermocouple.readCelsius());
        Serial.print(", ");
        Serial.println(tempSensor.getValue());
        hornoStateService.setTemp(thermocouple.readCelsius());
        vTaskDelay(pdMS_TO_TICKS(500)); // Wait for 500 ms
    }
}

void readTemp(void *pvParameters)
{
    while (true)
    {
        tempSensor.insertValue(thermocouple.readCelsius());
        vTaskDelay(pdMS_TO_TICKS(250)); // Wait for 500 ms
    }
}
void setup()
{
    // start serial and filesystem
    Serial.begin(SERIAL_BAUD_RATE);

    // start ESP32-SvelteKit
    esp32sveltekit.begin();

    // load the initial horno settings
    hornoStateService.begin();
    // start the horno service
    hornoMqttSettingsService.begin(); 

    // start the horno settings service
    hornoSettingsService.begin(); 
    
    // Display markers as a part of the startup sequence

    // Initialize the LCD with the backhorno control
    lcd.begin(LCD_CS_PIN, LCD_WR_PIN, LCD_DATA_PIN);

    // Clear the screen
    lcd.clear();

    lcd.setMarker(USB);
    delay(500);
    lcd.setMarker(CH51);
    delay(500);
    lcd.setMarker(CH21);
    delay(500);
    lcd.setMarker(STANDBY);
    delay(500);

    heaterTop.begin();
    heaterBottom.begin();
    buzzer.begin();
    hornoStateService.setHardMaxDuty(HEATER_TOP_SAFE_MAX_DUTY_PERCENT, HEATER_BOTTOM_SAFE_MAX_DUTY_PERCENT);

    Serial.println("Setup complete.");

    // ESP_LOGV("LEDTask", "Starting LED blink task");
    // xTaskCreatePinnedToCore(
    //     blinkLED,               // Function that should be called
    //     "LED Blink Task",       // Name of the task (for debugging)
    //     2048,                   // Stack size (bytes)
    //     NULL,                   // Pass no parameters
    //     (tskIDLE_PRIORITY + 10), // Task priority
    //     NULL,                   // Task handle
    //     1                       // Pin to core 1 (or 0 if preferred)
    // );
    xTaskCreatePinnedToCore(
        updateScreen,           // Function that should be called
        "Update Screen",        // Name of the task (for debugging)
        2048,                   // Stack size (bytes)
        NULL,                   // Pass no parameters
        (tskIDLE_PRIORITY + 1), // Task priority
        NULL,                   // Task handle
        1                       // Pin to core 1 (or 0 if preferred)
    );
    xTaskCreatePinnedToCore(
        readTemp,               // Function that should be called
        "Read Temp Sensor",     // Name of the task (for debugging)
        2048,                   // Stack size (bytes)
        NULL,                   // Pass no parameters
        (tskIDLE_PRIORITY + 1), // Task priority
        NULL,                   // Task handle
        1                       // Pin to core 1 (or 0 if preferred)
    );
    xTaskCreatePinnedToCore(
        controlHeaters,         // Function that should be called
        "Control Heaters",      // Name of the task (for debugging)
        4096,                   // Stack size (bytes)
        NULL,                   // Pass no parameters
        (tskIDLE_PRIORITY + 1), // Task priority
        NULL,                   // Task handle
        1                       // Pin to core 1 (or 0 if preferred)
    );
}

void loop()
{
    // Delete Arduino loop task, as it is not needed in this example
    vTaskDelete(NULL);
}
