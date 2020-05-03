all_apps = [
    'terminal_snk',
]

build_apps = get_option('build_apps').split(',')

foreach a : all_apps
    if build_apps.contains(a)
        subdir(a)
    endif
endforeach