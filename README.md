# What?

This repository contains a little python script to make writing an LV2 plugin a little less repetitive/painful.

# Requirements:

- Python 3
- regexec, if you want to rebuild the documentation (README.md): https://github.com/fps/regexec
- valgrind and sord, if you want to run the tests

# Usage

<pre>
usage: lv2-ttl2c [-h] [-b BUNDLE] [-o OUTPUT_DIRECTORY] [-p PREFIX]

Generate useful C code from a LV2 plugin bundle's metadata

optional arguments:
  -h, --help            show this help message and exit
  -b BUNDLE, --bundle BUNDLE
                        the bundle directory to analyze (default: .)
  -o OUTPUT_DIRECTORY, --output-directory OUTPUT_DIRECTORY
                        the output directory (default: .)
  -p PREFIX, --prefix PREFIX
                        the prefix added to output filenames (default: ttl2c_)
</pre>

You write the turle (ttl) files describing the plugins in your bundle and the python script then generates some useful <code>#include</code>s for you. Below you see the code that's necessary to write when running the script on the amp-plugin example from the lv2 distribution (included here for reference and testing in the <code>lv2/</code> directory)

```C
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


```

# How?

Per plugin a <code>basename</code> is generated by splitting the plugin URI by path separators and just using the last part of the path. Some characters are replaced to make valid C identifiers. In the example <code>http://lv2plug.in/plugins/eg-amp</code> thus becomes <code>eg_amp</code>. The generated source and header file names then get constructed as <code>ttl2c_${basename}.[h|c]</code>.

# An example with state

This example can be found in the file `exp.c`.

```C
#include "generated/ttl2c_eg_exp.h"
#include <math.h>
#include <stdlib.h>
#include <string.h>

// This is our state. The name of the type is struct plugin_state
// (the generated files assume this precise name):
typedef struct plugin_state {
    float s;
    float sampling_interval;
} plugin_state_t;

// The instantiate callback already gets a plugin_t *instance pointer
// instead of an LV2_Handle and only needs to perform additional
// initialisation.
static plugin_t* instantiate (
    plugin_t *instance, double sample_rate,
    const char *bundle_path, const LV2_Feature *const *features
) {
    instance->state = malloc(sizeof(plugin_state_t));
    memset(instance->state, 0, sizeof(plugin_state_t));
    instance->state->sampling_interval = 1.0f / sample_rate;
    return instance;
}

// And similarly the cleanup callback only needs to care about
// the additional deinitialisation (inverse of instantiate).
static void cleanup (plugin_t *instance) {
    free(instance->state);
}

static void run (
    plugin_t *instance, uint32_t nframes, 
    const plugin_port_t1_t t1,
    const plugin_port_in_t in,
    const plugin_port_out_t out
) {
    plugin_state_t *state = instance->state;

    const float a = 1.0f - expf(-state->sampling_interval/t1.data);
    for (uint32_t frame = 0; frame < nframes; ++frame) {
        out.data[frame] = in.data[frame] * a + state->s * (1 - a);
        state->s = out.data[frame];
    }
}

static const plugin_callbacks_t plugin_callbacks = {
    .instantiate = instantiate,
    .run = run,
    .cleanup = cleanup,
};

#include "generated/ttl2c_eg_exp.c"


```

# An example processing some MIDI

This example can be found in the file `midigate.c`.

```C
// This example is adapted from the eg-midigate plugin shipped
// with the LV2 distribution
#include "generated/ttl2c_eg_midigate.h"
#include <stdlib.h>
#include <string.h>

typedef struct plugin_state {
    unsigned n_active_notes;
    unsigned program; // 0 = normal, 1 = inverted
} plugin_state_t;

static plugin_t* instantiate (
    plugin_t *instance, double sample_rate,
    const char *bundle_path, const LV2_Feature *const *features
) {
    instance->state = malloc(sizeof(plugin_state_t));
    memset(instance->state, 0, sizeof(plugin_state_t));
    return instance;
}

static void write_output (
    plugin_t* self, uint32_t offset, uint32_t len,
    const plugin_port_in_t in,
    const plugin_port_out_t out
) {
    plugin_state_t *state = self->state;

    const bool active = (state->program == 0) ? (state->n_active_notes > 0)
                                           : (state->n_active_notes == 0);
    if (active) {
        memcpy(out.data + offset, in.data + offset, len * sizeof(float));
    } else {
        memset(out.data + offset, 0, len * sizeof(float));
    }
}

static void run (
    plugin_t *instance, uint32_t nframes, 
    const plugin_port_control_t control,
    const plugin_port_in_t in,
    const plugin_port_out_t out
) {
    plugin_state_t *state = instance->state;
    uint32_t  offset = 0;

    LV2_ATOM_SEQUENCE_FOREACH (control.data, ev) {
        if (ev->body.type == instance->midi_MidiEvent) {
            const uint8_t* const msg = (const uint8_t*)(ev + 1);
            switch (lv2_midi_message_type(msg)) {
            case LV2_MIDI_MSG_NOTE_ON:
                    ++state->n_active_notes;
                break;
            case LV2_MIDI_MSG_NOTE_OFF:
                if (state->n_active_notes > 0) {
                    --state->n_active_notes;
                }
                break;
            case LV2_MIDI_MSG_CONTROLLER:
                if (msg[1] == LV2_MIDI_CTL_ALL_NOTES_OFF) {
                    state->n_active_notes = 0;
                }
                break;
            case LV2_MIDI_MSG_PGM_CHANGE:
                if (msg[1] == 0 || msg[1] == 1) {
                    state->program = msg[1];
                }
                break;
            default:
                break;
            }
        }

        write_output(instance, offset, ev->time.frames - offset, in, out);
        offset = (uint32_t)ev->time.frames;
    }

    write_output(instance, offset, nframes - offset, in, out);
}

static void cleanup(plugin_t *instance) {
    free(instance->state);
}

static const plugin_callbacks_t plugin_callbacks = {
    .instantiate = instantiate,
    .run = run,
    .cleanup = cleanup,
};

#include "generated/ttl2c_eg_midigate.c"


```

# Makefile

Here is the makefile included with this project used to build and test the generated source:

```make
.PHONY: test clean all

# EXTRA_CFLAGS ?= -march=native -mcpu=native -O3 -Wall -Werror -pedantic
EXTRA_CFLAGS ?= -g -O1 -Wall -Werror -pedantic -fPIC

LV2_TTL_PATH ?= /usr/lib/lv2

all: plugins 

PLUGINS = amp exp midigate
PLUGIN_LIBRARIES = $(PLUGINS:%=lv2/example.lv2/%.so)

plugins: $(PLUGIN_LIBRARIES)

lv2/example.lv2/%.so: %.c generated/done makefile
	gcc ${EXTRA_CFLAGS} $< -shared -o $@

generated/done: lv2/example.lv2/*.ttl
	./lv2-ttl2c -b lv2/example.lv2 -o generated 
	touch generated/done

test: plugins
	sord_validate $$(find -L ${LV2_TTL_PATH} -iname "*.ttl") ${PWD}/lv2/example.lv2/*.ttl 2>&1
	LV2_PATH=${PWD}/lv2 lv2ls
	for n in $(PLUGINS); do LV2_PATH=${PWD}/lv2 lv2info http://lv2plug.in/plugins/eg-"$$n"; done
	for n in $(PLUGINS); do LV2_PATH=${PWD}/lv2 valgrind --leak-check=full lv2bench http://lv2plug.in/plugins/eg-"$$n"; done

doc: README.md 

README.md: README.md.in *.c makefile generated/done
	cat README.md.in | regexec | regexec -e "\[usage\]" -c "./lv2-ttl2c -h" -n 1 > README.md

clean:
	rm -f lv2/example.lv2/*.so generated/*.h generated/*.c generated/done lv2/example.lv2/done

```

# License

Gnu GPL v3



