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
#define PIN_IN  2
static sensor   _sensor[SENSORS];

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



    _sensors_init(&_sensor[0]);
    _sensors_init(&_sensor[1]);
    _sensors_init(&_sensor[2]);
    _sensors_init(&_sensor[3]);
    _sensors_init(&_sensor[4]);
    _sensors_init(&_sensor[5]);
    // _sensors_init(&_sensor[2]);

    _calibrate_sensor(&_sensor[0], 1.0);
    _calibrate_sensor(&_sensor[1], 1.0);
     _calibrate_sensor(&_sensor[2], 1.0);
      _calibrate_sensor(&_sensor[3], 1.0);
       _calibrate_sensor(&_sensor[4], 1.0);
        _calibrate_sensor(&_sensor[5], 1.0);
    // _calibrate_sensor(&_sensor[2], 1.0);


    for(int i = 0; i < 10; i++)
    {
        gpio_put(PICO_DEFAULT_LED_PIN, 1);
        sleep_ms(100);
        gpio_put(PICO_DEFAULT_LED_PIN, 0);
        sleep_ms(100);
    }


    while (1)
    {

        bool send = false;
        if(sense(&_sensor[0])) 
        {
            // keycode[0] = HID_KEY_A;
            send = true;
            sleep_ms(100);
            gpio_put(PICO_DEFAULT_LED_PIN, 0);
            sleep_ms(100);
            gpio_put(PICO_DEFAULT_LED_PIN, 1);
            sleep_ms(100);
            gpio_put(PICO_DEFAULT_LED_PIN, 0);
        }
        if(sense(&_sensor[1])) 
        {
            // keycode[0] = HID_KEY_A;
            send = true;
            sleep_ms(200);
            gpio_put(PICO_DEFAULT_LED_PIN, 0);
            sleep_ms(200);
            gpio_put(PICO_DEFAULT_LED_PIN, 1);
            sleep_ms(200);
            gpio_put(PICO_DEFAULT_LED_PIN, 0);
        }
        if(sense(&_sensor[2])) 
        {
            // keycode[0] = HID_KEY_A;
            send = true;
                        sleep_ms(300);
            gpio_put(PICO_DEFAULT_LED_PIN, 0);
            sleep_ms(300);
            gpio_put(PICO_DEFAULT_LED_PIN, 1);
            sleep_ms(300);
            gpio_put(PICO_DEFAULT_LED_PIN, 0);
        }
        if(sense(&_sensor[3])) 
        {
            // keycode[0] = HID_KEY_A;
            send = true;
                        sleep_ms(400);
            gpio_put(PICO_DEFAULT_LED_PIN, 0);
            sleep_ms(400);
            gpio_put(PICO_DEFAULT_LED_PIN, 1);
            sleep_ms(400);
            gpio_put(PICO_DEFAULT_LED_PIN, 0);
        }
        if(sense(&_sensor[4])) 
        {
            // keycode[0] = HID_KEY_A;
            send = true;
                        sleep_ms(500);
            gpio_put(PICO_DEFAULT_LED_PIN, 0);
            sleep_ms(500);
            gpio_put(PICO_DEFAULT_LED_PIN, 1);
            sleep_ms(500);
            gpio_put(PICO_DEFAULT_LED_PIN, 0);
        }
        if(sense(&_sensor[5])) 
        {
            // keycode[0] = HID_KEY_A;
            send = true;
                        sleep_ms(600);
            gpio_put(PICO_DEFAULT_LED_PIN, 0);
            sleep_ms(600);
            gpio_put(PICO_DEFAULT_LED_PIN, 1);
            sleep_ms(600);
            gpio_put(PICO_DEFAULT_LED_PIN, 0);
        }


        // tud_task(); // tinyusb device task
        // led_blinking_task();
        // midi_task();


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

// Variable that holds the current position in the sequence.
uint32_t note_pos = 0;

// Store example melody as an array of note values
uint8_t note_sequence[] =
{
  74,78,81,86,90,93,98,102,57,61,66,69,73,78,81,85,88,92,97,100,97,92,88,85,81,78,
  74,69,66,62,57,62,66,69,74,78,81,86,90,93,97,102,97,93,90,85,81,78,73,68,64,61,
  56,61,64,68,74,78,81,86,90,93,98,102
};

void midi_task(void)
{
  static uint32_t start_ms = 0;

  uint8_t const cable_num = 0; // MIDI jack associated with USB endpoint
  uint8_t const channel   = 0; // 0 for channel 1

  // send note every 1000 ms
  if (board_millis() - start_ms < 286) return; // not enough time
  start_ms += 286;

  // Previous positions in the note sequence.
  int previous = note_pos - 1;

  // If we currently are at position 0, set the
  // previous position to the last note in the sequence.
  if (previous < 0) previous = sizeof(note_sequence) - 1;

  // Send Note On for current position at full velocity (127) on channel 1.
//   tudi_midi_write24(0, 0x90, note_sequence[note_pos], 127);

  // Send Note Off for previous note.
//   tudi_midi_write24(0, 0x80, note_sequence[previous], 0);
// Send Note On for current position at full velocity (127) on channel 1.
  uint8_t note_on[3] = { 0x90 | channel, note_sequence[note_pos], 127 };
  tud_midi_stream_write(cable_num, note_on, 3);

  // Send Note Off for previous note.
  uint8_t note_off[3] = { 0x80 | channel, note_sequence[previous], 0};
  tud_midi_stream_write(cable_num, note_off, 3);

  // Increment position
  note_pos++;

  // If we are at the end of the sequence, start over.
  if (note_pos >= sizeof(note_sequence)) note_pos = 0;
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