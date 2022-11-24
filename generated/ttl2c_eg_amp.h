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
    gain = 0,
    in = 1,
    out = 2,
};

typedef struct plugin_port_gain {
    float *data;
} plugin_port_gain_t;

typedef struct plugin_port_in {
    float *data;
} plugin_port_in_t;

typedef struct plugin_port_out {
    float *data;
} plugin_port_out_t;

     

typedef struct plugin_callbacks
{
    struct plugin* (*const instantiate)(plugin_t *instance, double sample_rate, const char *bundle_path, const LV2_Feature *const *features);
    void (*const connect_port)(plugin_t *instance, uint32_t port, void *data_location);
    void(* const activate)(plugin_t *instance);
    void(* const run)(plugin_t *instance, uint32_t sample_count, const plugin_port_gain_t gain, const plugin_port_in_t in, const plugin_port_out_t out);
    void(*const deactivate)(plugin_t *instance);
    void(*const cleanup)(plugin_t *instance);
    const void *(*const extension_data)(const char *uri);
} plugin_callbacks_t;

#endif    