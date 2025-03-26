inc = -I./dependencies/include/ -I./dependencies/myinclude -I./dependencies/include/freetype
src =  ./sources/main.c ./sources/glad.c ./sources/mymath.c ./sources/sdffont.c ./sources/fileutils.c ./sources/myglobals.c  ./sources/mystring.c ./sources/ttfutils.c
tar = ./release/prog.exe
loc = -L./dependencies/libs/
lib = -lSDL3 -lfreetype

ALL:
	gcc $(inc) -o $(tar) $(src) $(loc) $(lib)
RUN:
	$(tar)
