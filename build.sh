PATH=../regexec/:$PATH LD_LIBRARY_PATH=$(dirname $(which lv2ls))/../lib make test doc
sord_validate $(find -L /nix/store/rk0sxw0bb9580wcffdbyj494rz9ym1f2-lv2-1.18.2/lib/lv2/ -iname "*.ttl") $PWD/lv2/example.lv2/*.ttl 2>&1
