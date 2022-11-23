#ifndef plugin_cb_hh
#define plugin_cb_hh

#include <lv2.h>
#include <stdint.h>
 
struct plugin_state;

struct plugin {
    struct plugin_state *state;
    float *ports[3];
};

enum plugin_port_indices {
    t1 = 0,
    in = 1,
    out = 2,
};

struct plugin_port_t1 {
    float *data;
};

struct plugin_port_in {
    float *data;
};

struct plugin_port_out {
    float *data;
};

     

struct plugin_callbacks
{
    struct plugin* (*instantiate)(struct plugin *instance, double sample_rate, const char *bundle_path, const LV2_Feature *const *features);
    void (*connect_port)(struct plugin *instance, uint32_t port, void *data_location);
    void(*activate)(struct plugin *instance);
    void(*run)(struct plugin *instance, uint32_t sample_count, struct plugin_port_t1 t1, struct plugin_port_in in, struct plugin_port_out out);
    void(*deactivate)(struct plugin *instance);
    void(*cleanup)(struct plugin *instance);
    const void *(*extension_data)(const char *uri);
};

#endif    