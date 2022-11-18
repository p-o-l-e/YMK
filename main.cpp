/*
 * The MIT License (MIT)
 *
 * Copyright (c) 2019 Ha Thach (tinyusb.org)
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to deal
 * in the Software without restriction, including without limitation the rights
 * to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 * copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in
 * all copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
 * THE SOFTWARE.
 *
 */

#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#include "bsp/board.h"
#include "tusb.h"

#include "usb_descriptors.h"
#include "sensor.h"
//--------------------------------------------------------------------+
// MACRO CONSTANT TYPEDEF PROTYPES
//--------------------------------------------------------------------+

// Blink pattern
enum
{
    BLINK_NOT_MOUNTED = 250, // device not mounted
    BLINK_MOUNTED = 1000,    // device mounted
    BLINK_SUSPENDED = 2500,  // device is suspended
};

static uint32_t blink_interval_ms = BLINK_NOT_MOUNTED;

void led_blinking_task(void);
void hid_task(void);
static void send_hid_report();

uint8_t keycode[6];

#define SENSORS 3  // Count
#define PIN_OUT 16 // via 1meg resistor connected to touch pad
#define PIN_IN  17
static sensor   _sensor[SENSORS];

/*------------- MAIN -------------*/
int main(void)
{
    board_init();
    tusb_init();

    gpio_init(PICO_DEFAULT_LED_PIN);
    gpio_set_dir(PICO_DEFAULT_LED_PIN, GPIO_OUT);


    _sensor[0].in  = PIN_IN;
    _sensor[0].out = PIN_OUT;

    _sensor[1].in  = 18;
    _sensor[1].out = PIN_OUT;

    _sensor[2].in  = 19;
    _sensor[2].out = PIN_OUT;

    _sensors_init(&_sensor[0]);
    _sensors_init(&_sensor[1]);
    _sensors_init(&_sensor[2]);

    _calibrate_sensor(&_sensor[0], 0.2);
    _calibrate_sensor(&_sensor[1], 1.0);
    _calibrate_sensor(&_sensor[2], 1.0);


    for(int i = 0; i < 10; i++)
    {
        gpio_put(PICO_DEFAULT_LED_PIN, 1);
        sleep_ms(100);
        gpio_put(PICO_DEFAULT_LED_PIN, 0);
        sleep_ms(100);
    }

    while (true)
    {
        bool send = false;
        tud_task(); // tinyusb device task      
        if(sense(&_sensor[0])) 
        {
            keycode[0] = HID_KEY_A;
            send = true;
        }

        if(sense(&_sensor[1])) 
        {
            keycode[1] = HID_KEY_W;
            send = true;
        }

        if(sense(&_sensor[2])) 
        {
            keycode[2] = HID_KEY_D;
            send = true;
        }
        if(send)
        {
            send_hid_report();
            for(int i = 0; i < 6; i++) keycode[i] = 0;
        }
        hid_task(); // keyboard implementation
    }
    return 0;
}

//--------------------------------------------------------------------+
// USB HID
//--------------------------------------------------------------------+

static void send_hid_report()
{
    // skip if hid is not ready yet
    if (!tud_hid_ready())
    {
        return;
    }
    tud_hid_keyboard_report(REPORT_ID_KEYBOARD, 0, keycode);
}

// Every 10ms, we poll the pins and send a report
void hid_task(void)
{
    // Poll every 10ms
    const uint32_t interval_ms = 10;
    static uint32_t start_ms = 0;

    if (board_millis() - start_ms < interval_ms)
    {
        return; // not enough time
    }
    start_ms += interval_ms;

    // Remote wakeup
    if (tud_suspended())
    {
        // Wake up host if we are in suspend mode
        // and REMOTE_WAKEUP feature is enabled by host
        tud_remote_wakeup();
    }
    else
    {
        // send a report
        send_hid_report();
    }
}

// Invoked when sent REPORT successfully to host
// Application can use this to send the next report
// Note: For composite reports, report[0] is report ID
void tud_hid_report_complete_cb(uint8_t instance, uint8_t const *report, uint8_t len)
{
}
// Invoked when received GET_REPORT control request
// Application must fill buffer report's content and return its length.
// Return zero will cause the stack to STALL request
uint16_t tud_hid_get_report_cb(uint8_t instance, uint8_t report_id, hid_report_type_t report_type, uint8_t *buffer, uint16_t reqlen)
{
    return 0;
}
// Invoked when received SET_REPORT control request or
// received data on OUT endpoint ( Report ID = 0, Type = 0 )
void tud_hid_set_report_cb(uint8_t instance, uint8_t report_id, hid_report_type_t report_type, uint8_t const *buffer, uint16_t bufsize)
{
}
