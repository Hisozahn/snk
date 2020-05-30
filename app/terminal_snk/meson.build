extra_deps = []
sources = ['main.c']

if (host_machine.system() == 'linux')
    extra_deps += cc.find_library('ncursesw')
    sources += ['terminal_linux.c']
elif (host_machine.system() == 'windows')
    sources += ['terminal_win.c']
endif

executable('terminal_snk',
    sources,
    dependencies: [
        dep_libsnk,
        extra_deps,
    ],
)