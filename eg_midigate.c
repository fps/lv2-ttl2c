#include "generated/ttl2c_eg_midigate.h"

static void run (
    plugin_t *instance, uint32_t nframes, 
    const plugin_port_control_t control,
    const plugin_port_in_t in,
    const plugin_port_out_t out
) {
    for (uint32_t frame = 0; frame < nframes; ++frame) {
        out.data[frame] = in.data[frame];
    }
}

static const plugin_callbacks_t plugin_callbacks = {
    .run = run
};

#include "generated/ttl2c_eg_midigate.c"

