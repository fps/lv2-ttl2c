// Include the generated header
#include "generated/ttl2c_eg_amp.h"

// Implement the one callback necessary
void run(struct eg_amp *instance, uint32_t nframes, float *gain, float *in, float *out)
{
    for (uint32_t frame = 0; frame < nframes; ++frame)
    {
        out[frame] = gain[0] * in[frame];
    }
}

// We want run() to be run ;)
struct eg_amp_callbacks eg_amp_callbacks = 
{
    .run = run
};

// Include the generated C file
#include "generated/ttl2c_eg_amp.c"

