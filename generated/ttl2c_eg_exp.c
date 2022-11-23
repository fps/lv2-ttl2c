#ifndef eg_exp_hh
#define eg_exp_hh
    
  
#include <lv2.h>
#include <stdlib.h>
#include <string.h>
    
enum eg_exp_port_indices {
    t1 = 0,
    in = 1,
    out = 2,
};

static void eg_exp_connect_port_desc(LV2_Handle instance, uint32_t port, void *data_location)
{
    if (eg_exp_callbacks.connect_port) 
    { 
        eg_exp_callbacks.connect_port(instance, port, data_location); 
    } 
    else 
    {
        if (port < 3) 
        {
            ((struct eg_exp*)instance)->ports[port] = (float*)data_location;
        }
    }
}

static LV2_Handle eg_exp_instantiate_desc(const LV2_Descriptor *descriptor, double sample_rate, const char *bundle_path, const LV2_Feature *const *features)
{
    struct eg_exp *instance = malloc(sizeof(struct eg_exp));
    memset(instance, 0,  sizeof(struct eg_exp));
    if (eg_exp_callbacks.instantiate)
    {
        eg_exp_callbacks.instantiate(instance, sample_rate, bundle_path, features);
    }
    return (LV2_Handle)(instance);
}

static void eg_exp_cleanup_desc(LV2_Handle instance)
{
    struct eg_exp *tinstance = (struct eg_exp*)instance;

    if (eg_exp_callbacks.cleanup)
    {
        eg_exp_callbacks.cleanup(tinstance);
    }

    free(tinstance);
}

static void eg_exp_activate_desc(LV2_Handle instance)
{
    if (eg_exp_callbacks.activate)
    {
        eg_exp_callbacks.activate(instance);
    }
}

static void eg_exp_deactivate_desc(LV2_Handle instance)
{
    if (eg_exp_callbacks.deactivate)
    {
        eg_exp_callbacks.deactivate(instance);
    }
}

static void eg_exp_run_desc(LV2_Handle instance, uint32_t sample_count)
{
    struct eg_exp *tinstance = (struct eg_exp*)instance;

    if (eg_exp_callbacks.run)
    {
        eg_exp_callbacks.run(tinstance, sample_count, tinstance->ports[0], tinstance->ports[1], tinstance->ports[2]);
    }
}

static const void *eg_exp_extension_data_desc(const char *uri)
{
    if (eg_exp_callbacks.extension_data)
    {
        return eg_exp_callbacks.extension_data(uri);
    } 
    else 
    {
        return 0;
    }
}



static LV2_Descriptor eg_exp_descriptor = 
{
    "http://lv2plug.in/plugins/eg-exp",
    eg_exp_instantiate_desc,
    eg_exp_connect_port_desc,
    eg_exp_activate_desc,
    eg_exp_run_desc,
    eg_exp_deactivate_desc,
    eg_exp_cleanup_desc,
    eg_exp_extension_data_desc
};

LV2_SYMBOL_EXPORT const LV2_Descriptor* lv2_descriptor (uint32_t index)
{
    if (0 == index) 
    {
          return &eg_exp_descriptor;
    }
    else 
    { 
          return NULL;
    }
}


#endif // eg_exp_hh    