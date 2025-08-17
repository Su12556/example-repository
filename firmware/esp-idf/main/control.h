#pragma once
#include <stdbool.h>
#include <stdint.h>

void control_init(void);
void control_update(void); // call at CONTROL_LOOP_HZ via timer