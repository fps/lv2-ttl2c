This repository contains a little python script to make writing an LV2 plugin a little less repetitive/painful.

You write the turle (ttl) files describing the plugins in your bundle and the python script then generates some useful <code>#include</code>s for you. Below you see the code that's necessary to write when running the script on the amp-plugin example from the lv2 distribution (included here for reference and testing in the <code>lv2/</code> directory)

```
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

```
