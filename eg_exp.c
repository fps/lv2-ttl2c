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
static struct plugin* instantiate(struct plugin *instance, double sample_rate, const char *bundle_path, const LV2_Feature *const *features)
{
    instance->state = malloc(sizeof(struct plugin_state));
    memset(instance->state, 0, sizeof(struct plugin_state));
    instance->state->sampling_interval = 1.0f / sample_rate;
    return instance;
}

// And similarly the cleanup callback only needs to care about the additional deinitialisation (inverse of instantiate).
static void cleanup(struct plugin *instance) {
    free(instance->state);
}

static void run(struct plugin *instance, uint32_t nframes, struct plugin_port_t1 t1, struct plugin_port_in in, struct plugin_port_out out)
{
    const float a = 1.0f - expf(-instance->state->sampling_interval/t1.data[0]);
    for (uint32_t frame = 0; frame < nframes; ++frame)
    {
        out.data[frame] = in.data[frame] * a + instance->state->s * (1 - a);
        instance->state->s = in.data[frame];
    }
}

static const struct plugin_callbacks plugin_callbacks = 
{
    .run = run,
    .instantiate = instantiate,
    .cleanup = cleanup,
};

#include "generated/ttl2c_eg_exp.c"

