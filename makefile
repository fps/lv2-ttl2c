.PHONY: test

all: plugins 

plugins: *.c lv2/example.lv2/*.ttl
	./lv2-ttl2c -b lv2/example.lv2 -o generated 
	gcc eg_amp.c -pedantic -Wall -Werror -shared -o lv2/example.lv2/amp.so
	gcc eg_exp.c -pedantic -Wall -Werror -shared -o lv2/example.lv2/exp.so

test: plugins
	LV2_PATH=${PWD}/lv2 lv2ls
	LV2_PATH=${PWD}/lv2 lv2info http://lv2plug.in/plugins/eg-amp
	LV2_PATH=${PWD}/lv2 valgrind lv2bench http://lv2plug.in/plugins/eg-amp
	LV2_PATH=${PWD}/lv2 valgrind lv2bench http://lv2plug.in/plugins/eg-exp

doc: README.md.in *.c
	cat README.md.in | regexec | regexec -e "\[usage\]" -c "./lv2-ttl2c -h" -n 1 > README.md
