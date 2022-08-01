echo Compiling for x86-64, using the local copy of SDL2/x86-64/libSDL2.so
g++ src/template.cpp -D HANDHELD -L SDL2/x86-64 -I SDL2/include -l SDL2 -o template

