#pragma once
#include "gimbal.h"

// Allow linkage to C code
#ifdef __cplusplus
extern "C" {
#endif

void gui_init();
void gui_update(Gimbal* gimbal, Gimbal* target);
void gui_render();
void gui_shutdown();

#ifdef __cplusplus
}
#endif
