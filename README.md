```
#include <lv2.h>
#include <stdio.h>
#include <stdint.h>

#include "lv2plug_in_plugins_eg_amp_callbacks.h"

void run(LV2_Handle instance, uint32_t nframes, float *gain, float *in, float *out)
{
    printf("hello from run()\n");
}

struct lv2plug_in_plugins_eg_amp_callbacks_t lv2plug_in_plugins_eg_amp_callbacks =
{
    NULL,
    NULL,
    NULL,
    run,
    NULL,
    NULL,
    NULL,
};

#include "lv2plug_in_plugins_eg_amp.h"

```
