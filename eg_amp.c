// Include the generated header
#include "generated/ttl2c_eg_amp.h"

// Implement the one callback necessary
void run(struct eg_amp *instance, uint32_t nframes, struct eg_amp_port_gain gain, struct eg_amp_port_in in, struct eg_amp_port_out out)
{
    for (uint32_t frame = 0; frame < nframes; ++frame)
    {
        out.data[frame] = gain.data[0] * in.data[frame];
    }
}

// We want run() to be run ;)
struct eg_amp_callbacks eg_amp_callbacks = 
{
    .run = run
};

// Include the generated C file
#include "generated/ttl2c_eg_amp.c"

