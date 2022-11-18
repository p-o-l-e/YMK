// The MIT License (MIT)

// Copyright (c) 2022, Rotorvator

// Permission is hereby granted, free of charge, to any person obtaining a copy
// of this software and associated documentation files (the "Software"), to deal
// in the Software without restriction, including without limitation the rights
// to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
// copies of the Software, and to permit persons to whom the Software is
// furnished to do so, subject to the following conditions:

// The above copyright notice and this permission notice shall be included in
// all copies or substantial portions of the Software.

// THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
// IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
// FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
// AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
// LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
// OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
// THE SOFTWARE.


#pragma once
#include <stdint.h>
#include "pico/stdlib.h"
#include "hardware/clocks.h"


typedef struct
{
    uint8_t  in;  // GPIO_IN
    uint8_t  out; // GPIO_OUT
    uint64_t threshold;
    bool     current;
    bool     prior;

} sensor;



void _sensors_init(sensor* o)
{
    gpio_init(o->in);
    gpio_init(o->out);
    gpio_set_dir(o->in, GPIO_IN);
    gpio_set_dir(o->out, GPIO_OUT);
    sleep_ms(100);
    o->threshold = 0;
    o->current = false;
    o->prior = false;
    gpio_put(o->out, 0);
    gpio_pull_up(o->in);
    gpio_pull_down(o->out);

    gpio_set_input_hysteresis_enabled(o->in, false);
    gpio_set_slew_rate(o->out, GPIO_SLEW_RATE_FAST);
}

uint64_t _get_cap(sensor* o)
{
    uint64_t start = time_us_64();
    gpio_put(o->out, 1);
    gpio_pull_up(o->in);
    while(gpio_get(o->in) == 0) {};

    gpio_put(o->out, 0);
    gpio_pull_down(o->in);
    return time_us_64() - start;
}


void _calibrate_sensor(sensor* o, double f)
{
    for(int i = 0; i < 50; i++)
    {
        uint64_t cap = (uint64_t)((double)_get_cap(o) * f);
        o->threshold = cap > o->threshold ? cap : o->threshold; 
    }
    sleep_ms(100);
}



bool sense(sensor* o)
{
    o->prior = o->current;
    _get_cap(o) > o->threshold ? o->current = true : o->current = false;
    bool state;
    if (o->current && o->prior)
    {
        gpio_put(PICO_DEFAULT_LED_PIN, 1);
        state = 1;
    }
    else if ((!o->current) && (!o->prior))
    {
        gpio_put(PICO_DEFAULT_LED_PIN, 0);
        state = 0;
    }
    sleep_ms(10);
    return state;
}