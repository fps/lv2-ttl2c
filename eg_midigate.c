// This example is adapted from the eg-midigate plugin shipped
// with the LV2 distribution

#include "generated/ttl2c_eg_midigate.h"
#include <stdlib.h>
#include <string.h>

typedef struct plugin_state {
    unsigned n_active_notes;
    unsigned program; // 0 = normal, 1 = inverted
} plugin_state_t;

static plugin_t* instantiate(
    plugin_t *instance, double sample_rate,
    const char *bundle_path, const LV2_Feature *const *features
) {
    instance->state = malloc(sizeof(plugin_state_t));
    memset(instance->state, 0, sizeof(plugin_state_t));
    return instance;
}

static void write_output(
    plugin_t* self, uint32_t offset, uint32_t len,
    const plugin_port_in_t in,
    const plugin_port_out_t out
) {
    plugin_state_t *state = self->state;

    const bool active = (state->program == 0) ? (state->n_active_notes > 0)
                                           : (state->n_active_notes == 0);
    if (active) {
        memcpy(out.data + offset, in.data + offset, len * sizeof(float));
    } else {
        memset(out.data + offset, 0, len * sizeof(float));
    }
}

static void run (
    plugin_t *instance, uint32_t nframes, 
    const plugin_port_control_t control,
    const plugin_port_in_t in,
    const plugin_port_out_t out
) {
    plugin_state_t *state = instance->state;
    uint32_t  offset = 0;

    LV2_ATOM_SEQUENCE_FOREACH (control.data, ev) {
        if (ev->body.type == instance->midi_MidiEvent) {
            const uint8_t* const msg = (const uint8_t*)(ev + 1);
            switch (lv2_midi_message_type(msg)) {
            case LV2_MIDI_MSG_NOTE_ON:
                    ++state->n_active_notes;
                break;
            case LV2_MIDI_MSG_NOTE_OFF:
                if (state->n_active_notes > 0) {
                    --state->n_active_notes;
                }
                break;
            case LV2_MIDI_MSG_CONTROLLER:
                if (msg[1] == LV2_MIDI_CTL_ALL_NOTES_OFF) {
                    state->n_active_notes = 0;
                }
                break;
            case LV2_MIDI_MSG_PGM_CHANGE:
                if (msg[1] == 0 || msg[1] == 1) {
                    state->program = msg[1];
                }
                break;
            default:
                break;
            }
        }

        write_output(instance, offset, ev->time.frames - offset, in, out);
        offset = (uint32_t)ev->time.frames;
    }

    write_output(instance, offset, nframes - offset, in, out);
}

static void cleanup(plugin_t *instance) {
    free(instance->state);
}

static const plugin_callbacks_t plugin_callbacks = {
    .instantiate = instantiate,
    .run = run,
    .cleanup = cleanup,
};

#include "generated/ttl2c_eg_midigate.c"

