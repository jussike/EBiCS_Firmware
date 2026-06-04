#include "button_event.h"
#include <stdint.h>

#define DEBOUNCE_TICKS     100
#define LONG_PRESS_TICKS   100000
#define MULTI_CLICK_WINDOW 15000

typedef enum {
    STATE_IDLE,
    STATE_DEBOUNCE_PRESS,
    STATE_PRESSED,
    STATE_DEBOUNCE_RELEASE,
    STATE_WAIT_FOR_NEXT,
    STATE_LONG_PRESSED,
    STATE_DEBOUNCE_LONG_RELEASE
} ButtonState_t;

ButtonEvent_t check_button_events(void) {
    static ButtonState_t state = STATE_IDLE;
    static uint32_t state_timer = 0;
    static uint32_t window_timer = 0;
    static int click_count = 0;

    ButtonEvent_t event_to_return = NO_PRESS;
    bool pin_now = read_pas_gpio_pin();

    if (click_count > 0 && state != STATE_PRESSED && window_timer < MULTI_CLICK_WINDOW) {
        window_timer++;
    }

    switch (state) {
        case STATE_IDLE:
            click_count = 0;
            if (pin_now) {
                state = STATE_DEBOUNCE_PRESS;
                state_timer = 0;
            }
            break;

        case STATE_DEBOUNCE_PRESS:
            if (pin_now) {
                state_timer++;
                if (state_timer >= DEBOUNCE_TICKS) {
                    state = STATE_PRESSED;
                    state_timer = 0;
                }
            } else {
                state = (click_count > 0) ? STATE_WAIT_FOR_NEXT : STATE_IDLE;
            }
            break;

        case STATE_PRESSED:
            if (pin_now) {
                state_timer++;
                if (state_timer > LONG_PRESS_TICKS) {
                    click_count = 0;
                    state = STATE_LONG_PRESSED;
                    state_timer = 0;
                }
            } else {
                state = STATE_DEBOUNCE_RELEASE;
                state_timer = 0;
            }
            break;

        case STATE_DEBOUNCE_RELEASE:
            if (!pin_now) {
                state_timer++;
                if (state_timer >= DEBOUNCE_TICKS) {
                    click_count++;
                    if (click_count >= 3) {
                        event_to_return = TRIPLE_PRESS;
                        state = STATE_IDLE;
                    } else {
                        window_timer = 0;
                        state = STATE_WAIT_FOR_NEXT;
                    }
                }
            } else {
                state = STATE_PRESSED;
            }
            break;

        case STATE_WAIT_FOR_NEXT:
            if (pin_now) {
                state = STATE_DEBOUNCE_PRESS;
                state_timer = 0;
            } else if (window_timer >= MULTI_CLICK_WINDOW) {
                event_to_return = (ButtonEvent_t)click_count;
                state = STATE_IDLE;
            }
            break;

        case STATE_LONG_PRESSED:
            if (!pin_now) {
                state = STATE_DEBOUNCE_LONG_RELEASE;
                state_timer = 0;
            }
            break;

        case STATE_DEBOUNCE_LONG_RELEASE:
            if (!pin_now) {
                state_timer++;
                if (state_timer >= DEBOUNCE_TICKS) {
                    state = STATE_IDLE;
                }
            } else {
                state = STATE_LONG_PRESSED;
            }
            break;
    }

    return event_to_return;
}
