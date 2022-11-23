// Include the generated header
#include "generated/ttl2c_eg_amp.h"

// Implement the one callback necessary
void run(struct plugin *instance, uint32_t nframes, struct plugin_port_gain gain, struct plugin_port_in in, struct plugin_port_out out)
{
    for (uint32_t frame = 0; frame < nframes; ++frame)
    {
        out.data[frame] = gain.data[0] * in.data[frame];
    }
}

// We want run() to be run ;)
struct plugin_callbacks plugin_callbacks = 
{
    .run = run
};

// Include the generated C file
#include "generated/ttl2c_eg_amp.c"

