# What?

This repository contains a little python script to make writing an LV2 plugin a little less repetitive/painful.

You write the turle (ttl) files describing the plugins in your bundle and the python script then generates some useful <code>#include</code>s for you. Below you see the code that's necessary to write when running the script on the amp-plugin example from the lv2 distribution (included here for reference and testing in the <code>lv2/</code> directory)

```C {.line-numbers}
// Include the generated header
#include "ttl2c_eg_amp.h"

// Implement the one callback necessary
void run(struct eg_amp *instance, uint32_t nframes, float *gain, float *in, float *out)
{
    for (uint32_t frame = 0; frame < nframes; ++frame)
    {
        out[frame] = gain[0] * in[frame];
    }
}

// We want run() to be run ;)
struct eg_amp_callbacks_t eg_amp_callbacks = 
{
    .run = run
};

// Include the generated C file
#include "ttl2c_eg_amp.c"
```

# How?

Per plugin a <code>basename</code> is generated by splitting the plugin URI by path separators and just using the last part of the path. Some characters are replaced to make valid C identifiers. In the example <code>http://lv2plug.in/plugins/eg-amp</code> thus becomes <code>eg_amp</code>. The generated source and header file names then get constructed as <code>ttl2c_${basename}.[h|c]</code>.
