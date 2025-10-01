inc = -I../dependencies/include/ -I../dependencies/myinclude -I../dependencies/include/freetype
src = ./main.c ../sources/glad.c ../sources/OGL_utils.c ../sources/FLS_utils.c ../sources/TXT_utils.c ../sources/BUF_utils.c ../sources/FNT_utils.c
tar = ./release/prog.exe
loc = -L../dependencies/libs/
lib = -lopengl32 -lgdi32 -luser32 -lfreetype

ALL:
	clang $(inc) -o $(tar) $(src) $(loc) $(lib)
RUN:
	$(tar)
