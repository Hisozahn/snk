extra_deps = []
sources = ['main.c']
c_std = 'c89'

if (host_machine.system() == 'linux')
    extra_deps += cc.find_library('ncursesw')
    c_std = 'gnu89'
    sources += ['terminal_linux.c']
elif (host_machine.system() == 'windows')
    sources += ['terminal_win.c']
endif

executable('terminal_snk',
    sources,
    c_args: [ '-std=' + c_std],
    dependencies: [
        dep_libsnk,
        extra_deps,
    ],
)