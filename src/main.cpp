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
// #include <LightMqttSettingsService.h>
// #include <LightStateService.h>
#include <PsychicHttpServer.h>
// #include <ModbusMaster.h>
// #include "MCP23017.h"
// #include <SPI.h>
// #include <Mcp320x.h>
// #include <EEPROM.h>


#define SERIAL_BAUD_RATE 115200

// #define I2C_SDA        21
// #define I2C_SCL        22

// #define SPI_CS    	   5  	    // SPI slave select
// #define ADC_VREF       5125 // 1250     // 1.25V Vref
// #define ADC_CLK        2000000 // 8000000  // SPI clock 8MHz

// boolean modbusSerialEcho = true;
// boolean modbusSimulation = false;
// boolean serialDebug = false;

// uint16_t f_simulated = 0;
// uint16_t v_simulated = 0;
// uint16_t f_simulatedInc = 0;
// uint16_t v_simulatedInc = 0;

// const uint8_t pinWorking         = 27; 
// const uint8_t pinMainPower       = 26; 
// const uint8_t pinStartFail       = 25; 

// const uint8_t pinRpm             = 34; // rpm (analog)
// const uint8_t pinTemperature     = 33; // temperature (analog)
// const uint8_t pinBateryLevel     = 35; // battery level (abalog)
// const uint8_t pinOilPressure     = 36; // Oil presuure (analog)

// uint64_t timeOn                   = 1500;
// uint64_t timeOff                  = 3000;
// uint64_t timeReadVars             = 0;
// boolean  myClock                  = false;
// float    Ts                       = 0.9;

// ModbusMaster node;
// uint8_t  result;
// uint16_t MODBUS_DIR_PIN = 4;
// uint16_t PIN_LEDBUILTIN = 2;
// uint16_t SLAVE_ID = 1;

// boolean  isMainPower;
// boolean  isWorking = false;
// boolean  isWorkingLast = false;
// boolean  isBatteryOk;
// boolean  isBatteryLow;
// boolean  isBatteryHigh;

// boolean  isHighTemp;
// boolean  isLowOilPress;
// boolean  isOverSpeed;
// boolean  isOverVoltage;

// boolean  cmdLineOff;
// boolean  cmdEgoff = false;
// boolean  cmdEgoffLast = false;
// boolean  cmdEgAlm = false;
// boolean  cmdEgWaiting = false;
// long     cmdEgOffTime;

// uint32_t workingHours;
// uint32_t workingHoursSaved;
// long     workingHoursInit;
// uint16_t temperature;
// uint16_t batteryLevel;
// int16_t  oilPressure;
// uint16_t temperatureRead_;
// uint16_t batteryLevelRead;
// int16_t  oilPressureRead;
// uint16_t temperature_k_1;
// uint16_t batteryLevel_k_1;
// int16_t  oilPressure_k_1;
// float    T_temperature        = 4;
// float    T_batteryLevel       = 4;
// float    T_oilPressure        = 15;
// uint16_t rpm;

// uint16_t  temperatureAnalogVal;
// uint16_t  oilPressureAnalogVal;

// uint64_t modbusQualityTotalReads = 0;
// uint64_t modbusQualityErrors = 0;
// uint16_t modbusErrors;
// uint16_t modbusFails;
// uint16_t modbusMaxErrors = 3;
// boolean  modbusState = true;
// const int numDataRegs = 41;
// const int numControRegs = 11;
// int16_t  modbusData[numDataRegs + numControRegs];

// uint8_t  analogChannelToRead = 0;

// uint32_t writesToEeprom;

// MCP23017 mcp1(I2C_SDA,I2C_SCL);    //create mcp instance

// MCP3201 adc(ADC_VREF, SPI_CS);

PsychicHttpServer server;

ESP32SvelteKit esp32sveltekit(&server, 120);

// LightMqttSettingsService lightMqttSettingsService = LightMqttSettingsService(&server,
//                                                                              esp32sveltekit.getFS(),
//                                                                              esp32sveltekit.getSecurityManager());

// LightStateService lightStateService = LightStateService(&server,
//                                                         esp32sveltekit.getSocket(),
//                                                         esp32sveltekit.getSecurityManager(),
//                                                         esp32sveltekit.getMqttClient(),
//                                                         &lightMqttSettingsService);

// void eepromInit() {
//   ///////////////////////////////////////////////////////////////////////
  
// }

// void modbusPreTransmission() {
//   delay(50);
//   digitalWrite(MODBUS_DIR_PIN, HIGH);
// }
// void modbusPostTransmission() {
//   digitalWrite(MODBUS_DIR_PIN, LOW);
//   delay(50);
// }

// void setupModbus() {
//   // RS-485 control pin
//   pinMode(MODBUS_DIR_PIN, OUTPUT);
//   digitalWrite(MODBUS_DIR_PIN, LOW);

//   Serial2.begin(4800);
//   Serial2.setTimeout(200);

//   node.begin(SLAVE_ID, Serial2);
//   node.preTransmission(modbusPreTransmission);
//   node.postTransmission(modbusPostTransmission);

//   eepromInit();
// }

// void simulateFV() {
//   f_simulated = uint16_t(3000 + rand() % 1000);
//   v_simulated = uint16_t(60 + rand() % 350);
//   f_simulatedInc = uint16_t(50 + rand() % 150);
//   v_simulatedInc = uint16_t(15 + rand() % 35);
// }

// void readModbus() {
//   if (!modbusSimulation) {
//     result = node.readHoldingRegisters(0, numDataRegs);
//     modbusQualityTotalReads++;
//     if (result == node.ku8MBSuccess) {
//       if (modbusSerialEcho) Serial.print("Success: ");
//       for (int k = 0; k < numDataRegs; k++) {
//         modbusData[k] = node.getResponseBuffer(k);
//       }
//       result = node.readHoldingRegisters(0x50, numControRegs);
//       if (result == node.ku8MBSuccess) {
//         modbusErrors = 0;
//         modbusState = true;
//         for (int k = 0; k < numControRegs; k++) {
//           modbusData[numDataRegs + k] = node.getResponseBuffer(k);
//         }
//       }
//     }
//   }
//   // result = 55;
//   if (result != node.ku8MBSuccess) {
//     if (modbusSerialEcho) {
//       Serial.print("Failed:");
//       Serial.print(result);
//     }
//     modbusQualityErrors++;
//     modbusErrors++;
//     if (modbusErrors > modbusMaxErrors) {
//       if (modbusState) modbusFails++;
//       modbusState = false;
//       modbusQualityTotalReads--;
//       modbusQualityErrors--;
//       for (int k = 0; k < numDataRegs + numControRegs; k++) {
//         modbusData[k] = 0;
//       }
//     }
//   }
//   if (modbusSimulation) { // || !modbusState) { // generates random values in case of modbusError

//     float runTime = millis() / 1000.0;

//     modbusData[0x00] = uint16_t(1300 + 1000 * sin(0.2 * runTime));   // phase voltages
//     modbusData[0x01] = uint16_t(1000 + rand() % 200);
//     modbusData[0x02] = uint16_t(1200 + rand() % 200);
    
//     float i1 = 16000 + 16000 * cos(0.02 * runTime);
//     i1 =  i1 + ((rand() % 4000) - 2000) * i1 / 30000;
//     modbusData[0x03] = uint16_t(i1); //uint16_t(16000 + 16000 * cos(0.01 * runTime));  // phase currents
//     modbusData[0x04] = uint16_t(00000 + rand() % 4000);
//     modbusData[0x05] = uint16_t(00000 + rand() % 2000);

//     modbusData[0x08] = int16_t(3000 + 3000 * cos(0.35678 * runTime + 8));  // active power
//     modbusData[0x09] = 0;//int16_t(100 + rand() % 300);
//     modbusData[0x0A] = 0;//int16_t(200 + rand() % 300);
//     modbusData[0x07] = int16_t(modbusData[0x08] + modbusData[0x09] + modbusData[0x0A]);
    
//     modbusData[0x0C] = i1;//int16_t(3000 + 6000 * cos(0.5 * runTime + 3)); // reactive powers
//     modbusData[0x0D] = 100;//int16_t(100 - (rand() % 200));
//     modbusData[0x0E] = -100; //int16_t(-100 - (rand() % 100));
//     modbusData[0x0B] = modbusData[0x0C] + modbusData[0x0D] + modbusData[0x0E];

//     modbusData[0x10] = uint16_t(sqrt(pow(modbusData[0x0C],2) + pow(modbusData[0x08],2))); //  aparent power
//     modbusData[0x11] = uint16_t(sqrt(pow(modbusData[0x0D],2) + pow(modbusData[0x09],2)));
//     modbusData[0x12] = uint16_t(sqrt(pow(modbusData[0x0E],2) + pow(modbusData[0x0A],2)));
//     modbusData[0x0F] = uint16_t(sqrt(pow(modbusData[0x07],2) + pow(modbusData[0x0B],2)));

//     modbusData[0x14] = uint16_t(1000 * cos(atan(abs(1.0*modbusData[0x0C])/(1.0*modbusData[0x08]))));  // power factor
//     modbusData[0x15] = uint16_t(1000 * cos(atan(abs(1.0*modbusData[0x0D])/(1.0*modbusData[0x09]))));
//     modbusData[0x16] = uint16_t(1000 * cos(atan(abs(1.0*modbusData[0x0E])/(1.0*modbusData[0x0A]))));
//     modbusData[0x13] = uint16_t(1000 * cos(atan(abs(1.0*modbusData[0x0B])/(1.0*modbusData[0x07]))));
    
//     modbusData[0x17] = uint16_t(1500 + 2000 * sin(0.4 * runTime));   // line voltages
//     modbusData[0x18] = int16_t(-150 + rand() % 300);
//     modbusData[0x19] = uint16_t(2850 + rand() % 300);
    
//     modbusData[0x1A] = uint16_t(3000 + rand() % 6000);  // frequency
//     modbusData[0x1B] = modbusData[0x1A];
//     modbusData[0x1C] = modbusData[0x1A];

//     modbusData[0x1D] = uint16_t(10 + rand() % 10);  // energy
//     modbusData[0x1E] = uint16_t(100 + rand() % 100);
//     modbusData[0x1F] = uint16_t(10 + rand() % 10);
//     modbusData[0x20] = uint16_t(100 + rand() % 100);
//     modbusData[0x21] = uint16_t(10 + rand() % 10);
//     modbusData[0x22] = uint16_t(100 + rand() % 100);
//     modbusData[0x23] = uint16_t(10 + rand() % 10);
//     modbusData[0x24] = uint16_t(100 + rand() % 100);
//     modbusData[0x25] = uint16_t(10 + rand() % 10);
//     modbusData[0x26] = uint16_t(100 + rand() % 100);
//     modbusData[0x27] = uint16_t(10 + rand() % 10);
//     modbusData[0x28] = uint16_t(100 + rand() % 100);

//     modbusData[0x2F] = 1; // PT voltage ratio
//     modbusData[0x30] = 5; //uint16_t(001 + rand() % 5);   // CT current ratio

//     modbusData[0x32] = uint16_t(000 + rand() % 500);    // voltage imbalamce
//     modbusData[0x33] = uint16_t(500 + rand() % 500);    // current imbalamce

//   }

//   // f_simulated += f_simulatedInc;
//   // modbusData[0x1A] = f_simulated;
//   // v_simulated += v_simulatedInc;
//   // modbusData[0x00] = v_simulated;
//   // modbusData[0x2F] = 1; // PT voltage ratio
//   // modbusData[0x30] = 5; //uint16_t(001 + rand() % 5);   // CT current ratio
  

//   if (modbusSerialEcho) {
//     // modbusSerialEcho = false;
//     Serial.print(" - ");
//     for (int k = 0; k < numDataRegs; k++) {
//       Serial.print(modbusData[k]);
//       Serial.print(" ");
//     }
//     Serial.println("");
//     Serial.print("        --> ");
//     for (int k = 0; k < numControRegs; k++) {
//       Serial.print(modbusData[numDataRegs + k]);
//       Serial.print(" ");
//     }
//     Serial.print(" - ");
//     Serial.print("T:");
//     Serial.print(modbusQualityTotalReads);
//     Serial.print(" E:");
//     Serial.print(modbusQualityErrors);
//     Serial.print(" PE:");
//     Serial.print(100 * ((float) modbusQualityErrors / (float) modbusQualityTotalReads));
//     Serial.print(" iE:");
//     Serial.print(modbusErrors);
//     Serial.print(" F:");
//     Serial.print(modbusFails);
//     Serial.print(" S:");
//     Serial.print(modbusState);
//     Serial.println();
//     Serial.print("        ==> ");
//     Serial.print("Temp:");
//     Serial.print(temperature);
//     Serial.print(" batL:");
//     Serial.print(batteryLevel);
//     Serial.print(" oilP:");
//     Serial.print(oilPressure);
//     Serial.print(" oilPval:");
//     Serial.print(oilPressureAnalogVal);
//     Serial.print(" rpm:");
//     Serial.print(rpm);
//     Serial.print(" W:");
//     Serial.print(isWorking);
//     Serial.print(" MP:");
//     Serial.print(isMainPower);
//     Serial.print(" HT:");
//     Serial.print(isHighTemp);
//     Serial.print(" LP:");
//     Serial.print(isLowOilPress);
//     Serial.print(" OS:");
//     Serial.print(isOverSpeed);
//     Serial.print(" OV:");
//     Serial.print(isOverVoltage);
//     Serial.print(" -- wH:");
//     Serial.print(workingHours);
//     Serial.print(" wHsaved:");
//     Serial.print(workingHoursSaved);
//     Serial.print(" wHinit:");
//     Serial.print(workingHoursSaved);
//     Serial.print(" ms:");
//     Serial.print(millis());
//     Serial.println("");
//   }
// }

// void processCmd() {
//   if ((cmdEgoff && !cmdEgoffLast) || cmdEgAlm) { // && !cmdEgAlmLast)) {
//     cmdEgAlm = false;
//     cmdEgWaiting = true;
//     cmdEgOffTime = millis() + 8000;
//     if (serialDebug) Serial.println("contando ===================");
//   }
// }

// void readVars() {
//   if (isWorking) {
//     long workingHourDiff = millis() - workingHoursInit;
//     if (workingHourDiff >= 6 * 60 * 100) {
//       workingHours++;
//       if(workingHours >= 1000000) workingHours = 0;
//       workingHoursInit += 6 * 60 * 100;
//     }
//   }
//   if (workingHours - workingHoursSaved >= 50 && !modbusSimulation) {
//     EEPROM.put(2, workingHours);
//     EEPROM.commit();
//     workingHoursSaved = workingHours;
//     writesToEeprom++;
//     if (serialDebug) Serial.println("salva de workingHours");
//     // modbusSerialEcho = true;
//   }
  
  // uint16_t raw = adc.read(MCP3201::Channel::SINGLE_0);
  // // get analog value
  // uint16_t val = adc.toAnalog(raw);

  // switch (analogChannelToRead) {
  // case 0:
  //   batteryLevelRead = (val + 10.1) / 20.2 + 5;
  //   if (batteryLevelRead < 0) batteryLevelRead = 0;
  //   if (millis() < 15000) batteryLevel_k_1 = batteryLevelRead;
  //   batteryLevel = (1-exp(-Ts / ((float) T_batteryLevel))) * batteryLevelRead + exp(-Ts / ((float) T_batteryLevel)) * batteryLevel_k_1;
  //   batteryLevel_k_1 = batteryLevel;
  //   break;
  // case 1:
  //   if (val > 3100) val = 3100;
  //   temperatureRead_ = -0.4721*val+1470;
  //   if (temperatureRead_ < 300) temperatureRead_ = 300;
  //   temperatureAnalogVal = val;
  //   if (millis() < 15000) temperature_k_1 = temperatureRead_;
  //   temperature = (1-exp(-Ts / ((float) T_temperature))) * temperatureRead_ + exp(-Ts / ((float) T_temperature)) * temperature_k_1;
  //   temperature_k_1 = temperature;
  //   break;
  // case 2:
  //   // oilPressure = -0.03146*val + 105.5;
  //   oilPressureRead = -0.0001532*pow(val,1.628)+87.14+11;
  //   if (oilPressureRead < 0) oilPressureRead = 0;
  //   oilPressureAnalogVal = val;
  //   if (millis() < 15000) oilPressure_k_1 = oilPressureRead;
  //   oilPressure = (1-exp(-Ts / ((float) T_oilPressure))) * oilPressureRead + exp(-Ts / ((float) T_oilPressure)) * oilPressure_k_1;
  //   oilPressure_k_1 = oilPressure;
  //   break;
  // }
  
  // isMainPower = batteryLevel > 60;
  
  // isBatteryOk = (batteryLevel >= 110 && batteryLevel <=155) ? true : false;
  // isBatteryLow = batteryLevel < 110 ? true : false; // 11 => 2290 UC, 660 mV (12V, 764mV)
  // isBatteryHigh = batteryLevel > 155 ? true : false; // 15 => 3200UC
  
  // isLowOilPress = oilPressure < 25 ? true : false;
  
  // rpm = modbusData[0x1A] * 0.3;

  // if (rpm > 1900 && !isOverSpeed) {
  //   cmdEgAlm = true;
  //   isOverSpeed = true;
  //   if (serialDebug) Serial.println("alarmaaaaaaa rpm");
  //   processCmd();
  // }
  // // cmdEgAlmLast = cmdEgAlm;
  // // isOverSpeed = (rpm > 1900 || (rpm < 1400 && rpm != 0)) ? true : false;
  
  // if ((modbusData[0x00] > 1300 || modbusData[0x01] > 1300 || modbusData[0x02] > 1300) && !isOverVoltage) {
  //   cmdEgAlm = true;
  //   isOverVoltage = true;
  //   Serial.println("alarmaaaaaaa Voltaje");
  //   processCmd();
  // }
  // isOverVoltage = (modbusData[0x00] > 1500 || modbusData[0x01] > 1500 || modbusData[0x02] > 1500);

  // if (temperature > 950 && !isHighTemp) {
  //   cmdEgAlm = true;
  //   isHighTemp = true;
  //   if (serialDebug) if (serialDebug) Serial.println("alarmaaaaaaa Temperatura");
  //   processCmd();
  // }
  // // isHighTemp = temperature > 950 ? true : false;


  // isWorking = modbusData[0x1A] > 4500;
  // if (isWorking && !isWorkingLast) {
  //   workingHoursInit = millis();
  //   isOverSpeed = false;
  //   isOverVoltage = false;
  //   isHighTemp = false;
  //   if (serialDebug) Serial.println("ARRANQUE...");
  //   // modbusSerialEcho = true;
  // }
  // if (!isWorking && isWorkingLast && !modbusSimulation) {
  //   // modbusSerialEcho = true;
  //   if (workingHours != workingHoursSaved) {
  //     EEPROM.put(2, workingHours);
  //     EEPROM.commit();
  //     writesToEeprom++;
  //     workingHoursSaved = workingHours;
  //     if (serialDebug) Serial.println("PARADA---");
  //   } else {
  //     if (serialDebug) Serial.println("PARADA--- (sin salvar eeprom) ----");
  //   }
  // }
  // isWorkingLast = isWorking;

//   analogChannelToRead++;
//   if (analogChannelToRead > 2) analogChannelToRead = 0;

//   uint8_t regMCP23017 = 0;
//   regMCP23017 |= isBatteryHigh ? 0x01 : 0x00;
//   regMCP23017 |= isOverVoltage ? 0x02 : 0x00;
//   regMCP23017 |= isOverSpeed   ? 0x04 : 0x00;
//   regMCP23017 |= isLowOilPress ? 0x08 : 0x00;
//   regMCP23017 |= isHighTemp    ? 0x10 : 0x00;
  
//   mcp1.write_gpio(MCP23017_PORTA, regMCP23017 , MCP23017_ADDRESS_20);
  
//   regMCP23017 = 0;
//   regMCP23017 |= analogChannelToRead & 0x03;
//   regMCP23017 |= cmdLineOff    ? 0x04 : 0x00;
//   regMCP23017 |= cmdEgWaiting  ? 0x08 : 0x00;
//   regMCP23017 |= isMainPower   ? 0x10 : 0x00;
//   regMCP23017 |= isWorking     ? 0x20 : 0x00;
//   regMCP23017 |= isBatteryOk   ? 0x40 : 0x00;
//   regMCP23017 |= isBatteryLow  ? 0x80 : 0x00;
  
//   mcp1.write_gpio(MCP23017_PORTB,  regMCP23017 , MCP23017_ADDRESS_20);

// }

// StateUpdateResult updateVars(LightState &state) {
//     state.ledOn = !state.ledOn;
//     for (int i = 0; i < numDataRegs + numControRegs; i++)
//       state.modbusRegs[i] = modbusData[i];

//     state.workingHours = workingHours / 10.0;
//     state.temperature = temperature;
//     state.batteryLevel = batteryLevel;
//     state.oilPressure = oilPressure;
//     state.rpm = rpm;

//     state.isWorking = isWorking;
//     state.isMainPower = isMainPower;
//     state.isBatteryOk = isBatteryOk;
//     state.isBatteryLow = isBatteryLow;
//     state.isBatteryHigh = isBatteryHigh;
//     state.isHighTemp = isHighTemp;
//     state.isLowOilPress = isLowOilPress;
//     state.isOverSpeed = isOverSpeed;
//     state.isOverVoltage = isOverVoltage;
    
//     state.modbusQualityTotalReads = modbusQualityTotalReads;
//     state.modbusQualityErrors = modbusQualityErrors;
//     state.modbusErrors = modbusErrors;
//     state.modbusFails = modbusFails;
//     state.modbusState = modbusState;

//     cmdEgoff = state.cmdStop;

//     processCmd();
    
//     if (cmdEgWaiting) {
//       if (millis() > cmdEgOffTime) {
//         cmdEgWaiting = false;
        
//         simulateFV();

//         if (cmdEgoff) {
//           state.cmdStop = false;
//           if (serialDebug) Serial.println("apagado por hardware............");
//         } else {
//           if (serialDebug) Serial.println("apagado por sotfware >>>>>>>>>>>>>>>>>>>>");
//         }
//         if (cmdEgAlm) {
//           cmdEgAlm = false;
//           if (serialDebug) Serial.println("apagada la Alarma **********");
//         }
//       }
//     }
//     cmdEgoffLast = cmdEgoff;
//     return StateUpdateResult::CHANGED;
// }

void setup()
{
    // delay(1000);
    // pinMode(LED_BUILTIN, OUTPUT);
    // digitalWrite(LED_BUILTIN, HIGH);

    // delay(1000);

    // simulateFV();

    // EEPROM.begin(64);

    // initialize serial
    Serial.begin(115200);

    // // start modbus
    // setupModbus();

    // // configure PIN mode
    // pinMode(SPI_CS, OUTPUT);

    // // set initial PIN state
    // digitalWrite(SPI_CS, HIGH);

    // // initialize SPI interface for MCP3201
    // SPISettings settings(ADC_CLK, MSBFIRST, SPI_MODE0);
    // SPI.begin();
    // SPI.beginTransaction(settings);

    
    // mcp1.iodir(MCP23017_PORTA, MCP23017_IODIR_ALL_OUTPUT , MCP23017_ADDRESS_20);
    // mcp1.iodir(MCP23017_PORTB, MCP23017_IODIR_ALL_OUTPUT , MCP23017_ADDRESS_20);

    // mcp1.write_gpio(MCP23017_PORTB,  0x00 , MCP23017_ADDRESS_20);

    // // Init workingHours
    // // EEPROM.write(0, 0x00);
    // // EEPROM.write(1, 0x00);
    // // EEPROM.put(2, workingHours);
    // // EEPROM.commit();

    // uint8_t eep0 = EEPROM.read(0);
    // uint8_t eep1 = EEPROM.read(1);
    // if (serialDebug) {
    //   Serial.println(eep0, HEX);
    //   Serial.println(eep1, HEX);
    // }
    // if (eep0 == 0x55 && eep1 == 0xAA) {
    //   EEPROM.get(2, workingHours);
    //   // if (serialDebug) Serial.println("-----   eeprom ok ---------");
    // } else {
    //   workingHours = 0;  // init value for workingHours
    //   EEPROM.write(0, 0x55);
    //   EEPROM.write(1, 0xAA);
    //   EEPROM.put(2, workingHours);
    //   EEPROM.commit();
    //   // if (serialDebug) Serial.println("********   EEPROM inicializada *********");
    // }
    // workingHoursSaved = workingHours;
    
    
    // start ESP32-SvelteKit
    esp32sveltekit.begin();

    // // load the initial light settings
    // lightStateService.begin();
    // // start the light service
    // lightMqttSettingsService.begin();

    //     // digitalWrite(LED_BUILTIN, LOW);

}

void loop()
{
    // Delete Arduino loop task, as it is not needed in this example
    // vTaskDelete(NULL);

  uint8_t result;
  ulong runTime = millis();

  // if ((runTime > timeOn) && !myClock) {
  //     // if (serialDebug) Serial.println("time ON ....");   
  //     myClock = true;
  //     readModbus();   
  // }
  // if ((runTime > timeOff) && myClock) {
  //     // if (serialDebug) Serial.println("time off ____");   
  //     timeOn = runTime + 1500;   // time ON
  //     timeOff = runTime + 3000;  // time OFF
  //     myClock = false;
  //     lightStateService.update(updateVars, "modbus");
  // }
  // if (runTime > timeReadVars) {
  //   readVars();
  //   timeReadVars = runTime + 1000 * Ts / 3;
  // }
}
