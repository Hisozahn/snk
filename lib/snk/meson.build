extra_args = [
    # Warnings related
    '-Werror',
    '-Wall',
    '-Wextra',
    '-Wcast-align',
    '-Wcast-qual',
    '-Wdisabled-optimization',
    '-Wformat=2',
    '-Winit-self',
    '-Wlogical-op',
    '-Wmissing-include-dirs',
    '-Wredundant-decls',
    '-Wshadow',
    '-Wstrict-overflow=5',
    '-Wundef',
    '-Wunused',
    '-Wno-variadic-macros',
    '-Wmaybe-uninitialized',
    '-Wuninitialized',
    '-fdiagnostics-show-option',
    # Make binaries independent of libc
    '-nostdlib',
    '-ffreestanding',
    # C standard
    '-std=c89',
]

sources = files(
    'snk.c',
    'snk_joint.c',
    'snk_position.c',
    'snk_snake.c',
    'snk_util.c',
)

headers = files(
    'snk_defs.h',
    'snk_joint.h',
    'snk_position.h',
    'snk_snake.h',
    'snk_util.h',
    'snk.h',
)