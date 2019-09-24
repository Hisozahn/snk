#!/bin/bash

build_dir='snk_build'

if [[ ! -e "${build_dir}" ]]; then
    mkdir "${build_dir}"
    meson "${build_dir}"
fi

cd "${build_dir}"
ninja
