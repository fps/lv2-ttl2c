import os

print(__file__)

template_header = open(os.path.dirname(__file__) + '/templates/plugin.h', 'r').read()
template_source = open(os.path.dirname(__file__) + '/templates/plugin.c', 'r').read()
