```
#ifndef fps_io_plugins_relative_dynamics_hh
#define fps_io_plugins_relative_dynamics_hh

#include <lv2.h>

struct fps_io_plugins_relative_dynamics {
    struct fps_io_plugins_relative_dynamics_state state;
    float *ports[8];
};

enum port_indices {
    in = 0,
    out = 1,
    t1 = 2,
    t2 = 3,
    strength = 4,
    delay = 5,
    maxratio = 6,
    minratio = 7,
};

static void fps_io_plugins_relative_dynamics_connect_port(LV2_Handle instance, uint32_t port, void *data_location)
{
    if (port < 8) {
        ((struct fps_io_plugins_relative_dynamics*)instance)->ports[port] = (float*)data_location;
    }
}

#endif // fps_io_plugins_relative_dynamics_hh
```
