#include "core/input.h"
#include "DEV_Config.h"
#include "pico/time.h"

#define BUTTON_PRESSED_LEVEL 0

static inline uint32_t now_ms(void) {
  return to_ms_since_boot(get_absolute_time());
}

void input_button_init(InputButton *btn, uint8_t pin) {
  btn->pin = pin;
  DEV_KEY_Config(pin);
  btn->prev_pressed = (DEV_Digital_Read(pin) == BUTTON_PRESSED_LEVEL) ? 1 : 0;
  btn->last_ms = 0;
}

bool input_button_pressed(InputButton *btn, uint32_t debounce_ms) {
  uint8_t cur = (DEV_Digital_Read(btn->pin) == BUTTON_PRESSED_LEVEL) ? 1 : 0;
  bool fired = (btn->prev_pressed == 0 && cur == 1);
  if (fired) {
    uint32_t t = now_ms();
    if (t - btn->last_ms < debounce_ms)
      fired = false;
    else
      btn->last_ms = t;
  }
  btn->prev_pressed = cur;
  return fired;
}
