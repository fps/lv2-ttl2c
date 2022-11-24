#ifndef plugin_cb_hh
#define plugin_cb_hh

#include <lv2.h>
#include <stdint.h>
 
struct plugin_state;

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

     

typedef struct plugin_callbacks
{
    struct plugin* (*instantiate)(plugin_t *instance, double sample_rate, const char *bundle_path, const LV2_Feature *const *features);
    void (*connect_port)(plugin_t *instance, uint32_t port, void *data_location);
    void(*activate)(plugin_t *instance);
    void(*run)(plugin_t *instance, uint32_t sample_count, plugin_port_t1_t t1, plugin_port_in_t in, plugin_port_out_t out);
    void(*deactivate)(plugin_t *instance);
    void(*cleanup)(plugin_t *instance);
    const void *(*extension_data)(const char *uri);
} plugin_callbacks_t;

#endif    