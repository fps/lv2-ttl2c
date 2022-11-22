#!/usr/bin/env python

import lilv
import argparse
import sys
import pprint
import os
import pathlib

argparser = argparse.ArgumentParser(
    prog = 'lv2-ttl2c.py',
    description = 'Generate useful C code from a LV2 plugin bundle\'s metadata')

argparser.add_argument('-b', '--bundle', default='.', help='the bundle directory to analyze')
argparser.add_argument('-o', '--output-directory', default='.', help='the output directory')

args = argparser.parse_args()

bundle_file_uri = pathlib.PurePath.as_uri(pathlib.Path(os.path.abspath(args.bundle)))

# print(bundle_file_uri)

w = lilv.World()
w.load_bundle(bundle_file_uri)

for plugin in w.get_all_plugins():
    basename = str(plugin.get_uri()).split("://")[1].replace(":","_").replace("/","_").replace("-","_").replace(".","_")    
    f = open(os.path.join(args.output_directory, f'{basename}.h'), 'w')
    f.write(f"""\
#ifndef {basename}_hh
#define {basename}_hh
    
  
#include <lv2.h>
#include <stdlib.h>
    
struct {basename} {{
    struct {basename}_state *state;
    float *ports[{plugin.get_num_ports()}];
}};

enum {basename}_port_indices {{\
    """)

    for index in range(plugin.get_num_ports()):
        port = plugin.get_port_by_index(index)
        f.write(f'    {port.get_symbol()} = {index},\n')

    f.write(f"""\
}};



// LV2_Handle(*instantiate_cb)(const struct LV2_Descriptor *descriptor, double sample_rate, const char *bundle_path, const LV2_Feature *const *features);
void (*{basename}_connect_port_cb)(LV2_Handle instance, uint32_t port, void *data_location) = NULL;
// void(*activate_cb)(LV2_Handle instance);
// void(*run_cb)(LV2_Handle instance, uint32_t sample_count);
// void(*deactivate_cb)(LV2_Handle instance);
// void(*cleanup_cb)(LV2_Handle instance);
// const void *(*extension_data_cb)(const char *uri);

static void {basename}_connect_port_desc(LV2_Handle instance, uint32_t port, void *data_location)
{{
    if ({basename}_connect_port_cb) 
    {{ 
        {basename}_connect_port_cb(instance, port, data_location); 
    }} 
    else 
    {{
        if (port < {plugin.get_num_ports()}) {{
            ((struct {basename}*)instance)->ports[port] = (float*)data_location;
        }}
    }}
}}

static LV2_Handle {basename}_instantiate_desc(const LV2_Descriptor *descriptor, double sample_rate, const char *bundle_path, const LV2_Feature *const *features)
{{
    struct {basename} *instance = malloc(sizeof(struct {basename}));
    return (LV2_Handle)(instance);
}}

static void {basename}_cleanup_desc(LV2_Handle instance)
{{
    struct {basename} *tinstance = (struct {basename}*)instance;
    free(tinstance);
}}

static void {basename}_activate_desc(LV2_Handle instance)
{{
}}

static void {basename}_deactivate_desc(LV2_Handle instance)
{{
}}

static void {basename}_run_desc(LV2_Handle instance, uint32_t sample_count)
{{
}}

static const void *{basename}_extension_data_desc(const char *uri)
{{
}}



static LV2_Descriptor {basename}_descriptor = 
{{
    "{plugin.get_uri()}",
    {basename}_instantiate_desc,
    {basename}_connect_port_desc,
    {basename}_activate_desc,
    {basename}_run_desc,
    {basename}_deactivate_desc,
    {basename}_cleanup_desc,
    {basename}_extension_data_desc
}};


#endif // {basename}_hh\
    """)



