#ifndef ttl2c_plugin_cc
#define ttl2c_plugin_cc
    
static void plugin_connect_port_desc (LV2_Handle instance, uint32_t port, void *data_location) {
    plugin_t *tinstance = (plugin_t*) instance;

    if (plugin_callbacks.connect_port) {
        plugin_callbacks.connect_port(tinstance, port, data_location);
    } else {
        if (port < {{ ports | length }}) {
            (tinstance)->ports[port] = data_location;
        }
    }
}

static LV2_Handle plugin_instantiate_desc (const LV2_Descriptor *descriptor, double sample_rate, const char *bundle_path, const LV2_Feature *const *features) {
    plugin_t *instance = (plugin_t*)malloc(sizeof(plugin_t));

    if (!instance) {
        return NULL;
    }

    memset(instance, 0,  sizeof(plugin_t));

    lv2_features_query(features, LV2_LOG__log, &instance->logger.log, false, NULL);

{% if needs_urid_map %}
    const char* missing =  lv2_features_query (features, LV2_URID__map, &instance->map, true, NULL);
    if (missing) {
        lv2_log_note(&instance->logger, "Missing feature: %s\n", missing);
        free(instance);
        return NULL;
    }
{% endif %}

    if (plugin_callbacks.instantiate) {
        instance = plugin_callbacks.instantiate (instance, sample_rate, bundle_path, features);
    }

    return (LV2_Handle)(instance);
}

static void plugin_cleanup_desc (LV2_Handle instance) {
    plugin_t *tinstance = (plugin_t*) instance;

    if (plugin_callbacks.cleanup) {
        plugin_callbacks.cleanup (tinstance);
    }

    free(tinstance);
}

static void plugin_activate_desc (LV2_Handle instance) {
    plugin_t *tinstance = (plugin_t*) instance;

    if (plugin_callbacks.activate) {
        plugin_callbacks.activate (tinstance);
    }
}

static void plugin_deactivate_desc (LV2_Handle instance) {
    plugin_t *tinstance = (plugin_t*) instance;

    if (plugin_callbacks.deactivate) {
        plugin_callbacks.deactivate (tinstance);
    }
}

static void plugin_run_desc (LV2_Handle instance, uint32_t sample_count) {
    if (plugin_callbacks.run) {
        plugin_t *tinstance = (plugin_t*) instance;

{% for port in ports %}
        plugin_port_{{ port.symbol }}_t const {{ port.symbol }} = { .data = (({% if port.is_atom_port %}LV2_Atom_Sequence{% else %}float{% endif %}*)((plugin_t*)instance)->ports[{{ port.index }}]){% if port.is_control_port %}[0]{% endif %} };
{% endfor %}

        plugin_callbacks.run (tinstance, sample_count{% for port in ports %}, {{ port.symbol }}{% endfor %});
    }
}

static const void *plugin_extension_data_desc (const char *uri) {
    if (plugin_callbacks.extension_data) {
        return plugin_callbacks.extension_data (uri);
    } else {
        return 0;
    }
}

static LV2_Descriptor plugin_descriptor = {
    "{{ uri }}",
    plugin_instantiate_desc,
    plugin_connect_port_desc,
    plugin_activate_desc,
    plugin_run_desc,
    plugin_deactivate_desc,
    plugin_cleanup_desc,
    plugin_extension_data_desc
};

LV2_SYMBOL_EXPORT const LV2_Descriptor* lv2_descriptor (uint32_t index) {
    if (0 == index) {
          return &plugin_descriptor;
    } else { 
          return NULL;
    }
}


#endif // plugin_hh
