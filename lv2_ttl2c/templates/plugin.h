#ifndef plugin_cb_hh
#define plugin_cb_hh

#include <lv2.h>
#include <lv2/log/logger.h>
#include <lv2/core/lv2_util.h>
#include <stdint.h>
#include <lv2/atom/atom.h>
#include <lv2/atom/util.h>
#include <lv2/midi/midi.h>

typedef struct plugin_state plugin_state_t;

typedef struct {
    plugin_state_t *state;
    void *ports[{{ ports|length }}];
{% if needs_urid_map %}
    LV2_URID_Map *map;
{% endif %}
{% if has_midi_ports %}
    LV2_URID midi_MidiEvent;
{% endif %}
    LV2_Log_Logger logger;
} plugin_t;

{% for port in ports %}
typedef struct {
    {% if port.is_atom_port %}LV2_Atom_Sequence {% else %}float {% endif %}{% if port.is_input_port %}const {% endif %}{% if not port.is_control_port%}* const {% endif %}data;
} plugin_port_{{ port.symbol }}_t;

{% endfor %}
typedef struct {
    plugin_t* (*const instantiate)(plugin_t *instance, double sample_rate, const char *bundle_path, const LV2_Feature *const *features);
    void (*const connect_port)(plugin_t *instance, uint32_t port, void *data_location);
    void (*const activate)(plugin_t *instance);
    void (*const run)(plugin_t *instance, uint32_t sample_count{% for port in ports %}, plugin_port_{{ port.symbol }}_t {{ port.symbol }}{% endfor %});
    void (*const deactivate)(plugin_t *instance);
    void (*const cleanup)(plugin_t *instance);
    const void *(*const extension_data)(const char *uri);
} plugin_callbacks_t;

#endif    
