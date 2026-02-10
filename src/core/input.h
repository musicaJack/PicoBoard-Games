#ifndef PICO_CODE_CORE_INPUT_H
#define PICO_CODE_CORE_INPUT_H

#include <stdbool.h>
#include <stdint.h>

typedef struct {
  uint8_t pin;
  uint8_t prev_pressed;
  uint32_t last_ms;
} InputButton;

void input_button_init(InputButton *btn, uint8_t pin);
bool input_button_pressed(InputButton *btn, uint32_t debounce_ms);

#endif
