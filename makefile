.PHONY: test clean all

# EXTRA_CFLAGS ?= -march=native -mcpu=native -O3 -Wall -Werror -pedantic
EXTRA_CFLAGS ?= -g -O1 -Wall -Werror -pedantic -fPIC

LV2_TTL_PATH ?= /usr/lib/lv2

all: plugins 

PLUGINS = amp exp midigate
PLUGIN_LIBRARIES = $(PLUGINS:%=lv2/example.lv2/%.so)

plugins: $(PLUGIN_LIBRARIES)

lv2/example.lv2/%.so: %.c generated/done makefile
	gcc ${EXTRA_CFLAGS} $< -shared -o $@

generated/done: lv2/example.lv2/*.ttl lv2-ttl2c lv2_ttl2c/templates/*
	./lv2-ttl2c -b lv2/example.lv2 -o generated 
	touch generated/done

test: plugins
	sord_validate $$(find -L ${LV2_TTL_PATH} -iname "*.ttl") ${PWD}/lv2/example.lv2/*.ttl 2>&1
	LV2_PATH=${PWD}/lv2 lv2ls
	for n in $(PLUGINS); do LV2_PATH=${PWD}/lv2 lv2info http://lv2plug.in/plugins/eg-"$$n"; done
	for n in $(PLUGINS); do LV2_PATH=${PWD}/lv2 valgrind --leak-check=full lv2bench http://lv2plug.in/plugins/eg-"$$n"; done

doc: README.md 

README.md: README.md.in *.c makefile generated/done
	cat README.md.in | regexec | regexec -e "\[usage\]" -c "./lv2-ttl2c -h" -n 1 > README.md

clean:
	rm -f lv2/example.lv2/*.so generated/*.h generated/*.c generated/done lv2/example.lv2/done
