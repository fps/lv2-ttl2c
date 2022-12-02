PATH=../regexec/:$PATH LD_LIBRARY_PATH=$(dirname $(which lv2ls))/../lib LV2_TTL_PATH=/nix/store/rk0sxw0bb9580wcffdbyj494rz9ym1f2-lv2-1.18.2/lib/lv2/  make test doc
