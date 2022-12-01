#include "generated/ttl2c_eg_atom.h"

static void run (
    plugin_t *instance, uint32_t nframes, 
    const plugin_port_gain_t gain, 
    const plugin_port_in_t in, 
    const plugin_port_out_t out,
    const plugin_port_atom_t atom
) {
    for (uint32_t frame = 0; frame < nframes; ++frame) {
        out.data[frame] = gain.data * in.data[frame];
    }
}

static const plugin_callbacks_t plugin_callbacks = {
    .run = run
};

#include "generated/ttl2c_eg_atom.c"

