# setup.py
from setuptools import setup

setup(
    name = 'lv2-ttl2c',
    version = '1.0.2',
    scripts = [ 'lv2-ttl2c' ],
    packages = [ ],
    include_package_data = True,
    install_requires = [ 'jinja2' ],
)

