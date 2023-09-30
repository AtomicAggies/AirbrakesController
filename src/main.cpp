/*
 * This file is part of the FreeRTOS port to Teensy boards.
 * Copyright (c) 2020 Timo Sandmann
 *
 * This library is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Lesser General Public
 * License as published by the Free Software Foundation; either
 * version 2.1 of the License, or (at your option) any later version.
 *
 * This library is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public
 * License along with this library. If not, see <http://www.gnu.org/licenses/>.
 */

/**
 * @file    main.cpp
 * @brief   FreeRTOS with SdFat library example for Teensy boards
 * @author  Timo Sandmann
 * @date    11.11.2020
 */

#include "arduino_freertos.h"
#include "avr/pgmspace.h"

#include "SD.h"
#include "semphr.h"

// #include "include/task.h"


static void task1(void*) {
    // ::SemaphoreHandle_t sem = *(::SemaphoreHandle_t*)parameter;

    while (true) {
#ifndef USE_ARDUINO_DEFINES
        arduino::digitalWriteFast(arduino::LED_BUILTIN, arduino::LOW);
#else
        digitalWriteFast(LED_BUILTIN, LOW);
#endif
        ::vTaskDelay(pdMS_TO_TICKS(1'000));

#ifndef USE_ARDUINO_DEFINES
        arduino::digitalWriteFast(arduino::LED_BUILTIN, arduino::HIGH);
#else
        digitalWriteFast(LED_BUILTIN, HIGH);
#endif
        ::vTaskDelay(pdMS_TO_TICKS(1'000));
        
        // Take the Mutex and write to the file
        // File file;

        // ::xSemaphoreTake(sem, portMAX_DELAY);
        // file = SD.open("/test.txt", FILE_WRITE);
        // if (file) {
        //     file.println("Blink Finished");
        // }
        // file.close();
        // ::xSemaphoreGive(sem);
    }
}

static void task2(void*) {
    // ::SemaphoreHandle_t sem = *(::SemaphoreHandle_t*)parameter;

    bool ticktock = 0;

    // ::xSemaphoreTake(sem, portMAX_DELAY);

    if (!SD.begin(BUILTIN_SDCARD)) {
        arduino::Serial.println("initialization failed!");
    } else {
        arduino::Serial.println("initialization done.");
    }

    // ::xSemaphoreGive(sem);

    File file;
    while (true) {
        ticktock = !ticktock;
        
        // Take the Mutex and write to the file
        // ::xSemaphoreTake(sem, portMAX_DELAY);

        file = SD.open("/test.txt", FILE_WRITE);
        if (file) {
            if (ticktock)
                file.println("TICK");
            else
                file.println("TOCK");
            file.close();
        } else {
            arduino::Serial.println("error opening test.txt");
        }

        // Give back the Mutex
        // ::xSemaphoreGive(sem);

        if (ticktock) {
            arduino::Serial.println("TICK");
            ::vTaskDelay(pdMS_TO_TICKS(1'000));
        } else {
            arduino::Serial.println("TOCK");
            ::vTaskDelay(pdMS_TO_TICKS(1'000));
        }
#ifdef SDFAT_BASE
        SD.sdfs.end();
#endif
        arduino::Serial.println("\n");
    }
}

FLASHMEM __attribute__((noinline)) void setup() {
    ::Serial.begin(115'200);
#ifndef USE_ARDUINO_DEFINES
    ::pinMode(arduino::LED_BUILTIN, arduino::OUTPUT);
    ::digitalWriteFast(arduino::LED_BUILTIN, arduino::HIGH);
#else
    ::pinMode(LED_BUILTIN, OUTPUT);
    ::digitalWriteFast(LED_BUILTIN, HIGH);
#endif
    ::delay(5'000);

    ::Serial.println(PSTR("\r\nBooting FreeRTOS kernel " tskKERNEL_VERSION_NUMBER ". Built by gcc " __VERSION__ " (newlib " _NEWLIB_VERSION ") on " __DATE__ ". ***\r\n"));

    // // Create a Mutex to act as a lock for the SD card
    // ::SemaphoreHandle_t sem = ::xSemaphoreCreateMutex();
    // // Verify that it was properly created
    // if (sem == NULL) {
    //     arduino::Serial.println("Unable to Create Semaphore");
    // } else {
    //     ::Serial.println("Counting Semaphore created successfully");
    //     while(1);
    // }

    // Create tasks, and pass the semaphore into them
    ::xTaskCreate(task1, "task1", 128, nullptr, 2, nullptr);
    ::xTaskCreate(task2, "task2", 2048, nullptr, 2, nullptr);

    ::Serial.println("setup(): starting scheduler...");
    ::Serial.flush();

    ::vTaskStartScheduler();
}

void loop() {}
