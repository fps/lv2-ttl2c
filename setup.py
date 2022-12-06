# setup.py
from setuptools import setup

setup(
    name = 'lv2-ttl2c',
    version = '1.0.2',
    scripts = [ 'lv2-ttl2c/bin/lv2-ttl2c' ],
    packages = [ 'lv2-ttl2c' ],
    include_package_data = True,
    install_requires = [ 'jinja2' ],
)

