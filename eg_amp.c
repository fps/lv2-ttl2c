// Include the generated header
#include "generated/ttl2c_eg_amp.h"

// Implement the one callback necessary. Note how there is one type per port.
// (this reduces errors)
static void run(
    plugin_t *instance, uint32_t nframes, 
    plugin_port_gain_t gain, plugin_port_in_t in, plugin_port_out_t out
)
{
    for (uint32_t frame = 0; frame < nframes; ++frame)
    {
        // Each port type has a .data member which hold the connected data
        // location:
        out.data[frame] = gain.data[0] * in.data[frame];
    }
}

// We want run() to be run ;)
const plugin_callbacks_t plugin_callbacks = 
{
    .run = run
};

// Include the generated C file
#include "generated/ttl2c_eg_amp.c"

