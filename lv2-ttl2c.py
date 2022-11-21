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

args = argparser.parse_args()

bundle_file_uri = pathlib.PurePath.as_uri(pathlib.Path(os.path.abspath(args.bundle)))

# print(bundle_file_uri)

w = lilv.World()
w.load_bundle(bundle_file_uri)

for plugin in w.get_all_plugins():
    basename = str(plugin.get_uri()).split("://")[1].replace(":","_").replace("/","_").replace("-","_").replace(".","_")    
    print(f'#ifndef {basename}_hh')
    print(f'#define {basename}_hh')
    print(f'')
    print(f'#include <lv2.h>')
    print(f'')
    print(f'struct {basename} {{')
    print(f'    struct {basename}_state state;')
    print(f'    float *ports[{plugin.get_num_ports()}];')
    print(f'}};')     
    print(f'')
    print(f'enum port_indices {{')
    for index in range(plugin.get_num_ports()):
        port = plugin.get_port_by_index(index)
        print(f'    {port.get_symbol()} = {index},')
    print(f'}};')
    print(f'')
    print(f'static void {basename}_connect_port(LV2_Handle instance, uint32_t port, void *data_location)')
    print(f'{{')
    print(f'    if (port < {plugin.get_num_ports()}) {{')
    print(f'        ((struct {basename}*)instance)->ports[port] = (float*)data_location;')
    print(f'    }}')
    print(f'}}')
    print(f'')
    print(f'#endif // {basename}_hh')



