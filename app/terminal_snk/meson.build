extra_deps = []
sources = ['main.c']

if (host_machine.system() == 'linux')
    extra_deps += cc.find_library('ncursesw')
    sources += ['terminal_linux.c']
elif (host_machine.system() == 'darwin')
    extra_deps += cc.find_library('ncurses')
    sources += ['terminal_linux.c']
elif (host_machine.system() == 'windows')
    sources += ['terminal_win.c']
else
    error('Unsupported host machine system: ' + host_machine.system())
endif

executable('terminal_snk',
    sources,
    dependencies: [
        dep_libsnk,
        extra_deps,
    ],
)

executable('testsnk',
    ['main.c', 'terminal_common.c'],
    dependencies: [
        dep_libsnk,
        extra_deps,
    ],
)
