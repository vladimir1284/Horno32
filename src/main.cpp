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
#include <PsychicHttpServer.h>
#include "HT1621_custom.h" // Include the HT1621 library
#include <max6675.h>
#include <Smoothed.h> 	// Include the library

Smoothed <float> tempSensor; 

int thermoDO = 19;
int thermoCS = 18;
int thermoCLK = 5;

// Temp sensor
MAX6675 thermocouple(thermoCLK, thermoCS, thermoDO);

// Define the HT1621 LCD object
HT1621 lcd;


#define SERIAL_BAUD_RATE 115200

// Demo counters
int count = 0;

// Pin configuration for the LCD
const int csPin = 21;   // Chip Select pin P4.3
const int wrPin = 22;   // Write pin P4.1
const int dataPin = 23;  // Data pin P4.2

unsigned long previousScreenMillis = 0;  // Store the last time the display was updated
#define SCREEN_UPDATE_INTERVAL 500 // Interval in milliseconds 

unsigned long previousSensorMillis = 0;  // Store the last time the display was updated
#define SENSOR_READ_INTERVAL 100 // ms


PsychicHttpServer server;

ESP32SvelteKit esp32sveltekit(&server, 120);


void setup()
{
    // initialize serial
    Serial.begin(SERIAL_BAUD_RATE);

    // Initialize temp sensor filter
    tempSensor.begin(SMOOTHED_AVERAGE, 10);	

    
    // start ESP32-SvelteKit
    esp32sveltekit.begin();

    Serial.println("HT1621 Demo Starting...");

    // Initialize the LCD with the backlight control
    lcd.begin(csPin, wrPin, dataPin);
    lcd.backlight(); // Turn on the backlight

    // Clear the screen
    lcd.clear();

    // Display markers as a part of the startup sequence
    lcd.setMarker(USB);
    delay(500);
    lcd.setMarker(CH51);
    delay(500);
    lcd.setMarker(CH21);
    delay(500);
    lcd.setMarker(STANDBY);
    delay(500);

    Serial.println("Setup complete.");
}


void loop()
{
     unsigned long currentMillis = millis();
    
    // Check if it's time to update the display
    if (currentMillis - previousScreenMillis >= SCREEN_UPDATE_INTERVAL) {
        previousScreenMillis = currentMillis;  // Save the current time
        
        // Display temperature simulation
        float simulatedTemperature = (float)count / 10;
        lcd.printCelsius(tempSensor.get());

    }

    // Check if it's time to update the display
    if (currentMillis - previousSensorMillis >= SENSOR_READ_INTERVAL) {
        previousSensorMillis = currentMillis;  // Save the current time
        
        tempSensor.add(thermocouple.readCelsius());
    }
  
}
