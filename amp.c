// This example is adapted from the eg-amp plugin shipped
// with the LV2 distribution

// Include the generated header
#include "generated/ttl2c_eg_amp.h"

// Implement the one callback necessary. Note how there is one type per port.
static void run (
    plugin_t *instance, uint32_t nframes, 
    const plugin_port_gain_t gain, 
    const plugin_port_in_t in, 
    const plugin_port_out_t out
) {
    for (uint32_t frame = 0; frame < nframes; ++frame) {
        // Each port type has a .data member which hold the
        // connected data location:
        out.data[frame] = gain.data * in.data[frame];
    }
}

// We want run() to be run ;)
static const plugin_callbacks_t plugin_callbacks = {
    .run = run
};

// Include the generated C file
#include "generated/ttl2c_eg_amp.c"

