import os
import jinja2

environment = jinja2.Environment(loader = jinja2.FileSystemLoader(os.path.dirname(__file__) + "/templates/"), trim_blocks = True)

# template_header = open(os.path.dirname(__file__) + '/templates/plugin.h', 'r').read()
# template_source = open(os.path.dirname(__file__) + '/templates/plugin.c', 'r').read()
