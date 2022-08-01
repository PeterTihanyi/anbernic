echo Cross-compiling AARCH64 file: template
aarch64-linux-gnu-g++-10 src/template.cpp -D HANDHELD -L SDL2/aarch64 -I SDL2/include -l SDL2 -o template

echo Copying AARCH64 files into ports/Template: template, libSDL2.so
cp template ports/Template/
cp SDL2/aarch64/libSDL2.so ports/Template/

