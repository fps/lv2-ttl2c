# What?

This repository contains a little python script to make writing an LV2 plugin a little less repetitive/painful.

<pre>
usage: lv2-ttl2c [-h] [-b BUNDLE] [-o OUTPUT_DIRECTORY] [-p PREFIX]

Generate useful C code from a LV2 plugin bundle's metadata

optional arguments:
  -h, --help            show this help message and exit
  -b BUNDLE, --bundle BUNDLE
                        the bundle directory to analyze
  -o OUTPUT_DIRECTORY, --output-directory OUTPUT_DIRECTORY
                        the output directory
  -p PREFIX, --prefix PREFIX
                        the prefix added to output filenames
</pre>

You write the turle (ttl) files describing the plugins in your bundle and the python script then generates some useful <code>#include</code>s for you. Below you see the code that's necessary to write when running the script on the amp-plugin example from the lv2 distribution (included here for reference and testing in the <code>lv2/</code> directory)

```C
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


```

# How?

Per plugin a <code>basename</code> is generated by splitting the plugin URI by path separators and just using the last part of the path. Some characters are replaced to make valid C identifiers. In the example <code>http://lv2plug.in/plugins/eg-amp</code> thus becomes <code>eg_amp</code>. The generated source and header file names then get constructed as <code>ttl2c_${basename}.[h|c]</code>.

# An example with state

```C
#include "generated/ttl2c_eg_exp.h"
#include <math.h>
#include <stdlib.h>
#include <string.h>

// This is our state. The name of the type is struct basename_state (the generated files assume this precise name):
struct plugin_state 
{
    float s;
    float sampling_interval;
};

// The instantiate callback already gets a struct plugin *instance pointer instead of an LV2_Handle
// and only needs to perform additional initialisation.
struct plugin* instantiate(struct plugin *instance, double sample_rate, const char *bundle_path, const LV2_Feature *const *features)
{
    instance->state = malloc(sizeof(struct plugin_state));
    memset(instance->state, 0, sizeof(struct plugin_state));
    instance->state->sampling_interval = 1.0f / sample_rate;
    return instance;
}

// And similarly the cleanup callback only needs to care about the additional deinitialisation (inverse of instantiate).
void cleanup(struct plugin *instance) {
    free(instance->state);
}

void run(struct plugin *instance, uint32_t nframes, struct plugin_port_t1 t1, struct plugin_port_in in, struct plugin_port_out out)
{
    const float a = 1.0f - expf(-instance->state->sampling_interval/t1.data[0]);
    for (uint32_t frame = 0; frame < nframes; ++frame)
    {
        out.data[frame] = in.data[frame] * a + instance->state->s * (1 - a);
        instance->state->s = in.data[frame];
    }
}

const struct plugin_callbacks plugin_callbacks = 
{
    .run = run,
    .instantiate = instantiate,
    .cleanup = cleanup,
};

#include "generated/ttl2c_eg_exp.c"

```

# Makefile

Here is the makefile included with this project used to build and test the generated source:

```make
.PHONY: all

all: plugins test doc

plugins:
	./lv2-ttl2c -b lv2/example.lv2/manifest.ttl -o generated 
	gcc eg_amp.c -pedantic -Wall -Werror -shared -o lv2/example.lv2/amp.so
	gcc eg_exp.c -pedantic -Wall -Werror -shared -o lv2/example.lv2/exp.so

test:
	LV2_PATH=${PWD}/lv2 lv2ls
	LV2_PATH=${PWD}/lv2 lv2info http://lv2plug.in/plugins/eg-amp
	LV2_PATH=${PWD}/lv2 valgrind lv2bench http://lv2plug.in/plugins/eg-amp
	LV2_PATH=${PWD}/lv2 valgrind lv2bench http://lv2plug.in/plugins/eg-exp

doc:
	cat README.md.in | ./regexec | ./regexec -e "\[usage\]" -c "./lv2-ttl2c -h" -n 1 > README.md

```


