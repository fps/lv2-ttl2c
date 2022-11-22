.PHONY: all

all: test

test:
	python ./lv2-ttl2c.py -b lv2/example.lv2/manifest.ttl && gcc test.c  -Wall -shared -o lv2/example.lv2/amp.so
	LV2_PATH=${PWD}/lv2 lv2ls
	LV2_PATH=${PWD}/lv2 lv2info http://lv2plug.in/plugins/eg-amp
	LV2_PATH=${PWD}/lv2 valgrind lv2bench http://lv2plug.in/plugins/eg-amp

