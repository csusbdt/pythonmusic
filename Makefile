# To generate an empty plist.

default : run

clean :
	rm -rf module.o module.so


module.so : module.c 
	#if [ ! -d build ]; then mkdir -p build; fi
	#if [ -e build/a.out ]; then rm build/a.out; fi
	clang 	-c                                 \
		module.c                           \
		-o module.o                        \
		`sdl2-config --cflags`             \
		-fPIC                              \
		-Wall 
	clang 	-bundle                            \
		module.o                           \
		-o module.so                       \
		-undefined dynamic_lookup          \
		`sdl2-config --libs`


#
#	clang 	-o build/a.out               \
#		-mmacosx-version-min=10.5    \
#		-D _THREAD_SAFE              \
#		-D SDL_ASSERT_LEVEL=2        \
#		-I include                   \
#		-I lua                       \
#		-L osx/lib                   \
#		-l SDL2                      \
#		-l z                         \
#		-l bz2                       \
#		-l iconv                     \
#		-l objc                      \
#		-l lua                       \
#		-Wl,-framework,OpenGL        \
#		-Wl,-framework,ForceFeedback \
#		-Wl,-framework,Cocoa         \
#		-Wl,-framework,Carbon        \
#		-Wl,-framework,IOKit         \
#		-Wl,-framework,CoreAudio     \
#		-Wl,-framework,AudioToolbox  \
#		-Wl,-framework,AudioUnit     \
#		src/*.c


run : module.so
	python main.py

