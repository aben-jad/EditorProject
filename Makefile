inc = -I./dependencies/include/ -I./dependencies/myinclude
src2 = ./sources/main.c ./sources/glad.c ./sources/IDE_utils.c ./sources/OGL_utils.c ./sources/FLS_utils.c
src = ./sources/main.c
tar = ./release/prog.exe
loc = -L./dependencies/libs/
lib = -lfreetype

ALL:
	gcc $(inc) -o $(tar) $(src) $(loc) $(lib)
RUN:
	$(tar)
