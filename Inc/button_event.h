#include <stdbool.h>

typedef enum {
    NO_PRESS = 0,
    SINGLE_PRESS,
    DOUBLE_PRESS,
    TRIPLE_PRESS
} ButtonEvent_t;

bool read_pas_gpio_pin(void);
ButtonEvent_t check_button_events(void);
