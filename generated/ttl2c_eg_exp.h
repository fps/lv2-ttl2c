#ifndef eg_exp_cb_hh
#define eg_exp_cb_hh

#include <lv2.h>
#include <stdint.h>
 
struct eg_exp_state;

struct eg_exp {
    struct eg_exp_state *state;
    float *ports[3];
};

struct eg_exp_callbacks
{
    struct eg_exp* (*instantiate)(struct eg_exp *instance, double sample_rate, const char *bundle_path, const LV2_Feature *const *features);
    void (*connect_port)(struct eg_exp *instance, uint32_t port, void *data_location);
    void(*activate)(struct eg_exp *instance);
    void(*run)(struct eg_exp *instance, uint32_t sample_count, float *t1, float *in, float *out);
    void(*deactivate)(struct eg_exp *instance);
    void(*cleanup)(struct eg_exp *instance);
    const void *(*extension_data)(const char *uri);
};

#endif    