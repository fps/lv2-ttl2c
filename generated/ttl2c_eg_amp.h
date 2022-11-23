#ifndef eg_amp_cb_hh
#define eg_amp_cb_hh

#include <lv2.h>
#include <stdint.h>
 
struct eg_amp_state;

struct eg_amp {
    struct eg_amp_state *state;
    float *ports[3];
};

struct eg_amp_callbacks
{
    struct eg_amp* (*instantiate)(struct eg_amp *instance, double sample_rate, const char *bundle_path, const LV2_Feature *const *features);
    void (*connect_port)(struct eg_amp *instance, uint32_t port, void *data_location);
    void(*activate)(struct eg_amp *instance);
    void(*run)(struct eg_amp *instance, uint32_t sample_count, float *gain, float *in, float *out);
    void(*deactivate)(struct eg_amp *instance);
    void(*cleanup)(struct eg_amp *instance);
    const void *(*extension_data)(const char *uri);
};

#endif    