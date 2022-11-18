# Pico TinyUSB Keyboard

USB HID Keyboard for Pico RP2040 with TinyUSB.

This is an adapted copy of the hid_composite example from [TinyUSB](https://github.com/hathach/tinyusb/tree/master/examples/device/hid_composite) showing how to build with TinyUSB when using the Raspberry Pi Pico SDK. This repository optimizes the REPORT_ID_KEYBOARD reports.

## Keyboard

In `keyboard.h` a USB HID Keyboard report is created from pin inputs on the Pico. 

Connect pico gpio pins to usb hid keyboard keys:

```c++
const static int num_pins = 11;
const PinKey pin_keys[num_pins] = { // map gpio pin to keycode
    { 2, HID_KEY_W },
    { 3, HID_KEY_A },
    { 4, HID_KEY_S },
    { 5, HID_KEY_D },
    { 16, HID_KEY_ARROW_UP },
    { 17, HID_KEY_ARROW_DOWN },
    { 18, HID_KEY_ARROW_LEFT },
    { 19, HID_KEY_ARROW_RIGHT },
    { 20, HID_KEY_ENTER },
    { 21, HID_KEY_SPACE },
    { 22, HID_KEY_SHIFT_LEFT }
};
```

Make sure `num_pins` is equal to the amount of pin-key combinations specified in `pin_keys`. In this case, there's 11 connected pins. Note that the pico has 26 usable gpio pins.

## Hardware

The input pins use the internal pull up resistor on the pico. They read `1` when **not** pressed. Connect a pin to ground to generate a key press for that pin.

```
            button
              __
    pin -----+  +----- ground
```

The pins are polled once every 10ms.

## install Pico SDK

See [Getting Started with the Raspberry Pi Pico](https://rptl.io/pico-get-started) and the README in the [pico-sdk](https://github.com/raspberrypi/pico-sdk) for information on getting up and running.

If the pico-sdk is in your $PATH, you can keep this repository anywhere. Cmake will probably find it.

## Compiling and uploading

Insert the Pico with the BOOTSEL button pressed. After it's mounted:

```bash
./upload
```

The upload script will compile the code and upload the compiled firmware to the pico.
