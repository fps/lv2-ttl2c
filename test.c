/*
    First we include a "preamble" of sorts where a struct type containing callbacks
    is declared
*/
#include "ttl2c_eg_amp.h"

/*
    Then we implement those callbacks that we want. Note that the ports
    have been translated from indices to named arguments:
*/
void run(struct eg_amp *instance, uint32_t nframes, float *gain, float *in, float *out)
{
    for (uint32_t frame = 0; frame < nframes; ++frame)
    {
        out[frame] = gain[0] * in[frame];
    }
}

/*
    And put them in a special variable of the callbacks struct type:
*/
struct eg_amp_callbacks_t eg_amp_callbacks = 
{
    .run = run
};

/*
    And finally we include the rest of the implementation magic
*/
#include "ttl2c_eg_amp.c"

