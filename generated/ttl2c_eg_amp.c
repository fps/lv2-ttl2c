#ifndef eg_amp_hh
#define eg_amp_hh
    
  
#include <lv2.h>
#include <stdlib.h>
#include <string.h>
    
static void eg_amp_connect_port_desc(LV2_Handle instance, uint32_t port, void *data_location)
{
    if (eg_amp_callbacks.connect_port) 
    { 
        eg_amp_callbacks.connect_port(instance, port, data_location); 
    } 
    else 
    {
        if (port < 3) 
        {
            ((struct eg_amp*)instance)->ports[port] = (float*)data_location;
        }
    }
}

static LV2_Handle eg_amp_instantiate_desc(const LV2_Descriptor *descriptor, double sample_rate, const char *bundle_path, const LV2_Feature *const *features)
{
    struct eg_amp *instance = malloc(sizeof(struct eg_amp));
    memset(instance, 0,  sizeof(struct eg_amp));
    if (eg_amp_callbacks.instantiate)
    {
        eg_amp_callbacks.instantiate(instance, sample_rate, bundle_path, features);
    }
    return (LV2_Handle)(instance);
}

static void eg_amp_cleanup_desc(LV2_Handle instance)
{
    struct eg_amp *tinstance = (struct eg_amp*)instance;

    if (eg_amp_callbacks.cleanup)
    {
        eg_amp_callbacks.cleanup(tinstance);
    }

    free(tinstance);
}

static void eg_amp_activate_desc(LV2_Handle instance)
{
    if (eg_amp_callbacks.activate)
    {
        eg_amp_callbacks.activate(instance);
    }
}

static void eg_amp_deactivate_desc(LV2_Handle instance)
{
    if (eg_amp_callbacks.deactivate)
    {
        eg_amp_callbacks.deactivate(instance);
    }
}

static void eg_amp_run_desc(LV2_Handle instance, uint32_t sample_count)
{
    struct eg_amp *tinstance = (struct eg_amp*)instance;

    if (eg_amp_callbacks.run)
    {
        const struct eg_amp_port_gain gain = { .data = tinstance->ports[0] };
        const struct eg_amp_port_in in = { .data = tinstance->ports[1] };
        const struct eg_amp_port_out out = { .data = tinstance->ports[2] };

        eg_amp_callbacks.run(tinstance, sample_count, gain, in, out);
    }
}

static const void *eg_amp_extension_data_desc(const char *uri)
{
    if (eg_amp_callbacks.extension_data)
    {
        return eg_amp_callbacks.extension_data(uri);
    } 
    else 
    {
        return 0;
    }
}



static LV2_Descriptor eg_amp_descriptor = 
{
    "http://lv2plug.in/plugins/eg-amp",
    eg_amp_instantiate_desc,
    eg_amp_connect_port_desc,
    eg_amp_activate_desc,
    eg_amp_run_desc,
    eg_amp_deactivate_desc,
    eg_amp_cleanup_desc,
    eg_amp_extension_data_desc
};

LV2_SYMBOL_EXPORT const LV2_Descriptor* lv2_descriptor (uint32_t index)
{
    if (0 == index) 
    {
          return &eg_amp_descriptor;
    }
    else 
    { 
          return NULL;
    }
}


#endif // eg_amp_hh    