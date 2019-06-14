project('videospy', 'c', default_options : ['buildtype=debugoptimized'])

sdl2 = dependency('SDL2')
libsndfile = dependency('sndfile')
executable('videospy', ['src/main.c', 'src/sdlstuff.c'], dependencies : [sdl2, libsndfile], install : true)