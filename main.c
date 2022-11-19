#include <stdio.h>
#include "pico/stdlib.h"
#include "hardware/gpio.h"
#include "pico/binary_info.h"

#include "bsp/board.h"
#include "tusb.h"

#include "sensor.h"

enum  {
    BLINK_NOT_MOUNTED = 250,
    BLINK_MOUNTED = 1000,
    BLINK_SUSPENDED = 2500,
};

static uint32_t blink_interval_ms = BLINK_NOT_MOUNTED;
const uint LED_PIN = PICO_DEFAULT_LED_PIN;

void led_blinking_task(void);
void midi_task(void);

#define SENSORS 6  // Count
static sensor   _sensor[SENSORS];

typedef struct
{
    bool on;
    bool gate;
    bool off;

} note;

note _note[SENSORS];

uint8_t note_sequence[] =
{
    74,78,81,86,90,93
};
// const double sensivity[SENSORS] = { 0.99, 0.99, 0.99, 0.99, 0.99, 0.99 };
const double sensivity[SENSORS] = { 1.1, 1.1, 1.1, 1.1, 1.1, 1.1 };

int main() 
{
    board_init();
    tusb_init();

    _sensor[0].in  = 10;
    _sensor[1].in  = 11;
    _sensor[2].in  = 12;
    _sensor[3].in  = 13;
    _sensor[4].in  = 14;
    _sensor[5].in  = 15;
    for(int i = 0; i < SENSORS; i++)
    {
        _sensors_init(&_sensor[i]);
        sleep_ms(20);
        _calibrate_sensor(&_sensor[i], sensivity[i]);
        sleep_ms(20);
        gpio_put(PICO_DEFAULT_LED_PIN, 1);
        sleep_ms(100);
        gpio_put(PICO_DEFAULT_LED_PIN, 0);
        sleep_ms(100);

    }


    while (1)
    {
        tud_task(); // tinyusb device task
        for(int i = 0; i < SENSORS; i++)
        {
            if(sense(&_sensor[i], 10)) 
            {
                if(!_note[i].off)
                {
                    if(_note[i].gate) _note[i].on = false;
                    else
                    {
                        _note[i].on = true;
                        _note[i].gate = true;
                    }
                }
            }
            else 
            {
                if(_note[i].gate)
                {
                    _note[i].gate = false;
                    _note[i].on = false;
                    _note[i].off = true;
                }
                
            }
        }
        
        
        // led_blinking_task();
        midi_task();
    }
}

//--------------------------------------------------------------------+
// Device callbacks
//--------------------------------------------------------------------+

// Invoked when device is mounted
void tud_mount_cb(void)
{
  blink_interval_ms = BLINK_MOUNTED;
}

// Invoked when device is unmounted
void tud_umount_cb(void)
{
  blink_interval_ms = BLINK_NOT_MOUNTED;
}

// Invoked when usb bus is suspended
// remote_wakeup_en : if host allow us  to perform remote wakeup
// Within 7ms, device must draw an average of current less than 2.5 mA from bus
void tud_suspend_cb(bool remote_wakeup_en)
{
  (void) remote_wakeup_en;
  blink_interval_ms = BLINK_SUSPENDED;
}

// Invoked when usb bus is resumed
void tud_resume_cb(void)
{
  blink_interval_ms = BLINK_MOUNTED;
}

//--------------------------------------------------------------------+
// MIDI Task
//--------------------------------------------------------------------+


void midi_task(void)
{
    uint8_t const cable_num = 0; // MIDI jack associated with USB endpoint
    uint8_t const channel   = 0; // 0 for channel 1

    // Send Note On for current position at full velocity (127) on channel 1.
    for(int i = 0; i < SENSORS; i++)
    {
        if(_note[i].on)
        {
            uint8_t note_on[3] = { 0x90 | channel, note_sequence[i], 127 };
            tud_midi_stream_write(cable_num, note_on, 3);
            _note[i].gate = true;
            _note[i].on = false;
        }
    }
    // Send Note Off for previous note.
    for(int i = 0; i < SENSORS; i++)
    {
        if(_note[i].off)
        {
            uint8_t note_off[3] = { 0x80 | channel, note_sequence[i], 0};
            tud_midi_stream_write(cable_num, note_off, 3);
            _note[i].off = false;
        }
    }
}

//--------------------------------------------------------------------+
// BLINKING TASK
//--------------------------------------------------------------------+
void led_blinking_task(void)
{
  static uint32_t start_ms = 0;
  static bool led_state = false;

  // Blink every interval ms
  if ( board_millis() - start_ms < blink_interval_ms) return; // not enough time
  start_ms += blink_interval_ms;

  board_led_write(led_state);
  led_state = 1 - led_state; // toggle
}