#include "generated/ttl2c_eg_exp.h"
#include <math.h>
#include <stdlib.h>
#include <string.h>

// This is our state. The name of the type is struct basename_state
// (the generated files assume this precise name):
typedef struct plugin_state {
    float s;
    float sampling_interval;
} plugin_state_t;

// The instantiate callback already gets a plugin_t *instance pointer
// instead of an LV2_Handle and only needs to perform additional
// initialisation.
static plugin_t* instantiate(
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

    const float a = 1.0f - expf(-state->sampling_interval/t1.data);
    for (uint32_t frame = 0; frame < nframes; ++frame) {
        out.data[frame] = in.data[frame] * a + state->s * (1 - a);
        state->s = in.data[frame];
    }
}

static const plugin_callbacks_t plugin_callbacks = {
    .instantiate = instantiate,
    .run = run,
    .cleanup = cleanup,
};

#include "generated/ttl2c_eg_exp.c"

