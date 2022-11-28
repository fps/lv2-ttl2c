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

// Implement the one callback necessary. Note how there is one type per port.
// (this reduces errors)
static void run (
    plugin_t *instance, uint32_t nframes, 
    const plugin_port_gain_t gain, 
    const plugin_port_in_t in, 
    const plugin_port_out_t out
) {
    for (uint32_t frame = 0; frame < nframes; ++frame) {
        // Each port type has a .data member which hold the connected data
        // location:
        out.data[frame] = gain.data[0] * in.data[frame];
    }
}

// We want run() to be run ;)
static const plugin_callbacks_t plugin_callbacks = {
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
typedef struct plugin_state {
    float s;
    float sampling_interval;
} plugin_state_t;

// The instantiate callback already gets a plugin_t *instance pointer instead of an LV2_Handle
// and only needs to perform additional initialisation.
static plugin_t* instantiate(plugin_t *instance, double sample_rate, const char *bundle_path, const LV2_Feature *const *features) {
    instance->state = malloc(sizeof(plugin_state_t));
    memset(instance->state, 0, sizeof(plugin_state_t));
    instance->state->sampling_interval = 1.0f / sample_rate;
    return instance;
}

// And similarly the cleanup callback only needs to care about the additional deinitialisation (inverse of instantiate).
static void cleanup(plugin_t *instance) {
    free(instance->state);
}

static void run (
    plugin_t *instance, uint32_t nframes, 
    const plugin_port_t1_t t1, 
    const plugin_port_in_t in, 
    const plugin_port_out_t out
) {
    plugin_state_t *state = instance->state;

    const float a = 1.0f - expf(-state->sampling_interval/t1.data[0]);
    for (uint32_t frame = 0; frame < nframes; ++frame) {
        out.data[frame] = in.data[frame] * a + state->s * (1 - a);
        state->s = in.data[frame];
    }
}

static const plugin_callbacks_t plugin_callbacks = {
    .run = run,
    .instantiate = instantiate,
    .cleanup = cleanup,
};

#include "generated/ttl2c_eg_exp.c"

```

# Makefile

Here is the makefile included with this project used to build and test the generated source:

```make
.PHONY: all doc test

all: plugins 

plugins:
	./lv2-ttl2c -b lv2/example.lv2 -o generated 
	gcc eg_amp.c -pedantic -Wall -Werror -shared -o lv2/example.lv2/amp.so
	gcc eg_exp.c -pedantic -Wall -Werror -shared -o lv2/example.lv2/exp.so

test: plugins
	LV2_PATH=${PWD}/lv2 lv2ls
	LV2_PATH=${PWD}/lv2 lv2info http://lv2plug.in/plugins/eg-amp
	LV2_PATH=${PWD}/lv2 valgrind lv2bench http://lv2plug.in/plugins/eg-amp
	LV2_PATH=${PWD}/lv2 valgrind lv2bench http://lv2plug.in/plugins/eg-exp

doc:
	cat README.md.in | regexec | regexec -e "\[usage\]" -c "./lv2-ttl2c -h" -n 1 > README.md

```

# Generated files

For reference here are the two generated files for the second example:

`ttl2c_eg_exp.h`:

```C
#ifndef plugin_cb_hh
#define plugin_cb_hh

#include <lv2.h>
#include <stdint.h>
 
typedef struct plugin_state plugin_state_t;

typedef struct plugin {
    struct plugin_state *state;
    float *ports[3];
} plugin_t;

enum plugin_port_indices {
    t1 = 0,
    in = 1,
    out = 2,
};

typedef struct plugin_port_t1 {
    float *data;
} plugin_port_t1_t;

typedef struct plugin_port_in {
    float *data;
} plugin_port_in_t;

typedef struct plugin_port_out {
    float *data;
} plugin_port_out_t;

     

typedef struct plugin_callbacks {
    struct plugin* (*const instantiate)(plugin_t *instance, double sample_rate, const char *bundle_path, const LV2_Feature *const *features);
    void (*const connect_port)(plugin_t *instance, uint32_t port, void *data_location);
    void (*const activate)(plugin_t *instance);
    void (*const run)(plugin_t *instance, uint32_t sample_count, const plugin_port_t1_t t1, const plugin_port_in_t in, const plugin_port_out_t out);
    void (*const deactivate)(plugin_t *instance);
    void (*const cleanup)(plugin_t *instance);
    const void *(*const extension_data)(const char *uri);
} plugin_callbacks_t;

#endif    
```

`ttl2c_eg_exp.c`:

```C
#ifndef plugin_hh
#define plugin_hh
    
  
#include <lv2.h>
#include <stdlib.h>
#include <string.h>
    
static void plugin_connect_port_desc(LV2_Handle instance, uint32_t port, void *data_location) {
    if (plugin_callbacks.connect_port) { 
        plugin_callbacks.connect_port((plugin_t *)instance, port, data_location); 
    } else {
        if (port < 3) {
            ((plugin_t*)instance)->ports[port] = (float*)data_location;
        }
    }
}

static LV2_Handle plugin_instantiate_desc(const LV2_Descriptor *descriptor, double sample_rate, const char *bundle_path, const LV2_Feature *const *features) {
    plugin_t *instance = (plugin_t*)malloc(sizeof(struct plugin));
    memset(instance, 0,  sizeof(struct plugin));
    if (plugin_callbacks.instantiate) {
        plugin_callbacks.instantiate(instance, sample_rate, bundle_path, features);
    }
    return (LV2_Handle)(instance);
}

static void plugin_cleanup_desc(LV2_Handle instance) {
    if (plugin_callbacks.cleanup) {
        plugin_callbacks.cleanup((plugin_t*)instance);
    }

    free(instance);
}

static void plugin_activate_desc(LV2_Handle instance) {
    if (plugin_callbacks.activate) {
        plugin_callbacks.activate((plugin_t*)instance);
    }
}

static void plugin_deactivate_desc(LV2_Handle instance) {
    if (plugin_callbacks.deactivate) {
        plugin_callbacks.deactivate((plugin_t*)instance);
    }
}

static void plugin_run_desc(LV2_Handle instance, uint32_t sample_count) {
    if (plugin_callbacks.run) {
        const struct plugin_port_t1 t1 = { .data = ((plugin_t*)instance)->ports[0] };
        const struct plugin_port_in in = { .data = ((plugin_t*)instance)->ports[1] };
        const struct plugin_port_out out = { .data = ((plugin_t*)instance)->ports[2] };

        plugin_callbacks.run((plugin_t*)instance, sample_count, t1, in, out);
    }
}

static const void *plugin_extension_data_desc(const char *uri) {
    if (plugin_callbacks.extension_data) {
        return plugin_callbacks.extension_data(uri);
    } else {
        return 0;
    }
}



static LV2_Descriptor plugin_descriptor = {
    "http://lv2plug.in/plugins/eg-exp",
    plugin_instantiate_desc,
    plugin_connect_port_desc,
    plugin_activate_desc,
    plugin_run_desc,
    plugin_deactivate_desc,
    plugin_cleanup_desc,
    plugin_extension_data_desc
};

LV2_SYMBOL_EXPORT const LV2_Descriptor* lv2_descriptor (uint32_t index) {
    if (0 == index) {
          return &plugin_descriptor;
    } else { 
          return NULL;
    }
}


#endif // plugin_hh    
```


# License

Gnu GPL v3



