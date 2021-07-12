libs = [
    'snk',
    'snk_config',
]

foreach lib : libs
    extra_args = []
    extra_deps = []
    sources = []
    includes = []

    subdir(lib)

    includes += include_directories(lib)
    install_headers(headers)

    lib_obj = library(
        lib,
        sources,
        c_args: extra_c_args,
        link_args: extra_link_args,
        include_directories: includes,
        install: true,
        install_dir: 'lib',
        dependencies: extra_deps,
    )
    dep = declare_dependency(link_with: lib_obj, include_directories: includes)

    set_variable('dep_lib' + lib, dep)

endforeach
