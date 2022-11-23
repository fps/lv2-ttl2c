#include "ttl2c_eg_exp.h"
#include <math.h>
#include <stdlib.h>
#include <string.h>

struct eg_exp_state 
{
    float s;
    float sampling_interval;
};

struct eg_exp* instantiate(struct eg_exp *instance, double sample_rate, const char *bundle_path, const LV2_Feature *const *features)
{
    instance->state = malloc(sizeof(struct eg_exp_state));
    memset(instance->state, 0, sizeof(struct eg_exp_state));
    instance->state->sampling_interval = 1.0f / sample_rate;
    return instance;
}

void cleanup(struct eg_exp *instance) {
    free(instance->state);
}

void run(struct eg_exp *instance, uint32_t nframes, float *t1, float *in, float *out)
{
    const float a = 1.0f - expf(-instance->state->sampling_interval/t1[0]);
    for (uint32_t frame = 0; frame < nframes; ++frame)
    {
        out[frame] = in[frame] * a + instance->state->s * (1 - a);
        instance->state->s = in[frame];
    }
}

struct eg_exp_callbacks_t eg_exp_callbacks = 
{
    .run = run,
    .instantiate = instantiate,
    .cleanup = cleanup,
};

#include "ttl2c_eg_exp.c"

