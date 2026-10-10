#include <lvgl.h>
#include <Ticker.h>

#include "Display.h"
#include "Sensor.h"
#include "BLEHandler.h"

int latest_bpm = 0;
float latest_spo2 = 0.0;
float latest_glucose = 0.0;


void setup()
{
    Serial.begin(115200);

    lvgl_setup();

    sensor_setup();

    splash_start_ms = millis();

    Serial.println("LVGL version:");
    Serial.print(LVGL_VERSION_MAJOR);
    Serial.print(".");
    Serial.print(LVGL_VERSION_MINOR);
    Serial.print(".");
    Serial.println(LVGL_VERSION_PATCH);

    Serial.println("BLE configurado com MTU = 200");

    BLE_setup();
}


void loop()
{
    static unsigned long last_lvgl_update = 0;
    static unsigned long last_ble_update = 0;

    unsigned long agora = millis();


    // =================================================
    // LVGL
    // =================================================

    if (agora - last_lvgl_update >= 10)
    {
        last_lvgl_update = agora;

        lv_timer_handler();

        display_loop();
    }


    // =================================================
    // SENSOR
    // =================================================

    sensor_loop();


    // =================================================
    // BLE
    // =================================================

    if (agora - last_ble_update >= 100)
    {
        last_ble_update = agora;

        BLE_send(
            latest_bpm,
            latest_spo2,
            latest_glucose,
            true
        );
    }
}
